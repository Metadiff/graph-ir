//
// Created by alex on 18/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node GraphInternal::cast(Node node, dataType data_type) {
            // If same data_type do nothing
            if(node->data_type == data_type){
                return alias(node);
            }
            // Standard
            auto g = node->graph;
            return g->derived_node(std::make_shared<op::Cast>(g, node, data_type));
        }

        Node GraphInternal::broadcast(Node node, Shape shape) {
            // If same shape do nothing
            if(node->shape == shape){
                return alias(node);
            }
            // Standard
            auto g = node->graph;
            return g->derived_node(std::make_shared<op::Broadcast>(g, node, shape));
        }

        Node GraphInternal::alias(Node node) {
            // Standard
            return apply<op::Alias>(get_base_node(node));
        }
    }
}
