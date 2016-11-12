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
                std::vector<bool> t;
                MatrixMul(GraphInPtr graph,
                          NodeVec parents,
                          std::vector<bool> t) :
                        AbstractOperator(graph, "MatrixMul"), AssociativeOperator(parents),
                        t(t){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixMul>(graph, ancestors, t);
                }

                Shape get_shape() const{
                    return {parents[0]->shape[t[0]], parents.back()->shape[not t.back()], 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    NodeVec left_mats, right_mats;
                    std::vector<bool> left_trans, right_trans;
                    for (auto i = 0; i < index; ++i) {
                        if(t[index]){
                            right_mats.push_back(parents[i]);
                            right_trans.push_back(t[i]);
                        } else {
                            left_mats.push_back(parents[i]);
                            left_trans.push_back(not t[i]);
                        }
                    }
                    for (auto i = index + 1; i < parents.size(); ++i) {
                        if(t[index]){
                            left_mats.push_back(parents[i]);
                            left_trans.push_back(t[i]);
                        } else {
                            right_mats.push_back(parents[i]);
                            right_trans.push_back(not t[i]);
                        }
                    }
                    // Have to reverse these
                    if(not t[index]){
                        std::reverse(left_mats.begin(), left_mats.end());
                        std::reverse(left_trans.begin(), left_trans.end());
                        std::reverse(right_mats.begin(), right_mats.end());
                        std::reverse(right_trans.begin(), right_trans.end());
                    }
                    left_mats.push_back(my_derivative);
                    left_mats.insert(left_mats.end(), right_mats.begin(), right_mats.end());
                    left_trans.push_back(t[index]);
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
                    return matrix_mul(product, t);
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
            class MatrixInverse : public FloatUnaryOperator {
            public:
                bool t;
                MatrixInverse(GraphInPtr graph, Node parent, bool t) :
                        AbstractOperator(graph, "MatrixInverse"), UnaryOperator(parent),
                        t(t){}

                Shape get_shape() const {
                    return parent->shape;
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverse>(graph, ancestors[0], t);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return - matrix_mul({result, my_derivative, result}, {not t, t, not t});
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return - matrix_mul({result, parent_derivatives[index], result}, {false, t, false});
                }
            };

            /** MatrixInverse times a second matrix - the same as solving the linear system */
            class MatrixInverseMul : public BinaryFloatElementwiseOperator {
            public:
                bool t_inv;
                bool t_mul;
                MatrixInverseMul(GraphInPtr graph, Node parent1, Node parent2, bool t_inv, bool t_mul) :
                        AbstractOperator(graph, "MatrixInverseMul"), BinaryOperator(parent1, parent2),
                        t_inv(t_inv), t_mul(t_mul){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverseMul>(graph, ancestors[0], ancestors[1], t_inv, t_mul);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    if(index == 0){
                        auto solve1 = matrix_inverse_mul(parent1, my_derivative, not t_inv);
                        if(t_inv){
                            return - dot(result, solve1, false, true);
                        } else {
                            return - dot(solve1, result, false, true);
                        }
                    } else {
                        if(t_mul) {
                            return matrix_inverse_mul(my_derivative, parent1, true, t_inv);
                        } else {
                            return matrix_inverse_mul(parent1, my_derivative, not t_inv);
                        }
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    if(not parent_derivatives[0].ptr.expired() and
                       not parent_derivatives[1].ptr.expired()){
                        if(index == 0){
                            Node p1;
                            if(t_mul){
                                p1 = dot(result, parent_derivatives[0], true, not t_inv);
                            } else {
                                p1 = dot(parent_derivatives[0], result, t_inv);
                            }
                            return matrix_inverse_mul(parent1, parent_derivatives[1] - p1, t_inv, t_mul);
                        } else {
                            return Node();
                        }
                    } else if(index == 0){
                        auto p1 = dot(parent_derivatives[index], result, t_inv);
                        return - matrix_inverse_mul(parent1, p1, t_inv);
                    } else {
                        return matrix_inverse_mul(parent1, parent_derivatives[index], t_inv, t_mul);
                    }
                }
            };

            /** Kronecker product */
            class Kronecker: public BinaryOperator, public FloatOperator {
            public:
                Kronecker(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator(graph, "Kronecker"), BinaryOperator(parent1, parent2) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Kronecker>(graph, ancestors[0], ancestors[1]);
                }

                Shape get_shape() const {
                    return {parent1->shape[0] * parent2->shape[0], parent1->shape[1] * parent2->shape[1], 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    if(index == 0){
                        return kron(parent_derivatives[index], parent2);
                    } else {
                        return kron(parent1, parent_derivatives[index]);
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
                    return mul(my_derivative, result, matrix_inverse(parent, true));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto tr = trace(matrix_inverse_mul(parent, parent_derivatives[index]));
                    return result * tr;
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
                    return my_derivative * matrix_inverse(parent, true);
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
                    return my_derivative * graph->eye(parent->shape[0]);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return trace(parent_derivatives[index]);
                }
            };


            /** A special operator for the cholesky forward diff */
            class CholeskyForwardDiff : public BinaryOperator, public FloatOperator {
            public:
                bool lower;
                CholeskyForwardDiff(GraphInPtr graph, Node cholesky, Node parent_derivative, bool lower) :
                        AbstractOperator(graph, "CholeskyForwardDiff"), BinaryOperator(cholesky, parent_derivative),
                        lower(lower) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<CholeskyForwardDiff>(graph, ancestors[0], ancestors[1], lower);
                }

                Shape get_shape() const {
                    return parent1->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    throw NotImplementedError(__LINE__, __FILE__);
                }
            };

            /** A special operator for the cholesky forward diff */
            class CholeskyBackwardDiff : public BinaryOperator, public FloatOperator {
            public:
                bool lower;
                CholeskyBackwardDiff(GraphInPtr graph, Node cholesky, Node parent_derivative, bool lower) :
                        AbstractOperator(graph, "CholeskyBackwardDiff"), BinaryOperator(cholesky, parent_derivative),
                        lower(lower) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<CholeskyBackwardDiff>(graph, ancestors[0], ancestors[1], lower);
                }

                Shape get_shape() const {
                    return parent1->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    throw NotImplementedError(__LINE__, __FILE__);
                }
            };

            /** Returns the Cholesky decomposition of the input */
            class Cholesky : public FloatUnaryOperator {
            public:
                bool lower;
                Cholesky(GraphInPtr graph, Node parent, bool lower) :
                        AbstractOperator(graph, "Cholesky"), UnaryOperator(parent),
                        lower(lower) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cholesky>(graph, ancestors[0], lower);
                }

                Shape get_shape() const {
                    return parent->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    auto op = std::make_shared<CholeskyBackwardDiff>(graph, result, my_derivative, lower);
                    return graph->derived_node(op);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto op = std::make_shared<CholeskyForwardDiff>(graph, result, parent_derivatives[index], lower);
                    return graph->derived_node(op);
                }
            };

            /** Returns the packed LU decomposition */
            class LU : public FloatUnaryOperator {
            public:
                LU(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "LU"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LU>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return parent->shape;
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }
            };

            /** Returns the packed QR decomposition */
            class QR : public MultiOutputOperator {
            public:
                QR(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "QR"), MultiOutputOperator(2) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<QR>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return MultiOutputOperator::get_shape();
                }

                Shape get_shape_at(int index) const {
                    if(index == 0){
                        if(parent->shape[0] != parent->shape[1]){
                            //TODO
                            throw NotImplementedError(__LINE__, __FILE__);
                        }
                        return parent->shape;
                    } else {
                        return parent->shape;
                    }
                };

                DataType get_data_type_at(int index) const {
                    return parent->data_type;
                };

                bool is_differentiable_at(int index) const {
                    return is_differentiable();
                };

                Node backward_diff_parent_at(Node my_derivative, int index)  const {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent_at(NodeVec & parent_derivatives, int index) const {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }
            };

            /** Returns the SVD decomposition */
            class SVD : public MultiOutputOperator {
            public:
                SVD(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "SVD"), MultiOutputOperator(3) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<SVD>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return MultiOutputOperator::get_shape();
                }

                Shape get_shape_at(int index) const {
                    if(index == 0){
                        return {parent->shape[0], parent->shape[0], 1, 1};
                    } else if(index == 1){
                        // TODO
                        throw NotImplementedError(__LINE__, __FILE__);
                    } else {
                        return {parent->shape[1], parent->shape[1], 1, 1};
                    }
                };

                DataType get_data_type_at(int index) const {
                    return parent->data_type;
                };

                bool is_differentiable_at(int index) const {
                    return is_differentiable();
                };

                Node backward_diff_parent_at(Node my_derivative, int index) const {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }

                Node forward_diff_parent_at(NodeVec & parent_derivatives, int index) const {
                    // TODO
                    throw NotImplementedError(__LINE__, __FILE__);
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_LINALG_H
