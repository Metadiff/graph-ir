//
// Created by alex on 18/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_SHAPE_H
#define METADIFF_CORE_OPERATORS_SHAPE_H

namespace md{
    namespace core {
        namespace op {

            /**
             * 1. If parent is a square matrix returns a vector of the diagonal elements
             * 2. If parent is a vector returns a square matrix, whose diagonal is equal to the parent
             */
            class Diagonal : public MorphOperator {
            public:
                Diagonal(GraphInPtr graph, Node parent) :
                        AbstractOperator("Diagonal", graph), UnaryOperator(parent) {
                    if (parent.dims() > 2) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent is not a matrix or a vector.");
                        err->log(logger());
                        throw err;
                    }
                    if (parent.dims() == 2 and parent->shape[0] != parent->shape[1]){
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent is not a square matrix.");
                        err->log(logger());
                        throw err;
                    }
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Diagonal>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    if(parent.dims() == 2){
                        return {parent->shape[0], 1, 1, 1};
                    } else {
                        return {parent->shape[0], parent->shape[1], 1, 1};
                    }
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->diag(my_derivative);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->diag(parent_derivatives[index]);
                }
            };

            /** Reshapes the input to a specified shape */
            class Reshape : public MorphOperator {
            public:
                Shape shape;

                Reshape(GraphInPtr graph, Node parent, Shape shape) :
                        AbstractOperator("Reshape", graph), UnaryOperator(parent),
                        shape(shape) {
                    if (number_of_elements(parent->shape) != number_of_elements(shape)) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Total number of elements must not change.");
                        err->log(logger());
                        throw err;
                    }
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Reshape>(graph, ancestors[0], shape);
                }

                Shape get_shape() const {
                    return shape;
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->reshape(my_derivative, parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->reshape(parent_derivatives[index], shape);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Reshape>(op);
//                        return symbolic_equals(parent, cast_op->parent) and shape == cast_op->shape;
//                    }
//                    return false;
//                }
            };


            /**
             * Reorders the axes of tensor
             */
            class Reorder : public MorphOperator {
            public:
                Axes order;

                Reorder(GraphInPtr graph,
                        Node parent,
                        Axes order) :
                        AbstractOperator("Reorder", graph), UnaryOperator(parent),
                        order(order) {
                    std::shared_ptr<GraphError> err;
                    if (order.size() > 4) {
                        err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name,
                                 "The ordering must contain no more than 4 elements");
                    } else if (parent.dims() == 4 and order.size() != 4) {
                        err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name,
                                 "The ordering for a 4 dimensional tensor should contain exactly 4 elements");
                    } else if (parent.dims() == 3 and order.size() != 3) {
                        err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name,
                                 "The ordering for a 3 dimensional tensor should contain at least 3 elements");
                    } else if (parent.dims() == 2 and order.size() != 2) {
                        err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name,
                                 "The ordering for a matrix should contain at least 2 elements");
                    } else if (order.size() == 0) {
                        err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name,
                                 "The ordering must contain at least 1 element");
                    }
                    if (err) {
                        err->log(logger());
                        throw err;
                    }
                    std::vector<bool> checks;
                    for (int i = 0; i < order.size(); i++) {
                        checks.push_back(false);
                    }
                    for (int i = 0; i < order.size(); i++) {
                        if (0 > order[i] or order[i] > 4) {
                            err = std::make_shared<InvalidArguments>
                                    (NodeVec{parent}, name,
                                     "The ordering must contain elements in the range [0,3]");
                            break;
                        }
                        if (checks[order[i]]) {
                            err = std::make_shared<InvalidArguments>
                                    (NodeVec{this->parent}, name,
                                     "The ordering must not have repeating elements");
                            break;
                        }
                        checks[i] = true;
                    }
                    if (err) {
                        err->log(logger());
                        throw err;
                    }
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Reorder>(graph, ancestors[0], order);
                }

                Shape get_shape() const {
                    Shape shape = {1, 1, 1, 1};
                    for (int i = 0; i < 4; i++) {
                        if (i < order.size()) {
                            shape[i] = parent->shape[order[i]];
                        }
                    }
                    return shape;
                }

                static Axes reverse_order(Axes &order) {
                    Axes reversed;
                    // 2, 0, 1, 3
                    // 1, 2, 0, 3
                    for (unsigned short i = 0; i < 4; i++) {
                        reversed[order[i]] = i;
                    }
                    return reversed;
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->reorder(my_derivative, reverse_order(order));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->reorder(parent_derivatives[index], order);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Reorder>(op);
//                        return symbolic_equals(parent, cast_op->parent) and order == cast_op->order;
//                    }
//                    return false;
//                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_SHAPE_H
