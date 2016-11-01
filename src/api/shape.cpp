//
// Created by alex on 18/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node diag(Node node){
            Graph g = node->g();
            std::string op_name = "Diagonal";
            if(node.dims() == 0){
                // If it is a scalar nothing to do
                return alias(node);
            } else if(node.dims() > 2){
                // If this is more than a 2D tensor throw an error
                auto err = std::make_shared<InvalidArguments>
                        (NodeVec{node}, op_name, "Parent is not a matrix or a vector.");
                err->log(utils::op_logger(op_name));
                throw err;
            } else if(node.dims() == 2 and node->shape[0] != 1 and node->shape[0] != node->shape[1]){
                // If this is not a square matrix or a vector throw an error
                auto err = std::make_shared<InvalidArguments>
                        (NodeVec{node}, op_name, "Tensor is not a square matrix or vector.");
                err->log(utils::op_logger(op_name));
                throw err;
            }
            // diag(diag(x)) = x, when x is a vector
            auto base = get_base_node(node);
            if(base->op->name == "Diag" and base.dims() == 2){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Diagonal>(g.get(), node);
            return g->derived_node(op);
        }

        Node reshape(Node node, Shape shape){
            Graph g = node->g();
            std::string op_name = "Reshape";
            if (number_of_elements(node->shape) != number_of_elements(shape)) {
                // If the number of elements is different throw an error.
                auto err = std::make_shared<InvalidArguments>
                        (NodeVec{node}, op_name, "Total number of elements must not change.");
                err->log(utils::op_logger(op_name));
                throw err;
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

        Node reorder(Node node, Axes order){
            Graph g = node->g();
            std::string op_name = "Reorder";
            if(node.dims() == 0){
                // For a scalar do nothing
                return alias(node);
            } else if(order.size() < 2 or order.size() > 4){
                // TODO throw an exception
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
                        // TODO throw an error
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
            if(node.dims() == 0){
                return alias(node);
            }
            // Switch the last two dimensions
            int dims = node.dims();
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
