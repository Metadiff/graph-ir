//
// Created by alex on 30/09/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{
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
            Logger ptr = spdlog::get(name);
            if (not ptr) {
                ptr = std::make_shared<spdlog::logger>(name, gir_sink());
                spdlog::register_logger(ptr);
                ptr->set_level(level);
                ptr->set_pattern("[%Y-%m-%d %H:%M:%S][%l][graph::%n] %v");
            }
            return ptr;
        }

        Logger op_logger(std::string const name, LogLevel const level){
            Logger ptr = spdlog::get(name);
            if (not ptr) {
                ptr = std::make_shared<spdlog::logger>(name, gir_sink());
                spdlog::register_logger(ptr);
                ptr->set_level(level);
                ptr->set_pattern("[%Y-%m-%d %H:%M:%S][%l][op::%n] %v");
            }
            return ptr;
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
                            op_logger(op_name)->error("Incompatible shapes in nodes: \n{}", to_string(nodes));
                            throw InvalidOperatorArgument(nodes, op_name, "Incompatible shapes in nodes: \n"
                                                                          + to_string(nodes));
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
                        op_logger(op_name)->error("Implicit broadcast of node {} with shape {} to shape {}",
                                                  node->id, to_string(node->shape), to_string(shape));
                        throw InvalidOperatorArgument(NodeVec{node}, op_name,
                                                      "Implicit broadcast of node " + std::to_string(node->id)
                                                      + " with shape " + to_string(node->shape)
                                                      + " to shape "  + to_string(shape));
                    case WARN:
                        op_logger(op_name)->warn("Implicit broadcast of node {} with shape {} to shape {}",
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
                        op_logger(op_name)->error("Implicit cast from type {} to type {}",
                                                  to_string(node->data_type),
                                                  to_string(data_type));
                        throw InvalidOperatorArgument(NodeVec{node}, op_name,
                                                      "Attempting implicit cast from type "
                                                      + to_string(node->data_type)
                                                      + " to type " + to_string(data_type));
                    }
                    case WARN: {
                        op_logger(op_name)->warn("Implicit cast from type {} to type {}",
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
                    op_logger(op_name)->error("Reduction on unit axis {} from shape {}", axes[i], to_string(shape));
                    throw InvalidOperatorArgument(NodeVec{}, op_name, "Reduction on unit axis " + std::to_string(axes[i])
                                                                      + " from shape " + to_string(shape));
                } else if(i > 0 and axes[i] == axes[i-1]){
                    op_logger(op_name)->error("Duplicate axis {}", axes[i]);
                    throw InvalidOperatorArgument(NodeVec{}, op_name, "Duplicate axis " + std::to_string(axes[i]));
                } else if(axes[i] < 0 or axes[i] > 3){
                    op_logger(op_name)->error("Axis is not in range [0, 3] - {}", axes[i]);
                    throw InvalidOperatorArgument(NodeVec{}, op_name, "Axis is not in range [0, 3] -" + std::to_string(axes[i]));
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
    }
}
