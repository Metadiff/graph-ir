//
// Created by alex on 18/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_SHAPE_H
#define METADIFF_GRAPH_IR_OPERATORS_SHAPE_H

namespace md{
    namespace gir {
        namespace op {

            /**
             * 1. If parent is a square matrix returns a vector of the diagonal elements
             * 2. If parent is a vector returns a square matrix, whose diagonal is equal to the parent
             */
            class Diagonal : public MorphOperator {
            public:
                Diagonal(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Diagonal"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Diagonal>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    if(parent.order() == 2){
                        return {parent->shape[0], 1, 1, 1};
                    } else {
                        return {parent->shape[0], parent->shape[1], 1, 1};
                    }
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return diag(my_derivative);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return diag(parent_derivatives[index]);
                }
            };

            /** Takes the lower triangular part of a matrix */
            class LowerTriangular : public MorphOperator {
            public:
                double k;
                LowerTriangular(GraphInPtr graph, Node parent, double k) :
                        AbstractOperator(graph, "LowerTriangular"), UnaryOperator(parent),
                        k(k) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LowerTriangular>(graph, ancestors[0], k);
                }

                Shape get_shape() const {
                    return parent->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return lower_tri(my_derivative, k);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return lower_tri(parent_derivatives[index], k);
                }
            };

            /** Takes the upper triangular part of a matrix */
            class UpperTriangular : public MorphOperator {
            public:
                double k;
                UpperTriangular(GraphInPtr graph, Node parent, double k) :
                        AbstractOperator(graph, "UpperTriangular"), UnaryOperator(parent),
                        k(k){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<UpperTriangular>(graph, ancestors[0], k);
                }

                Shape get_shape() const {
                    return parent->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return upper_tri(my_derivative, k);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return upper_tri(parent_derivatives[index], k);
                }
            };

            /** Reshapes the input to a specified shape */
            class Reshape : public MorphOperator {
            public:
                Shape shape;

                Reshape(GraphInPtr graph, Node parent, Shape shape) :
                        AbstractOperator(graph, "Reshape"), UnaryOperator(parent),
                        shape(shape) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Reshape>(graph, ancestors[0], shape);
                }

                Shape get_shape() const {
                    return shape;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return reshape(my_derivative, parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return reshape(parent_derivatives[index], shape);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Reshape>(op);
//                        return symbolic_equals(parent, cast_op->parent) and shape == cast_op->shape;
//                    }
//                    return false;
//                }
            };


            /** Reorders the axes of tensor */
            class Reorder : public MorphOperator {
            public:
                Axes order;

                Reorder(GraphInPtr graph,
                        Node parent,
                        Axes order) :
                        AbstractOperator(graph, "Reorder"), UnaryOperator(parent),
                        order(order) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Reorder>(graph, ancestors[0], order);
                }

                Shape get_shape() const {
                    Shape shape = parent->shape;
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
                    for (auto i = 0; i < 4; ++i) {
                        reversed[order[i]] = i;
                    }
                    return reversed;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return reorder(my_derivative, reverse_order(order));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return reorder(parent_derivatives[index], order);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Reorder>(op);
//                        return symbolic_equals(parent, cast_op->parent) and order == cast_op->order;
//                    }
//                    return false;
//                }
            };

            /** Filps the elements along the axes of tensor */
            class Flip : public MorphOperator {
            public:
                Axes axes;

                Flip(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator(graph, "Flip"), UnaryOperator(parent),
                        axes(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Flip>(graph, ancestors[0], axes);
                }

                Shape get_shape() const {
                    return parent->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return flip(my_derivative, axes);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return flip(parent_derivatives[index], axes);
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
#endif //METADIFF_GRAPH_IR_OPERATORS_SHAPE_H
