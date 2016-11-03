//
// Created by alex on 19/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{
        Node abs(Node node){
            Graph g = node.g();
            // If parent is a positive operator do nothing
            auto base = get_base_node(node);
            if(base->op->name == "Square" or base->op->name == "Exp"
               or base->op->name == "Sigmoid" or base->op->name == "Log1p"
               or base->op->name == "Softplus"){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::Abs>(g.get(), node);
            return g->derived_node(op);
        }

        Node square(Node node){
            Graph g = node.g();
            // If parent is square root return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Sqrt"){
                return alias(base->op->get_parents()[0]);
            }
            // If the parent is abs, return the square of its parent
            if(base->op->name == "Abs"){
                node = base->op->get_parents()[0];
            }
            // Standard
            Operator op = std::make_shared<op::Square>(g.get(), node);
            return g->derived_node(op);
        }

        Node sqrt(Node node) {
            Graph g = node.g();
            // If parent is square return abs
            auto base = get_base_node(node);
            if(base->op->name == "Square"){
                return abs(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Sqrt>(g.get(), node);
            return g->derived_node(op);
        }

        Node exp(Node node){
            Graph g = node.g();
            // If parent is log return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Log"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Exp>(g.get(), node);
            return g->derived_node(op);
        }

        Node log(Node node){
            Graph g = node.g();
            // If parent is exp return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Exp"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::Log>(g.get(), node);
            return g->derived_node(op);
        }

        Node log10(Node node){
            Graph g = node.g();
            // If parent is exp return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Exp"){
                return base->op->get_parents()[0] * g->LN_10();
            }
            // Standard
            Operator op = std::make_shared<op::Log10>(g.get(), node);
            return g->derived_node(op);
        }

        Node log1p(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Log1p>(g.get(), node);
            return g->derived_node(op);
        }

        Node sin(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Sin>(g.get(), node);
            return g->derived_node(op);
        }

        Node cos(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Cos>(g.get(), node);
            return g->derived_node(op);
        }

        Node tan(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Tan>(g.get(), node);
            return g->derived_node(op);
        }

        Node cot(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Cot>(g.get(), node);
            return g->derived_node(op);
        }

        Node sinh(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Sinh>(g.get(), node);
            return g->derived_node(op);
        }

        Node cosh(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Cosh>(g.get(), node);
            return g->derived_node(op);
        }

        Node tanh(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Tanh>(g.get(), node);
            return g->derived_node(op);
        }

        Node coth(Node node){
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Coth>(g.get(), node);
            return g->derived_node(op);
        }

        Node pow(Node node, Node power){
            Graph g = node.g();
            if(g != power.g()){
                op_logger("Power")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node, power},
                                              "Power", "The input variables are not from the same graph.");
            }
            if(power.order() == 0 and power->op->name == "ConstValue"){
                auto cast_op = std::dynamic_pointer_cast<op::ConstantValue>(power->op);
                if(cast_op->value == 2){
                    return square(node);
                }
                if(cast_op->value == 0.5){
                    return sqrt(node);
                }
            }
            // Sort out the shapes
            NodeVec nodes = {node, power};
            verify_shapes_and_broadcast(nodes, "Power");
            // Standard
            Operator op = std::make_shared<op::LogicalAnd>(g.get(), nodes[0], nodes[1]);
            return g->derived_node(op);
        }
    }
}
