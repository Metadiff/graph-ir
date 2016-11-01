//
// Created by alex on 04/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node add(NodeVec nodes){
            if(nodes.size() < 2){
                // TODO proper exception
                throw 1;
            }
            Graph g = nodes[0]->g();
            for(auto i=1; i<nodes.size(); ++i){
                if(g != nodes[i]->g()){
                    // TODO proper exception
                    throw 1;
                }
            }
            // TODO check for redundancies like x + (-x)
            nodes = verify_shapes_and_broadcast(nodes, "Add");
            // Standard
            Operator op = std::make_shared<op::Add>(g.get(), nodes);
            return g->derived_node(op);
        }

        Node add(Node node1, Node node2){
            return add({node1, node2});
        }

        Node add(Node node1, Node node2, Node node3){
            return add({node1, node2, node3});
        }

        Node add(Node node1, Node node2, Node node3, Node node4){
            return add({node1, node2, node3, node4});
        }

        Node neg(Node node){
            Graph g = node->g();
            // Enforcing neg(neg(x)) = x
            auto base = get_base_node(node);
            // The -(-x) = x
            if(base->op->name == "Neg"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Neg>(g.get(), node);
            return g->derived_node(op);
        }

        Node neg(Node node1, Node node2){
            return add({node1, neg(node2)});
        }

        Node mul(NodeVec nodes){
            if(nodes.size() < 2){
                // TODO proper exception
                throw 1;
            }
            Graph g = nodes[0]->g();
            for(auto i=1; i<nodes.size(); ++i){
                if(g != nodes[i]->g()){
                    // TODO proper exception
                    throw 1;
                }
            }
            // TODO check for redundancies like x * (1/x)
            nodes = verify_shapes_and_broadcast(nodes, "Mul");
            // Standard
            Operator op = std::make_shared<op::Mul>(g.get(), nodes);
            return g->derived_node(op);
        }

        Node mul(Node node1, Node node2){
            return mul({node1, node2});
        }

        Node mul(Node node1, Node node2, Node node3){
            return mul({node1, node2, node3});
        }

        Node mul(Node node1, Node node2, Node node3, Node node4){
            return mul({node1, node2, node3, node4});
        }

        Node div(Node node){
            Graph g = node->g();
            // Enforcing neg(neg(x)) = x
            auto base = get_base_node(node);
            // The div(div(x)) = x
            if(base->op->name == "Div"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Division>(g.get(), node);
            return g->derived_node(op);
        }

        Node div(Node node1, Node node2){
            return mul({node1, div(node2)});
        }

        Node int_div(Node node1, Node node2){
            Graph g = node1->g();
            if(g != node2->g()){
                // TODO proper exception
                throw 1;
            }
            // Make sure all operands are integers
            if(node1->data_type > i64){
                node1 = cast_or_throw(node1, g->props.max_int, "IntDiv");
            }
            if(node2->data_type > i64){
                node2 = cast_or_throw(node2, g->props.max_int, "IntDiv");
            }
            // Standard
            Operator op = std::make_shared<op::IntDiv>(g.get(), node1, node2);
            return g->derived_node(op);
        }

        Node int_mod(Node node1, Node node2){
            Graph g = node1->g();
            if(g != node2->g()){
                // TODO proper exception
                throw 1;
            }
            // Make sure all operands are integers
            if(node1->data_type > i64){
                node1 = cast_or_throw(node1, g->props.max_int, "IntMod");
            }
            if(node2->data_type > i64){
                node2 = cast_or_throw(node2, g->props.max_int, "IntMod");
            }
            // Standard
            Operator op = std::make_shared<op::IntMod>(g.get(), node1, node2);
            return g->derived_node(op);
        }
    }
}
