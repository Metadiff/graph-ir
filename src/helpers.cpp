//
// Created by alex on 02/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{

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

        Node get_base_node(Node node){
            return get_base_op(node->op)->result;
        }

        bool validate_axes(Axes axes) {
            if (axes.size() > 4) {
                return false;
            }
            bool checks[4]{false, false, false, false};
            for (int i = 0; i < axes.size(); i++) {
                if (axes[i] > 3) {
                    return false;
                }
                if (checks[axes[i]]) {
                    return false;
                }
                checks[axes[i]] = true;
            }
            return true;
        }

        Node implicit_broadcast(Node node, Shape shape, std::string op_name) {
            if(node->shape != shape){
                Graph g = node.g();
                switch (g->props.policies.implicit_broadcast){
                    case RAISE:
                        g->logger()->error("[op::{}] Implicit broadcast of node {} with shape {} to shape {}",
                                           op_name, node->id, to_string(node->shape), to_string(shape));
                        throw InvalidOperatorArgument(NodeVec{node}, op_name,
                                                      "Implicit broadcast of node " + std::to_string(node->id)
                                                      + "with shape " + to_string(node->shape)
                                                      + "to shape "  + to_string(shape));
                    case WARN:
                        g->logger()->warn("[op::{}] Implicit broadcast of node {} with shape {} to shape {}",
                                          op_name, node->id, to_string(node->shape), to_string(shape));
                    default: ;
                }
                return broadcast(node, shape);
            } else {
                return node;
            }
        }

        Shape get_max_shape(NodeVec nodes, std::string op_name){
            Shape shape = {1, 1, 1, 1};
            for(auto i=0; i < nodes.size(); ++i){
                for(auto j=0; j < 4; ++j){
                    if(shape[j] != nodes[i]->shape[j]){
                        if(shape[j] == 1){
                            shape[j] = nodes[i]->shape[j];
                        } else if (nodes[i]->shape[j] != 1){
                            nodes[i].g()->logger()->error("[op::{}] Incompatible shapes in nodes: \n"
                                                                   "{}", op_name, to_string(nodes));
                            throw InvalidOperatorArgument(nodes, op_name, "Incompatible shapes in nodes: \n"
                                                                          + to_string(nodes));
                        }
                    }
                }
            }
            return shape;
        }

        NodeVec verify_shapes_and_broadcast(NodeVec nodes, std::string op_name) {
            auto max_shape = get_max_shape(nodes, op_name);
            for(auto i=0; i < nodes.size(); ++i){
                nodes[i] = implicit_broadcast(nodes[i], max_shape, op_name);
            }
            return nodes;
        }
//        NodeVec verify_shapes_and_broadcast(NodeVec nodes, std::string op_name) {
//            Shape max_shape = nodes[0]->shape;
//            for (auto i = 1; i < nodes.size(); i++) {
//                Shape node_shape = nodes[i]->shape;
//                bool max = false;
//                for (int j = 0; j < 4; j++) {
//                    if (node_shape[j] != 1 and max_shape[j] != 1 and node_shape[j] != max_shape[j]) {
//                        auto err = std::make_shared<IncompatibleShapes>(nodes, op_name);
//                        err->log(utils::op_logger(op_name));
//                        throw err;
//                    } else if(node_shape[j] != 1 and max_shape[j] == 1){
//                        max_shape[j] = node_shape[j];
//                    }
//                }
//            }
//            NodeVec outputs;
//            Policy implicit_broadcast = nodes[0].g()->props.policies.implicit_broadcast;
//            for (auto i = 0; i < nodes.size(); i++) {
//                if(nodes[i]->shape != max_shape){
//                    auto err = std::make_shared<ImplicitBroadcast>(nodes, op_name);
//                    operate_policy(implicit_broadcast, utils::op_logger(op_name), err);
//                    outputs.push_back(api::broadcast(nodes[i], max_shape));
//                } else {
//                    outputs.push_back(nodes[i]);
//                }
//            }
//            return outputs;
//        }

        Node cast_or_throw(Node node, DataType data_type, std::string op_name){
            if(node->data_type != data_type){
                Graph g = node.g();
                switch (g->props.policies.implicit_cast){
                    case RAISE: {
                        g->logger()->error("[op::{}] Implicit cast from type {} to type {}",
                                           op_name, node->data_type, data_type);
                        throw InvalidOperatorArgument(NodeVec{node}, op_name,
                                                      "Attempting implicit cast from type " + to_string(node->data_type)
                                                      + " to type " + to_string(data_type));
                    }
                    case WARN: {
                        g->logger()->warn("[op::{}] Implicit cast from type {} to type {}",
                                          op_name, node->data_type, data_type);
                        break;
                    }
                    default: {}
                }
                node = cast(node, data_type);
            }
            return node;
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

        Axes auto_infer_axes(Node node){
            Axes axes;
            for(int i=0; i<4; ++i){
                if(node->shape[i] != 1){
                    axes.push_back(i);
                }
            }
            return axes;
        }
    }
}
