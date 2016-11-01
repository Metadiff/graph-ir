//
// Created by alex on 18/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node GraphInternal::diag(Node node){
            // If it is a scalar nothing to do
            if(node.dims() == 0){
                return api::alias(node);
            }
            // diag(diag(x)) = x
            auto base = get_base_node(node);
            if(base->op->name == "Diag"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return derived_node(std::make_shared<op::Diagonal>(this, node));
        }

        Node GraphInternal::reshape(Node node, Shape shape){
            // If shapes are equal do nothing
            if(node->shape == shape){
                return api::alias(node);
            }
            // The reshape(reshape(x)) = reshape(x)
            auto base = get_base_node(node);
            if(base->op->name == "Reshape"){
                return reshape(base->op->get_parents()[0], shape);
            }
            // Standard
            return derived_node(std::make_shared<op::Reshape>(this, base, shape));
        }

        Node GraphInternal::reorder(Node node, Axes order){
            // For a scalar do nothing
            if(node.dims() == 0){
                return api::alias(node);
            }
            bool ordered = true;
            for(auto i=0; i<order.size(); ++i){
                if(order[i] != i){
                    ordered = false;
                    break;
                }
            }
            // ordered means that you are not reordering
            if(ordered){
                return api::alias(node);
            }
            // The reorder(reorder(x)) = reorder(x) with some specifics
            auto base = get_base_node(node);
            if(base->op->name == "Reorder"){
                std::shared_ptr<const op::Reorder> cast_op = std::dynamic_pointer_cast<const op::Reorder>(base->op);
                if(cast_op->order.size() != order.size()){
                    return derived_node(std::make_shared<op::Reorder>(this, base, order));
                }
                Axes new_order;
                for(auto i=0;i < order.size(); ++i){
                    new_order.push_back(cast_op->order[order[i]]);
                }
                ordered = true;
                for(auto i=0; i<new_order.size(); ++i){
                    if(new_order[i] != i){
                        ordered = false;
                        break;
                    }
                }
                if(ordered){
                    return api::alias(base->op->get_parents()[0]);
                }
                return reorder(base->op->get_parents()[0], new_order);
            }
            // Standard
            return derived_node(std::make_shared<op::Reorder>(this, base, order));
        }

        Node GraphInternal::transpose(Node node){
            // For a scalar do nothing
            if(node.dims() == 0){
                return api::alias(node);
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
