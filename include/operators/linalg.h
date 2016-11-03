//
// Created by alex on 16/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_LINALG_H
#define METADIFF_GRAPH_IR_OPERATORS_LINALG_H

namespace md{
    namespace gir {
        namespace op {
            /** General Matrix-Matrix Multiplication (GEMM) */
            class MatrixMul : public AssociativeOperator {
            public:
                std::vector<bool> transpositions;
                MatrixMul(GraphInPtr graph,
                          NodeVec parents,
                          std::vector<bool> transpositions) :
                        AbstractOperator(graph, "MatrixMul"), AssociativeOperator(parents),
                        transpositions(transpositions){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixMul>(graph, ancestors, transpositions);
                }

                Shape get_shape() const{
                    return {parents[0]->shape[transpositions[0]], parents.back()->shape[not transpositions.back()], 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    NodeVec left_mats, right_mats;
                    std::vector<bool> left_trans, right_trans;
                    for (auto i = 0; i < index; ++i) {
                        if(transpositions[i]){
                            right_mats.push_back(parents[i]);
                            right_trans.push_back(transpositions[i]);
                        } else {
                            left_mats.push_back(parents[i]);
                            left_trans.push_back(not transpositions[i]);
                        }
                    }
                    for (auto i = index + 1; i < parents.size(); ++i) {
                        if(transpositions[i]){
                            left_mats.push_back(parents[i]);
                            left_trans.push_back(transpositions[i]);
                        } else {
                            right_mats.push_back(parents[i]);
                            right_trans.push_back(not transpositions[i]);
                        }
                    }
                    // Have to reverse these
                    if(not transpositions[index]){
                        std::reverse(left_mats.begin(), left_mats.end());
                        std::reverse(left_trans.begin(), left_trans.end());
                        std::reverse(right_mats.begin(), right_mats.end());
                        std::reverse(right_trans.begin(), right_trans.end());
                    }
                    left_mats.push_back(my_derivative);
                    left_mats.insert(left_mats.end(), right_mats.begin(), right_mats.end());
                    left_trans.push_back(transpositions[index]);
                    left_trans.insert(left_trans.end(), right_trans.begin(), right_trans.end());
                    return matrix_mul(left_mats, left_trans);
                }


                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    NodeVec product;
                    for(auto i=0; i<parents.size(); ++i){
                        if(i == index){
                            product.push_back(parent_derivatives[i]);
                        } else{
                            product.push_back(parents[i]);
                        }
                    }
                    return matrix_mul(product, transpositions);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        if (parents.size() != op->get_parents().size()) {
//                            return false;
//                        }
//                        for (int i = 0; i < parents.size(); i++) {
//                            if (not symbolic_equals(parents[i], op->get_parents()[i])) {
//                                return false;
//                            }
//                        }
//                        return true;
//                    }
//                    return false;
//                }
            };

            /** MatrixInverse */
            class MatrixInverse : public FloatUnaryElementwiseOperator {
            public:
                MatrixInverse(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "MatrixInverse"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverse>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return neg(matrix_mul({result, my_derivative, result}, {true, false, true}));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return neg(matrix_mul({result, parent_derivatives[index], result}, {false, false, false}));
                }
            };

            /** MatrixInverse times a second matrix - the same as solving the linear system */
            class MatrixInverseMul : public BinaryFloatElementwiseOperator {
            public:
                bool transpose;
                MatrixInverseMul(GraphInPtr graph, Node parent1, Node parent2, bool tranpose = false) :
                        AbstractOperator(graph, "MatrixInverseMul"), BinaryOperator(parent1, parent2),
                        transpose(tranpose){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverseMul>(graph, ancestors[0], ancestors[1], transpose);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    if(index == 0){
                        auto solve1 = matrix_inverse_mul(parent1, my_derivative, true);
                        if(transpose){
                            return - dot(result, solve1, false, true);
                        } else {
                            return - dot(solve1, result, false, true);
                        }
                    } else {
                        return matrix_inverse_mul(parent1, my_derivative, not transpose);
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    if(not parent_derivatives[0].ptr.expired() and
                            not parent_derivatives[1].ptr.expired()){
                        if(index == 0){
                            auto d = dot(parent_derivatives[0], result, transpose);
                            auto diff = neg(parent_derivatives[1], d);
                            return matrix_inverse_mul(parent1, diff, transpose);
                        } else {
                            return Node();
                        }
                    } else if(index == 0){
                        auto d = dot(parent_derivatives[index], result, transpose);
                        auto solve = matrix_inverse_mul(parent1, d, transpose);
                        return neg(solve);
                    } else {
                        return matrix_inverse_mul(parent1, parent_derivatives[index], transpose);
                    }
                }
            };

            /** Determinant of a square matrix */
            class Determinant : public FloatUnaryOperator {
            public:
                Determinant(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Determinant"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Determinant>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, result, transpose(matrix_inverse(parent)));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto tr = trace(matrix_inverse_mul(parent, parent_derivatives[index]));
                    return mul(result, tr);
                }

            };

            /** The natural logarithm of the determinant a square matrix */
            class LogDeterminant : public FloatUnaryOperator {
            public:
                LogDeterminant(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "LogDeterminant"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogDeterminant>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, transpose(matrix_inverse(parent)));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return trace(matrix_inverse_mul(parent, parent_derivatives[index]));
                }

            };

            /** The trace  of a square matrix */
            class Trace : public MorphOperator {
            public:
                Trace(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Trace"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Trace>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, graph->eye(parent->shape[0]));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return trace(parent_derivatives[index]);
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_LINALG_H
