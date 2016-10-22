//
// Created by alex on 19/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node GraphInternal::square(Node node){
            // If parent is square root return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Sqrt"){
                return alias(base->op->get_parents()[0]);
            }
            // If the parent is abs, return the square of its parent
            if(base->op->name == "Abs"){
                node = base->op->get_parents()[0];
            }
            return apply<op::Square>(node);
        }

        Node GraphInternal::sqrt(Node node) {
            // If parent is square return abs
            auto base = get_base_node(node);
            if(base->op->name == "Square"){
                return abs(base->op->get_parents()[0]);
            }
            return apply<op::Sqrt>(node);
        }

        Node GraphInternal::exp(Node node){
            // If parent is log return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Log"){
                return alias(base->op->get_parents()[0]);
            }
            return apply<op::Exp>(node);
        }

        Node GraphInternal::log(Node node){
            // If parent is exp return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Exp"){
                return alias(base->op->get_parents()[0]);
            }
            return apply<op::Log>(node);
        }

        Node GraphInternal::log10(Node node){
            // If parent is exp return the upper node
            auto base = get_base_node(node);
            if(base->op->name == "Exp"){
                return mul(base->op->get_parents()[0], LN_10());
            }
            return apply<op::Log10>(node);
        }

        Node GraphInternal::abs(Node node){
            // If parent is a positive operator do nothing
            auto base = get_base_node(node);
            if(base->op->name == "Square" or base->op->name == "Exp"
               or base->op->name == "Sigmoid" or base->op->name == "Log1p"
               or base->op->name == "Softplus"){
                return alias(node);
            }
            return apply<op::Abs>(node);
        }

        Node GraphInternal::log1p(Node node){
            return apply<op::Log1p>(node);
        }

        Node GraphInternal::softplus(Node node, double threshold){
            return apply<op::Softplus>(node);
        }

        Node GraphInternal::sigmoid(Node node){
            return apply<op::Sigmoid>(node);
        }

        Node GraphInternal::sin(Node node){
            return apply<op::Sin>(node);
        }

        Node GraphInternal::cos(Node node){
            return apply<op::Cos>(node);
        }

        Node GraphInternal::tan(Node node){
            return apply<op::Tan>(node);
        }

        Node GraphInternal::cot(Node node){
            return apply<op::Cot>(node);
        }

        Node GraphInternal::sinh(Node node){
            return apply<op::Sinh>(node);
        }

        Node GraphInternal::cosh(Node node){
            return apply<op::Cosh>(node);
        }

        Node GraphInternal::tanh(Node node){
            return apply<op::Tanh>(node);
        }

        Node GraphInternal::coth(Node node){
            return apply<op::Coth>(node);
        }

        Node GraphInternal::pow(Node node, Node power){
            if(power.dims() == 0 and power->op->name == "ConstValue"){
                auto cast_op = std::static_pointer_cast<op::ConstantValue>(power->op);
                if(cast_op->value == 2){
                    return square(node);
                }
                if(cast_op->value == 0.5){
                    return sqrt(node);
                }
            }
            return apply<op::Pow>(node, power);
        }
    }
}
