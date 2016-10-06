//
// Created by alex on 04/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node Node::cast(dataType data_type) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::Cast>(graph, this, data_type));
        }

        Node Node::alias() {
            return apply<op::Alias>(unwrap());
        }

        Node Node::broadcast(Shape shape) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::Broadcast>(graph, this, shape));
        }

        Node Node::broadcast_to(Node other) {
            return broadcast(other->shape);
        }

        Node Node::sum(Axes axes) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::Sum>(graph, this, axes));
        }

        Node GraphInternal::add(NodeVec nodes) {
            // TODO a + (-a) = 0
            // TODO a * b + c * b = (a + c) * b ???
//            std::vector <size_t> neg_indexes;
//            for (size_t i = 0; i < nodes.size(); i++) {
//                if (nodes[i]->op->name == "Neg") {
//                    neg_indexes.push_back(i);
//                }
//            }
//            if (neg_indexes.size() == 0 or neg_indexes.size() == nodes.size()) {
//                return apply<op::Add>(nodes);
//            } else {
//                NodeVec reordered;
//                for (size_t i = 0; i < nodes.size(); i++) {
//                    if (std::find(neg_indexes.begin(), neg_indexes.end(), i) == neg_indexes.end()) {
//                        reordered.push_back(nodes[i]);
//                    }
//                }
//                for (size_t i = 0; i < neg_indexes.size(); i++) {
//                    reordered.push_back(nodes[neg_indexes[i]]);
//                }
//                return apply<op::Add>(reordered);
//            }
            return apply<op::Add>(nodes);
        };

        Node operator+(Node node1, Node node2) {
            return node1->graph->add(NodeVec {node1, node2});
        };

        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator+(L node1, Node node2) {
            return node2->graph->add(NodeVec {node2->graph->wrap(node1), node2});
        };

        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator+(Node node1, R node2) {
            return node1->graph->add(NodeVec {node1, node1->graph->wrap(node2)});
        };

        Node Node::neg() {
            // TODO x.neg().neg() = x
            return apply<op::Neg>(unwrap());
        }

        Node operator-(Node node) {
            return node.neg();
        }

        Node operator-(Node node1, Node node2) {
            return node1->graph->add(NodeVec{node1, node2.neg()});
        }

        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator-(L node1, Node node2) {
            return node2->graph->add(NodeVec {node2->graph->wrap(node1), node2.neg()});
        };

        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator-(Node node1, R node2) {
            return node1->graph->add(NodeVec {node1, node1->graph->wrap(node2).neg()});
        };

        Node GraphInternal::mul(NodeVec nodes) {
            // TODO e^x * e^y = e^(x+y)
            // TODO x * x = x.square()
            // TODO x * (y / x) = y
            // Reorder so that Div operators are always at the end
//            std::vector <size_t> div_indexes;
//            for (size_t i = 0; i < nodes.size(); i++) {
//                if (nodes[i]->op->name == "Div") {
//                    div_indexes.push_back(i);
//                }
//            }
//            if (div_indexes.size() == 0 or div_indexes.size() == nodes.size()) {
//                return apply<op::Mul>(nodes);
//            } else {
//                NodeVec reordered;
//                for (size_t i = 0; i < nodes.size(); i++) {
//                    if (std::find(div_indexes.begin(), div_indexes.end(), i) == div_indexes.end()) {
//                        reordered.push_back(nodes[i]);
//                    }
//                }
//                for (size_t i = 0; i < div_indexes.size(); i++) {
//                    reordered.push_back(nodes[div_indexes[i]]);
//                }
//                return apply<op::Mul>(reordered);
//            }
            return apply<op::Mul>(nodes);
        };

        Node operator*(Node node1, Node node2) {
            return node1->graph->mul(NodeVec{node1, node2});
        };

        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator*(L node1, Node node2) {
            return node2->graph->mul(NodeVec {node2->graph->wrap(node1), node2});
        };

        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator*(Node node1, R node2) {
            return node1->graph->mul(NodeVec {node1, node1->graph->wrap(node2)});
        };

        Node Node::div() {
            // TODO x.div().div() = x
            return apply<op::Div>(unwrap());
        }

        Node operator/(Node node1, Node node2) {
            return node1->graph->mul(NodeVec{node1, node2.div()});
        };

        template<typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator/(L node1, Node node2) {
            return node2->graph->mul(NodeVec {node2->graph->wrap(node1), node2.div()});
        };

        template<typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator/(Node node1, R node2) {
            return node1->graph->mul(NodeVec {node1, node1->graph->wrap(node2).div()});
        };
    }
}
