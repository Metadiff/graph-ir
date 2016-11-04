//
// Created by alex on 13/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_ARITHMETIC_H
#define METADIFF_GRAPH_IR_OPERATORS_ARITHMETIC_H

namespace md {
    namespace gir {
        namespace op {
            /** Addition */
            class Add : public AssociativeElementwiseOperator {
            public:
                std::vector<bool> neg;
                Add(GraphInPtr graph, NodeVec parents, std::vector<bool> neg) :
                        AbstractOperator(graph, "Add"), AssociativeOperator(parents),
                        neg(neg){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Add>(graph, ancestors, neg);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return neg[index] ? (- my_derivative) : my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return backward_diff_parent(parent_derivatives[index], index);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        bool check[parents.size()];
//                        for (int i = 0; i < parents.size(); i++) {
//                            check[i] = false;
//                        }
//                        if (parents.size() != op->get_parents().size()) {
//                            return false;
//                        }
//                        for (int i = 0; i < parents.size(); i++) {
//                            Node parent = op->get_parents()[i];
//                            int j = 0;
//                            for (; j < parents.size(); j++) {
//                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
//                                    check[j] = true;
//                                    break;
//                                }
//                            }
//                            if (j == parents.size()) {
//                                return false;
//                            }
//                        }
//                    }
//                    return false;
//                }
            };

//            /** Negation */
//            class Neg : public UnaryElementwiseOperator {
//            public:
//                Neg(GraphInPtr graph, Node parent) :
//                        AbstractOperator(graph, "Neg"), UnaryOperator(parent) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<Neg>(graph, ancestors[0]);
//                }
//
//                DataType get_data_type() const {
//                    // If unsigned make it signed
//                    if(parent->data_type < i8){
//                        return static_cast<DataType>(parent->data_type + 4);
//                    }
//                    return parent->data_type;
//                }
//
//                Node backward_diff_parent(Node my_derivative, int index){
//                    return neg(my_derivative);
//                }
//
//                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
//                    return neg(parent_derivatives[index]);
//                }
//            };

            /** Elementwise multiplication */
            class Mul : public AssociativeElementwiseOperator {
            public:
                std::vector<bool> div;
                Mul(GraphInPtr graph, NodeVec parents, std::vector<bool> div) :
                        AbstractOperator(graph, "Mul"), AssociativeOperator(parents),
                        div(div){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mul>(graph, ancestors, div);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    if (parents.size() == 2 and not div[index]) {
                        return parents[1-index];
                    } else {
                        auto factor = mul({result, my_derivative, parents[index]}, {false, false, true});
                        return div[index] ? (- factor) : factor;
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return backward_diff_parent(parent_derivatives[index], index);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        bool check[parents.size()];
//                        for (int i = 0; i < parents.size(); i++) {
//                            check[i] = false;
//                        }
//                        if (parents.size() != op->get_parents().size()) {
//                            return false;
//                        }
//                        for (int i = 0; i < parents.size(); i++) {
//                            Node parent = op->get_parents()[i];
//                            int j = 0;
//                            for (; j < parents.size(); j++) {
//                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
//                                    check[j] = true;
//                                    break;
//                                }
//                            }
//                            if (j == parents.size()) {
//                                return false;
//                            }
//                        }
//                    }
//                    return false;
//                }
            };

//            /** Elementwise inverse (division) */
//            class Division : public FloatUnaryElementwiseOperator {
//            public:
//                Division(GraphInPtr graph, Node parent) :
//                        AbstractOperator(graph, "Division"), UnaryOperator(parent) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<Division>(graph, ancestors[0]);
//                }
//
//                Node backward_diff_parent(Node my_derivative, int index){
//                    return - (my_derivative / square(parent));
//                }
//            };

            /** Integer division */
            class IntDiv : public BinaryIntegerElementwiseOperator{
            public:
                IntDiv(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator(graph, "IntDiv"), BinaryOperator(parent1, parent2){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IntDiv>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Integer modulus (reminder) */
            class IntMod : public BinaryIntegerElementwiseOperator {
            public:
                IntMod(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator(graph, "IntMod"), BinaryOperator(parent1, parent2){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IntMod>(graph, ancestors[0], ancestors[1]);
                }
            };
        }
    }
}

#endif //METADIFF_GRAPH_IR_OPERATORS_ARITHMETIC_H
