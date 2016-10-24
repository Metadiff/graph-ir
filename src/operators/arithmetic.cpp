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
                return alias(base->op->get_parents()[0]);
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
                return alias(base->op->get_parents()[0]);
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

//        Node Node::cast(dataType data_type) {
//            GraphInPtr graph = unwrap()->graph;
//            return graph->derived_node(std::make_shared<op::Cast>(graph, this, data_type));
//        }
//
//        Node Node::alias() {
//            return apply<op::Alias>(unwrap());
//        }
//
//        Node Node::broadcast(Shape shape) {
//            GraphInPtr graph = unwrap()->graph;
//            return graph->derived_node(std::make_shared<op::Broadcast>(graph, this, shape));
//        }
//
//        Node Node::broadcast_to(Node other) {
//            return broadcast(other->shape);
//        }
//
////        Node Node::sum(Axes axes) {
////            GraphInPtr graph = unwrap()->graph;
////            return graph->derived_node(std::make_shared<op::Sum>(graph, this, axes));
////        }
//
//        Node GraphInternal::add(NodeVec nodes) {
//            // TODO a + (-a) = 0
//            // TODO a * b + c * b = (a + c) * b ???
//            return apply<op::Add>(nodes);
//        };
//
//        Node operator+(Node node1, Node node2) {
//            return node1->graph->add(NodeVec {node1, node2});
//        };
//
//        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator+(L node1, Node node2) {
//            return node2->graph->add(NodeVec {node2->graph->wrap(node1), node2});
//        };
//
//        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator+(Node node1, R node2) {
//            return node1->graph->add(NodeVec {node1, node1->graph->wrap(node2)});
//        };
//
//        Node Node::neg() {
//            // TODO x.neg().neg() = x
//            return apply<op::Neg>(unwrap());
//        }
//
//        Node operator-(Node node) {
//            return node.neg();
//        }
//
//        Node operator-(Node node1, Node node2) {
//            return node1->graph->add(NodeVec{node1, node2.neg()});
//        }
//
//        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator-(L node1, Node node2) {
//            return node2->graph->add(NodeVec {node2->graph->wrap(node1), node2.neg()});
//        };
//
//        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator-(Node node1, R node2) {
//            return node1->graph->add(NodeVec {node1, node1->graph->wrap(node2).neg()});
//        };
//
//        Node GraphInternal::mul(NodeVec nodes) {
//            // TODO e^x * e^y = e^(x+y)
//            // TODO x * x = x.square()
//            // TODO x * (y / x) = y
//            // Reorder so that Div operators are always at the end
////            std::vector <size_t> div_indexes;
////            for (size_t i = 0; i < nodes.size(); i++) {
////                if (nodes[i]->op->name == "Div") {
////                    div_indexes.push_back(i);
////                }
////            }
////            if (div_indexes.size() == 0 or div_indexes.size() == nodes.size()) {
////                return apply<op::Mul>(nodes);
////            } else {
////                NodeVec reordered;
////                for (size_t i = 0; i < nodes.size(); i++) {
////                    if (std::find(div_indexes.begin(), div_indexes.end(), i) == div_indexes.end()) {
////                        reordered.push_back(nodes[i]);
////                    }
////                }
////                for (size_t i = 0; i < div_indexes.size(); i++) {
////                    reordered.push_back(nodes[div_indexes[i]]);
////                }
////                return apply<op::Mul>(reordered);
////            }
//            return apply<op::Mul>(nodes);
//        };
//
//        Node operator*(Node node1, Node node2) {
//            return node1->graph->mul(NodeVec{node1, node2});
//        };
//
//        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator*(L node1, Node node2) {
//            return node2->graph->mul(NodeVec {node2->graph->wrap(node1), node2});
//        };
//
//        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator*(Node node1, R node2) {
//            return node1->graph->mul(NodeVec {node1, node1->graph->wrap(node2)});
//        };
//
//        Node Node::div() {
//            // TODO x.div().div() = x
//            return apply<op::Div>(unwrap());
//        }
//
//        Node operator/(Node node1, Node node2) {
//            return node1->graph->mul(NodeVec{node1, node2.div()});
//        };
//
//        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
//        Node operator/(L node1, Node node2) {
//            return node2->graph->mul(NodeVec {node2->graph->wrap(node1), node2.div()});
//        };
//
//        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
//        Node operator/(Node node1, R node2) {
//            return node1->graph->mul(NodeVec {node1, node1->graph->wrap(node2).div()});
//        };
    }
}
