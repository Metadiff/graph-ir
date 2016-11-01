//
// Created by alex on 18/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node GraphInternal::cast(Node node, DataType data_type) {
            // If same data_type do nothing
            if(node->data_type == data_type){
                return alias(node);
            }
            // Standard
            return derived_node(std::make_shared<op::Cast>(this, node, data_type));
        }

        Node GraphInternal::alias(Node node) {
            // Standard
            return apply<op::Alias>(get_base_node(node));
        }

        Node GraphInternal::broadcast(Node node, Shape shape) {
            // If same shape do nothing
            if(node->shape == shape){
                return alias(node);
            }
            // Standard
            return derived_node(std::make_shared<op::Broadcast>(this, node, shape));
        }

        Node GraphInternal::make_constant(Node node){
            // If already a constant do nothing
            if(not node->is_differentiable){
                return alias(node);
            }
            // Standard
            return apply<op::MakeConstant>(node);
        }

        Node GraphInternal::select(Node condition, Node if_true, Node if_false){
            // TODO check if node1 == node2 than just return that
            return apply<op::Select>(condition, if_true, if_false);
        }
    }
}
