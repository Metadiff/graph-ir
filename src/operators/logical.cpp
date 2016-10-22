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
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::LogicalNot>(node);
        }

        Node GraphInternal::logical_and(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LogicalAnd>(get_base_node(node1), get_base_node(node2));
        }

        /** Applies or elementwise */
        Node GraphInternal::logical_or(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LogicalOr>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 > node2 */
        Node GraphInternal::greater_than(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::GreaterThan>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 < node2 */
        Node GraphInternal::less_than(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LessThan>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 >= node2 */
        Node GraphInternal::greater_than_or_equal(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::GreaterThanOrEqual>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 <= node2 */
        Node GraphInternal::less_than_or_equal(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::LessThanOrEqual>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 == node2 */
        Node GraphInternal::equals(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::Equals>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 != node2 */
        Node GraphInternal::not_equals(Node node1, Node node2){
            // TODO check if node1 == node2 than just return that
            return apply<op::NotEquals>(get_base_node(node1), get_base_node(node2));
        }

        /** node1 == node2 up to a tolerance */
        Node GraphInternal::approx_equals(Node node1, Node node2, double tol){
            // TODO check if node1 == node2 than just return that
            auto op = std::make_shared<op::ApproximatelyEquals>(this, get_base_node(node1), get_base_node(node2), tol);
            return derived_node(op);
        }

        /** isNan */
        Node GraphInternal::isNan(Node node){
            auto base = get_base_node(node);
            // The not(not(x)) = x
            if(base->op->name == "IsNaN"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::IsNaN>(node);
        }

        /** isInf */
        Node GraphInternal::isInf(Node node){
            auto base = get_base_node(node);
            // The not(not(x)) = x
            if(base->op->name == "IsInf"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            return apply<op::IsInf>(node);
        }

        /** Selects elementwise each chinels */
        Node GraphInternal::select(Node condition, Node if_true, Node if_false){
            // TODO check if node1 == node2 than just return that
            return apply<op::Select>(condition, if_true, if_false);
        }

//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator&&(L node1, Node node2) {
//            return node2->graph->wrap(node1).logical_and(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator&&(Node node1, R node2) {
//            return node1.logical_and(node1->graph->wrap(node2));
//        };
//
//        Node Node::logical_or(Node node) {
//            return apply<op::Or>(this, node);
//        }
//
//        Node operator||(Node node1, Node node2) {
//            return apply<op::Or>(node1, node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator||(L node1, Node node2) {
//            return node2->graph->wrap(node1).logical_or(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator||(Node node1, R node2) {
//            return node1.logical_or(node1->graph->wrap(node2));
//        };
//
//        Node Node::gt(Node node) {
//            return apply<op::GreaterThan>(this, node);
//        }
//
//        Node operator>(Node node1, Node node2) {
//            return node1.gt(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator>(L node1, Node node2) {
//            return node2->graph->wrap(node1).gt(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator>(Node node1, R node2) {
//            return node1.gt(node1->graph->wrap(node2));
//        };
//
//        Node Node::ge(Node node) {
//            return apply<op::GreaterThanOrEqual>(this, node);
//        }
//
//        Node operator>=(Node node1, Node node2) {
//            return node1.ge(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator>=(L node1, Node node2) {
//            return node2->graph->wrap(node1).ge(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator>=(Node node1, R node2) {
//            return node1.ge(node1->graph->wrap(node2));
//        };
//
//        Node Node::lt(Node node) {
//            return apply<op::LessThan>(this, node);
//        }
//
//        Node operator<(Node node1, Node node2) {
//            return node1.lt(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator<(L node1, Node node2) {
//            return node2->graph->wrap(node1).lt(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator<(Node node1, R node2) {
//            return node1.lt(node1->graph->wrap(node2));
//        };
//
//        Node Node::le(Node node) {
//            return apply<op::LessThanOrEqual>(this, node);
//        }
//
//        Node operator<=(Node node1, Node node2) {
//            return node1.le(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator<=(L node1, Node node2) {
//            return node2->graph->wrap(node1).le(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator<=(Node node1, R node2) {
//            return node1.le(node1->graph->wrap(node2));
//        };
//
//        Node Node::eq(Node node) {
//            return apply<op::Equals>(this, node);
//        }
//
//        Node operator==(Node node1, Node node2) {
//            return node1.eq(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator==(L node1, Node node2) {
//            return node2->graph->wrap(node1).eq(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator==(Node node1, R node2) {
//            return node1.eq(node1->graph->wrap(node2));
//        };
//
//        Node Node::neq(Node node) {
//            return apply<op::NotEquals>(this, node);
//        }
//
//        Node operator!=(Node node1, Node node2) {
//            return node1.neq(node2);
//        }
//
//        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator!=(L node1, Node node2) {
//            return node2->graph->wrap(node1).neq(node2);
//        };
//
//        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator!=(Node node1, R node2) {
//            return node1.neq(node1->graph->wrap(node2));
//        };
//
//        Node Node::approx_eq(Node node, double tol) {
//            GraphInPtr graph = unwrap()->graph;
//            return graph->derived_node(std::make_shared<op::ApproximatelyEquals>(graph, this, node, tol));
//        }
//
//        Node Node::approx_neq(Node node, double tol) {
//            return this->approx_eq(node, tol).logical_not();
//        }
//
//        Node Node::is_nan() {
//            return apply<op::IsNaN>(this);
//        }
//
//        Node Node::is_inf() {
//            return apply<op::IsInf>(this);
//        }
//
//        Node Node::all(){
//            return apply<op::All>(this);
//        }
//
//        Node Node::any() {
//            return apply<op::Any>(this);
//        }
//
//        Node Node::select(Node result_true, Node result_false) {
//            return unwrap()->graph->derived_node(
//                    std::make_shared<op::Select>(unwrap()->graph, this, result_true, result_false));
//        }
    }
}

