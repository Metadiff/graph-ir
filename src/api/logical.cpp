//
// Created by alex on 10/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{

        Node logical_not(Node node){
            Graph g = node.g();
            if(node->data_type != b8){
                // Node should be b8
                node = implicit_cast(node, b8, "LogicalNot");
            } else {
                auto base = get_base_node(node);
                // The not(not(x)) = x
                if (base->op->name == "LogicalNot") {
                    return api::alias(base->op->get_parents()[0]);
                }
            }
            // Standard
            Operator op = std::make_shared<op::LogicalNot>(g.get(), node);
            return g->derived_node(op);
        }

        Node logical_and(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("LogicalAnd")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "LogicalAnd", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "LogicalAnd");
            // Node1 should be b8
            if(nodes[0]->data_type != b8){
                nodes[0] = implicit_cast(nodes[0], b8, "LogicalAnd");
            }
            // Node2 should be b8
            if(nodes[1]->data_type != b8){
                nodes[1] = implicit_cast(nodes[1], b8, "LogicalAnd");
            }
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return nodes[0];
            }
            // Standard
            Operator op = std::make_shared<op::LogicalAnd>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node logical_or(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("LogicalOr")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "LogicalOr", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "LogicalOr");
            // Node1 should be b8
            if(nodes[0]->data_type != b8){
                nodes[0] = implicit_cast(nodes[0], b8, "LogicalOr");
            }
            // Node2 should be b8
            if(nodes[1]->data_type != b8){
                nodes[1] = implicit_cast(nodes[1], b8, "LogicalOr");
            }
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return nodes[0];
            }
            // Standard
            Operator op = std::make_shared<op::LogicalOr>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node greater_than(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("GreaterThan")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "GreaterThan", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "GreaterThan");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->zeros(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::GreaterThan>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node less_than(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("LessThan")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "LessThan", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "LessThan");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->zeros(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::LessThan>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node greater_than_or_equal(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("GreaterThanOrEqual")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "GreaterThanOrEqual", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "GreaterThanOrEqual");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->ones(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::GreaterThanOrEqual>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node less_than_or_equal(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("LessThanOrEqual")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "LessThanOrEqual", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "LessThanOrEqual");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->ones(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::LessThanOrEqual>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node equals(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("Equals")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "Equals", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "Equals");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->ones(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::Equals>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node not_equals(Node node1, Node node2){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("NotEquals")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "NotEquals", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "NotEquals");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->ones(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::NotEquals>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }

        Node approx_equals(Node node1, Node node2, double tolerance){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("ApproximatelyEquals")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2},
                                              "ApproximatelyEquals", "The input variables are not from the same graph.");
            }
            // Sort out the shapes
            NodeVec nodes = {node1, node2};
            verify_shapes_and_broadcast(nodes, "ApproximatelyEquals");
            // If the two nodes are the same
            if(symbolic_equals(nodes[0], nodes[1])){
                return g->ones(nodes[0]->shape, b8);
            }
            // Standard
            Operator op = std::make_shared<op::ApproximatelyEquals>(g.get(), nodes[0], nodes[1], tolerance);
            return g->derived_node(op);
        }

        Node isNan(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::IsNaN>(g.get(), node);
            return g->derived_node(op);
        }

        Node isInf(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::IsInf>(g.get(), node);
            return g->derived_node(op);
        }
    }
}

