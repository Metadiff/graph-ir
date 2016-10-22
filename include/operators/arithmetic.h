//
// Created by alex on 13/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_BASE_H
#define METADIFF_CORE_OPERATORS_BASE_H

namespace md {
    namespace core {
        namespace op {
            /** Addition operator */
            class Add : public ElementwiseNary {
            public:
                Add(GraphInPtr graph, NodeVec parents) :
                        ElementwiseNary("Add", graph, parents) {}

                Add(GraphInPtr graph, Node parent1, Node parent2) :
                        Add(graph, {parent1, parent2}) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Add>(graph, ancestors);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad;
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        bool check[parents.size()];
                        for (int i = 0; i < parents.size(); i++) {
                            check[i] = false;
                        }
                        if (parents.size() != op->get_parents().size()) {
                            return false;
                        }
                        for (int i = 0; i < parents.size(); i++) {
                            Node parent = op->get_parents()[i];
                            int j = 0;
                            for (; j < parents.size(); j++) {
                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
                                    check[j] = true;
                                    break;
                                }
                            }
                            if (j == parents.size()) {
                                return false;
                            }
                        }
                    }
                    return false;
                }
            };

            /** Unary negation */
            class Neg : public UnaryOperator {
            public:
                Neg(GraphInPtr graph, Node parent) :
                        UnaryOperator("Neg", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Neg>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->neg(my_grad);
                };
            };

            /** Elementwise multiplication */
            class Mul : public ElementwiseNary {
            public:
                Mul(GraphInPtr graph, NodeVec parents) :
                        ElementwiseNary("Mul", graph, parents) {};

                Mul(GraphInPtr graph, Node p1, Node p2) :
                        ElementwiseNary("Mul", graph, {p1, p2}) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mul>(graph, ancestors);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    // TODO change the ones and zeros to correct
                    if (parents.size() == 2) {
                        // Special case when only two parents
                        return graph->mul(my_grad, parents[1 - index]);
                    } else {
                        Node product = graph->mul(my_grad, owner);
                        return graph->div(product, parents[index]);
                    }
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        bool check[parents.size()];
                        for (int i = 0; i < parents.size(); i++) {
                            check[i] = false;
                        }
                        if (parents.size() != op->get_parents().size()) {
                            return false;
                        }
                        for (int i = 0; i < parents.size(); i++) {
                            Node parent = op->get_parents()[i];
                            int j = 0;
                            for (; j < parents.size(); j++) {
                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
                                    check[j] = true;
                                    break;
                                }
                            }
                            if (j == parents.size()) {
                                return false;
                            }
                        }
                    }
                    return false;
                }
            };

            /** Unary division (inverse) */
            class Div : public FloatUnaryOperator {
            public:
                Div(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Div", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Div>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    auto result = graph->div(my_grad, graph->square(parent));
                    return graph->neg(result);
                }
            };

            /** Integer dicision */
            class IntDiv : public BinaryOperator {
            public:
                IntDiv(GraphInPtr graph, Node parent1, Node parent2) :
                        BinaryOperator("IntDiv", graph, parent1, parent2) {
                    // TODO check types of parent1 and parent2 and cast them to ints if needed
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Div>(graph, ancestors[0]);
                }

                dataType get_data_type() const {
                    return graph->max_int;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Integer reminder */
            class IntMod : public BinaryOperator {
            public:
                IntMod(GraphInPtr graph, Node parent1, Node parent2) :
                        BinaryOperator("IntMod", graph, parent1, parent2) {
                    // TODO check types of parent1 and parent2 and cast them to ints if needed
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Div>(graph, ancestors[0]);
                }

                dataType get_data_type() const {
                    return graph->max_int;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };
        }
    }
}

#endif //METADIFF_OPERATORS_BASE_H
