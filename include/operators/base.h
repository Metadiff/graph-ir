//
// Created by alex on 13/12/15.
//

#ifndef METADIFF_OPERATORS_BASE_H
#define METADIFF_OPERATORS_BASE_H

namespace md {
    namespace core {
        namespace op {
            class Cast : public UnaryOperator {
            public:
                dataType data_type;

                Cast(GraphInPtr graph, Node parent, dataType data_type) :
                        UnaryOperator("Cast", graph, parent),
                        data_type(data_type) {};

                dataType get_data_type() const {
                    return data_type;
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cast>(graph, ancestors[0], data_type);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad.cast(parent->data_type);
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const Cast>(op);
                        return symbolic_equals(parent, cast_op->parent) and data_type == cast_op->data_type;
                    }
                    return false;
                }
            };

            /**
             * Represents an alias for another node
             * This could be particularly useful for multiple device case
             * where an Alias with another device would mean a transfer
             */
            class Alias : public UnaryOperator {
            public:
                Alias(GraphInPtr graph, Node parent) :
                        UnaryOperator("Alias", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Alias>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad;
                }

                bool equals(Operator const op) const {
                    Operator my_op = get_base_op(parent->op);
                    return my_op->equals(op) or op->equals(my_op);
                }
            };

            /** Broadcasts the parent to the specified shape */
            class Broadcast : public UnaryOperator {
            public:
                Shape to_shape;

                Broadcast(GraphInPtr graph,
                          Node parent,
                          Shape to_shape) :
                        UnaryOperator("Broadcast", graph, parent),
                        to_shape(to_shape) {
                    for (int i = 0; i < 4; i++) {
                        if (parent->shape[i] != 1 and parent->shape[i] != to_shape[i]) {
                            auto err = std::make_shared<InvalidArguments>
                                    (NodeVec{parent}, name, "Can not broadcast to shape " +
                                                                               core::to_string(to_shape));
                            err->log(logger());
                            throw err;
                        }
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Broadcast>(graph, ancestors[0], to_shape);
                }

                Shape get_shape() const {
                    return to_shape;
                }

                Axes get_broadcast_axes() const {
                    Axes axes;
                    auto p1_shape = this->parent->shape;
                    for (size_t i = 0; i < 4; i++) {
                        if (p1_shape[i] != to_shape[i]) {
                            axes.push_back(i);
                        }
                    }
                    return axes;
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad.sum(get_broadcast_axes());
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const Broadcast>(op);
                        return symbolic_equals(parent, cast_op->parent) and to_shape == cast_op->to_shape;
                    }
                    return false;
                }
            };


            /** Performs a summation reduction along the axes specified */
            class Sum : public UnaryOperator {
            public:
                Axes axes;

                Sum(GraphInPtr graph,
                    Node parent,
                    Axes axes) :
                        UnaryOperator("Sum", graph, parent),
                        axes(axes) {
                    if (not validate_axes(axes)) {
                        std::string axes_str;
                        for (int i = 0; i < axes.size(); i++) {
                            axes_str += std::to_string(axes[i]);
                            if (i < axes.size() - 1) {
                                axes_str += ", ";
                            }
                        }
                        if (axes.size() == 0) {
                            axes_str = "NULL";
                        }
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent}, name, "Invalid axes: " + axes_str);
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sum>(graph, ancestors[0], axes);
                }

                Shape get_shape() const {
                    Shape p_shape = parent->shape;
                    for (int i = 0; i < axes.size(); i++) {
                        p_shape[axes[i]] = 1;
                    }
                    return p_shape;
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad.broadcast(parent->shape);
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const Sum>(op);
                        return symbolic_equals(parent, cast_op->parent) and axes == cast_op->axes;
                    }
                    return false;
                }

            };

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
                    return my_grad.neg();
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
                        if (my_grad->op->name == "Ones") {
                            return parents[1 - index];
                        } else if (my_grad->op->name == "Zeros") {
                            return my_grad;
                        }
                        if (parents[1 - index]->op->name == "Ones") {
                            return my_grad;
                        } else if (parents[1 - index]->op->name == "Zeros") {
                            return parents[1 - index];
                        }
                        return apply<Mul>(my_grad, parents[1 - index]);
                    } else {
                        Node product = apply<Mul>(my_grad, owner);
                        return apply<Mul>(product, parents[index].div());
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
            class Div : public UnaryOperator {
            public:
                Div(GraphInPtr graph, Node parent) :
                        UnaryOperator("Div", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Div>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return my_grad->graph->mul({my_grad, parent.square().div()}).neg();
                }
            };
        }
    }
}

#endif //METADIFF_OPERATORS_BASE_H
