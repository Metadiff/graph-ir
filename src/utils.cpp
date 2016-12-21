//
// Created by alex on 30/09/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{

        std::logic_error throw_op_ige(std::string name, std::string msg) {
            op_logger(name)->error(msg);
            return InternalGraphError(name, msg);
        }

        std::invalid_argument throw_op_iae(NodeVec nodes, std::string name, std::string msg) {
            op_logger(name)->error(msg);
            return InvalidOperatorArgument(nodes, name, msg);
        }

        Logger logger(std::string const name, LogLevel const level){
            Logger ptr = spdlog::get(name);
            if (not ptr) {
                ptr = std::make_shared<spdlog::logger>(name, gir_sink());
                spdlog::register_logger(ptr);
                ptr->set_level(level);
                ptr->set_pattern("[%Y-%m-%d %H:%M:%S][%l][%n] %v");
            }
            return ptr;
        }

        Logger g_logger(std::string const name, LogLevel const level){
            return logger("graph::" + name, level);
        }

        Logger op_logger(std::string const name, LogLevel const level){
            return logger("op::" + name, level);
        }

        Logger backend_logger(std::string const name, LogLevel const level){
            return logger("backend::" + name, level);
        }

        Logger function_logger(std::string const name, LogLevel const level){
            return logger("function::" + name, level);
        }

        int byte_size(DataType data_type){
            const int factors[4] = {1, 2, 4, 8};
            if(data_type.type == COMPLEX){
                return factors[data_type.precision] * 2;
            } else {
                return factors[data_type.precision];
            }
        }

        SymInt number_of_elements(Shape const shape){
            return shape[0] * shape[1] * shape[2] * shape[3];
        }

        Operator get_base_op(Operator const op) {
            Operator base_op = op;
            while (base_op->name == "Alias") {
                base_op = base_op->get_parents()[0]->op;
            }
            return base_op;
        }

        Node get_base_node(Node const node){
            return get_base_op(node->op)->result;
        }

        bool symbolic_equals(Node const & node1, Node const & node2) {
            if (node1->id == node2->id) {
                return true;
            }
            if(node1->is_input_dependent != node2->is_input_dependent){
                return false;
            }
            if(node1->is_differentiable != node2->is_differentiable){
                return false;
            }
            auto base_op1 = get_base_op(node1->op);
            auto base_op2 = get_base_op(node2->op);
            return base_op1->equals(base_op2) or base_op2->equals(base_op1);
        };


        Shape get_max_shape(NodeVec const & nodes, std::string const op_name){
            Shape shape = {1, 1, 1, 1};
            for(auto i=0; i < nodes.size(); ++i){
                for(auto j=0; j < 4; ++j){
                    if(shape[j] != nodes[i]->shape[j]){
                        if(shape[j] == 1){
                            shape[j] = nodes[i]->shape[j];
                        } else if (nodes[i]->shape[j] != 1){
                            throw throw_op_iae(nodes, op_name, "Incompatible shapes in nodes: \n" + to_string(nodes));
                        }
                    }
                }
            }
            return shape;
        }

        Node implicit_broadcast(Node const node, Shape const shape, std::string const op_name) {
            if(node->shape != shape and node.order() > 0){
                Graph g = node.g();
                switch (g->props.policies.implicit_broadcast){
                    case RAISE:
                        throw throw_op_iae(NodeVec{node}, op_name,
                                           fmt::format("Implicit broadcast of node {} with shape {} to shape {}.",
                                                       node->id, to_string(node->shape), to_string(shape)));
                    case WARN:
                        op_logger(op_name)->warn("Implicit broadcast of node {} with shape {} to shape {}.",
                                                 node->id, to_string(node->shape), to_string(shape));
                    default: ;
                }
                return broadcast(node, shape);
            } else {
                return node;
            }
        }

        void verify_shapes_and_broadcast(NodeVec & nodes, std::string const op_name) {
            auto max_shape = get_max_shape(nodes, op_name);
            for(auto i=0; i < nodes.size(); ++i){
                nodes[i] = implicit_broadcast(nodes[i], max_shape, op_name);
            }
        }

        Node implicit_cast(Node const node, DataType const data_type, std::string const op_name){
            if(node->data_type != data_type){
                Graph g = node.g();
                switch (g->props.policies.implicit_cast){
                    case RAISE: {
                        throw throw_op_iae(NodeVec{node}, op_name,
                                           fmt::format("Implicit cast from type {} to type {}.",
                                                       to_string(node->data_type),
                                                       to_string(data_type)));
                    }
                    case WARN: {
                        op_logger(op_name)->warn("Implicit cast from type {} to type {}.",
                                                 to_string(node->data_type),
                                                 to_string(data_type));
                        break;
                    }
                    default: {}
                }
                return cast(node, data_type);
            } else {
                return node;
            }
        }

        void validate_axes(Shape const shape, Axes & axes, std::string const op_name){
            std::sort(axes.begin(), axes.end());
            for(auto i=0; i < axes.size(); ++i){
                if(shape[axes[i]] == 1){
                    throw throw_op_iae(NodeVec{}, op_name,
                                       fmt::format("Reduction on unit axis {} from shape {}.", axes[i], to_string(shape)));
                } else if(i > 0 and axes[i] == axes[i-1]){
                    throw throw_op_iae(NodeVec{}, op_name,
                                       fmt::format("Duplicate axis {}.", axes[i]));
                } else if(axes[i] < 0 or axes[i] > 3){
                    throw throw_op_iae(NodeVec{}, op_name,
                                       fmt::format("Axis is not in range [0, 3] - {}", axes[i]));
                }
            }
        }

        Axes auto_infer_axes(Shape const shape){
            Axes axes;
            for(auto i=0; i < 4; ++i){
                if(shape[i] != 1){
                    axes.push_back(i);
                }
            }
            return axes;
        }

        bool is_dependent(Node const anchor, Node const monitored){
            // TODO this should be implemented correctly
            return anchor->id > monitored->id;
        }

        std::vector<std::string> unique_dimensions(Graph graph) {
            std::set<std::string> unique;
            for(auto i=0; i < graph->nodes.size(); ++i){
                if(graph->nodes[i]->op->name == "SymIntWrapper"){
                    auto cast_op = std::dynamic_pointer_cast<op::SymIntWrapper>(graph->nodes[i]->op);
                    for (auto m = 0; m < cast_op->value.monomials.size(); ++m) {
                        for (auto p = 0; p < cast_op->value.monomials[m].powers.size(); ++p) {
                            auto candidate = cast_op->value.monomials[m].powers[p].first;
                            // Check that the candidate is not a floor/ceil/min/max
                            if (candidate.type == sym::Variable) {
                                unique.insert(candidate.id);
                            }
                        }
                    }
                } else {
                    auto shape = graph->nodes[i]->shape;
                    for (auto j = 0; j < 4; ++j) {
                        if (not shape[j].is_constant()) {
                            for (auto m = 0; m < shape[j].monomials.size(); ++m) {
                                for (auto p = 0; p < shape[j].monomials[m].powers.size(); ++p) {
                                    auto candidate = shape[j].monomials[m].powers[p].first;
                                    // Check that the candidate is not a floor/ceil/min/max
                                    if (candidate.type == sym::Variable) {
                                        unique.insert(candidate.id);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return std::vector<std::string>(unique.begin(), unique.end());
        }

        bool verify_shapes(std::vector<std::array<long, 4>> const & input_shapes,
                           std::vector<std::array<long, 4>> const & last_shapes,
                           NodeVec const & symbolic_inputs,
                           std::unordered_map<std::string, int64_t> & last_verified,
                           std::vector<std::pair<SymInt, int64_t >> implicit) {
            // Check if all shapes match
            bool changed = false;
            if(last_shapes.size() != input_shapes.size()){
                changed = true;
            } else {
                for(auto i = 0; i < input_shapes.size(); ++i){
                    if(last_shapes[i] != input_shapes[i]){
                        changed = true;
                        break;
                    }
                }
            }
            if(changed) {
                // If they have make variable deduction
                for (auto i = 0; i < symbolic_inputs.size(); ++i) {
                    for (auto j = 0; j < 4; ++j) {
                        if (not symbolic_inputs[i]->shape[j].is_constant()) {
                            implicit.push_back({symbolic_inputs[i]->shape[j], input_shapes[i][j]});
                        } else if (symbolic_inputs[i]->shape[j].eval({}) != input_shapes[i][j]) {
                            throw std::invalid_argument(fmt::format(
                                    "Incorrect shape of input at index {}. "
                                            "The shape on dimension {} does not match. "
                                            "Expected {}, but got {}.",
                                    i, j, symbolic_inputs[i]->shape[j].eval({}),
                                    input_shapes[i][j]));
                        }
                    }
                }
                if (implicit.size() > 0) {
                    last_verified = sym::deduce_values(implicit);
                    return true;
                }
            }
            return false;
        }
    }
}
