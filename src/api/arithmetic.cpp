//
// Created by alex on 04/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{

        Node add(NodeVec nodes, std::vector<bool> neg){
            if(nodes.size() == 0){
                op_logger("Add")->error("Zero nodes provided.");
                throw InvalidOperatorArgument(nodes, "Add", "Zero nodes provided.");
            } else if(nodes.size() == 1){
                op_logger("Add")->error("One nodes provided.");
                throw InvalidOperatorArgument(nodes, "Add", "One nodes provided.");
            }
            Graph g = nodes[0].g();
            for(auto i=1; i<nodes.size(); ++i){
                if(g != nodes[i].g()){
                    op_logger("Add")->error("The input variables are not from the same graph.");
                    throw InvalidOperatorArgument(nodes,
                                                  "Add", "The input variables are not from the same graph.");
                }
            }
            // If empty by default means it is only false
            if(neg.size() == 0){
                for(auto i=0; i<nodes.size(); ++i){
                    neg.push_back(false);
                }
            }
            // TODO check for redundancies like x + (-x)
            verify_shapes_and_broadcast(nodes, "Add");
            // Standard
            Operator op = std::make_shared<op::Add>(g.get(), nodes, neg);
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
            Graph g = node.g();
            // Enforcing neg(neg(x)) = x
            auto base = get_base_node(node);
            // The -(-x) = x
            if(base->op->name == "Add" and base->op->get_parents().size() == 1){
                auto cast_op = std::dynamic_pointer_cast<op::Add>(base->op);
                if(cast_op->neg[0]) {
                    alias(base->op->get_parents()[0]);
                } else {
                    Operator op =  std::make_shared<op::Add>(g.get(), base->op->get_parents(),
                                                             std::vector<bool> {true});
                    return g->derived_node(op);
                }
            }
            // Standard
            Operator op = std::make_shared<op::Add>(g.get(), NodeVec {node}, std::vector<bool> {true});
            return g->derived_node(op);
        }

        Node neg(Node node1, Node node2){
            return add({node1, node2}, {false, true});
        }

        Node mul(NodeVec nodes, std::vector<bool> div){
            if(nodes.size() == 0){
                op_logger("Mul")->error("Zero nodes provided.");
                throw InvalidOperatorArgument(nodes,
                                              "Mul", "Zero nodes provided.");
            } else if(nodes.size() == 1){
                // TODO
                op_logger("Mul")->error("One nodes provided.");
                throw InvalidOperatorArgument(nodes,
                                              "Mul", "One nodes provided.");
            }
            Graph g = nodes[0].g();
            for(auto i=1; i<nodes.size(); ++i){
                if(g != nodes[i].g()){
                    op_logger("Mul")->error("The input variables are not from the same graph.");
                    throw InvalidOperatorArgument(nodes,
                                                  "Mul", "The input variables are not from the same graph.");
                }
            }
            // If empty by default means it is only false
            if(div.size() == 0){
                for(auto i=0; i<nodes.size(); ++i){
                    div.push_back(false);
                }
            }
            // TODO check for redundancies like x * (1/x)
            verify_shapes_and_broadcast(nodes, "Mul");
            // Standard
            Operator op = std::make_shared<op::Mul>(g.get(), nodes, div);
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
            Graph g = node.g();
            // Enforcing neg(neg(x)) = x
            auto base = get_base_node(node);
            // The div(div(x)) = x
            if(base->op->name == "Mul" and base->op->get_parents().size() == 1){
                auto cast_op = std::dynamic_pointer_cast<op::Mul>(base->op);
                if(cast_op->div[0]) {
                    alias(base->op->get_parents()[0]);
                } else {
                    Operator op =  std::make_shared<op::Mul>(g.get(), base->op->get_parents(),
                                                             std::vector<bool> {true});
                    return g->derived_node(op);
                }
            }
            // Standard
            Operator op = std::make_shared<op::Mul>(g.get(), NodeVec {node}, std::vector<bool> {true});
            return g->derived_node(op);
        }

        Node div(Node node1, Node node2){
            return mul({node1, node2}, {false, true});
        }

        Node int_div(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("IntDiv")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "IntDiv", "The input variables are not from the same graph.");
            }
            // Make sure all operands are integers
            if(node1->data_type > i64){
                node1 = implicit_cast(node1, g->props.max_int, "IntDiv");
            }
            if(node2->data_type > i64){
                node2 = implicit_cast(node2, g->props.max_int, "IntDiv");
            }
            // Standard
            Operator op = std::make_shared<op::IntDiv>(g.get(), node1, node2);
            return g->derived_node(op);
        }

        Node int_mod(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("IntMod")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "IntMod", "The input variables are not from the same graph.");
            }
            // Make sure all operands are integers
            if(node1->data_type > i64){
                node1 = implicit_cast(node1, g->props.max_int, "IntMod");
            }
            if(node2->data_type > i64){
                node2 = implicit_cast(node2, g->props.max_int, "IntMod");
            }
            // Standard
            Operator op = std::make_shared<op::IntMod>(g.get(), node1, node2);
            return g->derived_node(op);
        }
    }
}
