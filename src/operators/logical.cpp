//
// Created by alex on 10/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        Node GraphInternal::logical_not(Node node){
            auto base = get_base_node(node);
            // The not(not(x)) = x
            if(base->op->name == "LogicalNot"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::LogicalNot>(node);
        }

        Node GraphInternal::logical_and(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LogicalAnd>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::logical_or(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LogicalOr>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::greater_than(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::GreaterThan>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::less_than(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LessThan>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::greater_than_or_equal(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::GreaterThanOrEqual>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::less_than_or_equal(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LessThanOrEqual>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::equals(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::Equals>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::not_equals(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::NotEquals>(get_base_node(node1), get_base_node(node2));
        }

        Node GraphInternal::approx_equals(Node node1, Node node2, double tol){
            // TODO check if node1 == node2 than just return that
            auto op = std::make_shared<op::ApproximatelyEquals>(this, get_base_node(node1), get_base_node(node2), tol);
            return derived_node(op);
        }

        Node GraphInternal::isNan(Node node){
            auto base = get_base_node(node);
            // The not(not(x)) = x
            if(base->op->name == "IsNaN"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::IsNaN>(node);
        }

        Node GraphInternal::isInf(Node node){
            auto base = get_base_node(node);
            // The not(not(x)) = x
            if(base->op->name == "IsInf"){
                return api::alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::IsInf>(node);
        }
    }
}

