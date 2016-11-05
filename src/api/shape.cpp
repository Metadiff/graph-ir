//
// Created by alex on 18/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{

        Node diag(Node node){
            Graph g = node.g();
            if(node.order() == 0){
                // If it is a scalar nothing to do
                return alias(node);
            } else if(node.order() > 2){
                // If this is more than a 2D tensor throw an error
                op_logger("Diagonal")->error("The input is of order {} > 2", node.order());
                throw InvalidOperatorArgument(NodeVec{node}, "Diagonal",
                                              "The input is of order " + std::to_string(node.order()) + " > 2");
            } else if(node.order() == 2 and node->shape[0] != 1 and node->shape[0] != node->shape[1]){
                // If this is not a square matrix or a vector throw an error
                op_logger("Diagonal")->error("The input is not a square matrix - {}", to_string(node->shape));
                throw InvalidOperatorArgument(NodeVec{node}, "Diagonal",
                                              "The input is not a square matrix - " + to_string(node->shape));
            }
            // diag(diag(x)) = x, when x is a vector
            auto base = get_base_node(node);
            if(base->op->name == "Diag" and base.order() == 2){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Diagonal>(g.get(), node);
            return g->derived_node(op);
        }

        Node lower_tri(Node node, int k){
            Graph g = node.g();
            if(node.order() == 0){
                // If it is a scalar nothing to do
                return alias(node);
            } else if(node.order() > 2){
                // If this is more than a 2D tensor throw an error
                op_logger("LowerTriangular")->error("The input is of order {} > 2", node.order());
                throw InvalidOperatorArgument(NodeVec{node}, "LowerTriangular",
                                              "The input is of order " + std::to_string(node.order()) + " > 2");
            } else if(node.order() == 2 and node->shape[0] != 1 and node->shape[0] != node->shape[1]){
                // If this is not a square matrix or a vector throw an error
                op_logger("LowerTriangular")->error("The input is not a square matrix - {}", to_string(node->shape));
                throw InvalidOperatorArgument(NodeVec{node}, "LowerTriangular",
                                              "The input is not a square matrix - " + to_string(node->shape));
            }
            // diag(diag(x)) = x, when x is a vector
            auto base = get_base_node(node);
            if(base->op->name == "LowerTriangular"){
                auto cast_op = std::dynamic_pointer_cast<op::LowerTriangular>(base->op);
                if(cast_op->k >= k){
                    return alias(node);
                } else {
                    return lower_tri(base->op->get_parents()[0], k);
                }
            }
            // Standard
            Operator op = std::make_shared<op::LowerTriangular>(g.get(), node, k);
            return g->derived_node(op);
        }

        Node upper_tri(Node node, int k){
            Graph g = node.g();
            if(node.order() == 0){
                // If it is a scalar nothing to do
                return alias(node);
            } else if(node.order() > 2){
                // If this is more than a 2D tensor throw an error
                op_logger("UpperTriangular")->error("The input is of order {} > 2", node.order());
                throw InvalidOperatorArgument(NodeVec{node}, "UpperTriangular",
                                              "The input is of order " + std::to_string(node.order()) + " > 2");
            } else if(node.order() == 2 and node->shape[0] != 1 and node->shape[0] != node->shape[1]){
                // If this is not a square matrix or a vector throw an error
                op_logger("UpperTriangular")->error("The input is not a square matrix - {}", to_string(node->shape));
                throw InvalidOperatorArgument(NodeVec{node}, "UpperTriangular",
                                              "The input is not a square matrix - " + to_string(node->shape));
            }
            // diag(diag(x)) = x, when x is a vector
            auto base = get_base_node(node);
            if(base->op->name == "UpperTriangular"){
                auto cast_op = std::dynamic_pointer_cast<op::UpperTriangular>(base->op);
                if(cast_op->k >= k){
                    return alias(node);
                } else {
                    return upper_tri(base->op->get_parents()[0], k);
                }
            }
            // Standard
            Operator op = std::make_shared<op::UpperTriangular>(g.get(), node, k);
            return g->derived_node(op);
        }

        Node reshape(Node node, Shape shape){
            Graph g = node.g();
            if (number_of_elements(node->shape) != number_of_elements(shape)) {
                // If the number of elements is different throw an error.
                op_logger("Reshape")->error("The number of elements can not change from {} to {}",
                        number_of_elements(node->shape).to_string(),
                        number_of_elements(shape).to_string());
                throw InvalidOperatorArgument(NodeVec{node}, "Reshape",
                                              "The number of elements can not change from "
                                              + number_of_elements(node->shape).to_string() + " to "
                                              + number_of_elements(shape).to_string());
            } else if(node->shape == shape){
                // If reshaping to the same shape do nothing
                return alias(node);
            }
            // The reshape(reshape(x)) = reshape(x)
            auto base = get_base_node(node);
            if(base->op->name == "Reshape"){
                return reshape(base->op->get_parents()[0], shape);
            }
            // Standard
            Operator op = std::make_shared<op::Reshape>(g.get(), node, shape);
            return g->derived_node(op);
        }

        Node flatten(Node node){
            return reshape(node, Shape{number_of_elements(node->shape), 1, 1, 1});
        }

        Node reorder(Node node, Axes order){
            Graph g = node.g();
            if(order.size() < 2 or order.size() > 4){
                op_logger("Reorder")->error("Invalid number of axis for reordering given - {}", order.size());
                throw InvalidOperatorArgument(NodeVec{node},
                                              "Reorder", "Invalid number of axis for reordering given - "
                                                          + std::to_string(order.size()));
            } else {
                // Fill them in
                for(auto i=order.size(); i < 4; ++i){
                    order.push_back(i);
                }
                // Check they are distinct
                bool checks[4] = {false, false, false, false};
                for(auto i = 0; i < 4; ++i){
                    checks[order[i]] = true;
                }
                for(auto i = 0; i < 4; ++i){
                    if(not checks[i]){
                        op_logger("Reorder")->error("Duplicate axis - {}", i);
                        throw InvalidOperatorArgument(NodeVec{node},
                                                      "Reorder", "Duplicate axis - " + std::to_string(i));
                    }
                }
            }
            bool ordered = true;
            for(auto i=0; i<4; ++i){
                if(order[i] != i){
                    ordered = false;
                    break;
                }
            }
            // If the axes are ordered do nothing
            if(ordered){
                return alias(node);
            }
            // The reorder(reorder(x)) = reorder(x) with some specifics
            auto base = get_base_node(node);
            if(base->op->name == "Reorder"){
                std::shared_ptr<const op::Reorder> cast_op = std::dynamic_pointer_cast<const op::Reorder>(base->op);
                Axes original_order = {0, 1, 2, 3};
                for(auto i=0; i < cast_op->order.size(); ++i){
                    original_order[cast_op->order[i]] = i;
                }
                Axes new_order = original_order;
                for(auto i=0;i < order.size(); ++i){
                    new_order[order[i]] = original_order[i];
                }
                bool ordered = true;
                for(auto i=0; i<4; ++i){
                    if(order[i] != i){
                        ordered = false;
                        break;
                    }
                }
                // If the axes are ordered do nothing
                if(ordered){
                    return alias(node);
                }
                // Standard
                Operator op = std::make_shared<op::Reorder>(g.get(), base, order);
                return g->derived_node(op);
            }
            // Standard
            Operator op = std::make_shared<op::Reorder>(g.get(), node, order);
            return g->derived_node(op);
        }

        Node transpose(Node node){
            // For a scalar do nothing
            if(node.order() == 0){
                return alias(node);
            }
            // Switch the last two dimensions
            int dims = node.order();
            dims = dims == 1 ? 2 : dims;
            Axes order;
            for(auto i=0;i<dims-2; ++i){
                order.push_back(i);
            }
            order.push_back(dims-1);
            order.push_back(dims-2);
            return reorder(node, order);
        }
    }
}
