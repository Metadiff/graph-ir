//
// Created by alex on 04/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        Node GraphInternal::add(NodeVec nodes){
            // TODO check for redundancies like x + (-x)
            return apply<op::Add>(nodes);
        }

        Node GraphInternal::add(Node node1, Node node2){
            return add({node1, node2});
        }

        Node GraphInternal::add(Node node1, Node node2, Node node3){
            return add({node1, node2, node3});
        }

        Node GraphInternal::add(Node node1, Node node2, Node node3, Node node4){
            return add({node1, node2, node3, node4});
        }

        Node GraphInternal::neg(Node node){
            auto base = get_base_node(node);
            // The -(-x) = x
            if(base->op->name == "Neg"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::Neg>(node);
        }

        Node GraphInternal::neg(Node node1, Node node2){
            return add({node1, neg(node2)});
        }

        Node GraphInternal::mul(NodeVec nodes){
            // TODO check for redundancies like x * (1/x)
            return apply<op::Mul>(nodes);
        }

        Node GraphInternal::mul(Node node1, Node node2){
            return mul({node1, node2});
        }

        Node GraphInternal::mul(Node node1, Node node2, Node node3){
            return mul({node1, node2, node3});
        }

        /** Multiplies nodes */
        Node GraphInternal::mul(Node node1, Node node2, Node node3, Node node4){
            return mul({node1, node2, node3, node4});
        }

        Node GraphInternal::div(Node node){
            auto base = get_base_node(node);
            // The -(-x) = x
            if(base->op->name == "Div"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::Division>(node);
        }

        Node GraphInternal::div(Node node1, Node node2){
            return mul({node1, div(node2)});
        }

        Node GraphInternal::int_div(Node node1, Node node2){
            return apply<op::IntDiv>(node1, node2);
        }

        Node GraphInternal::int_mod(Node node1, Node node2){
            return apply<op::IntMod>(node1, node2);
        }
    }
}
