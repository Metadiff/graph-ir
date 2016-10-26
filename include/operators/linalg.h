//
// Created by alex on 16/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_LINALG_H
#define METADIFF_CORE_OPERATORS_LINALG_H

namespace md{
    namespace core {
        namespace op {
            /** General Matrix-Matrix Multiplication (GEMM) */
            class MatrixMul : public AssociativeOperator {
            public:
                std::vector<bool> transpositions;
                MatrixMul(GraphInPtr graph,
                          NodeVec parents,
                          std::vector<bool> transpositions) :
                        AbstractOperator("MatrixMul", graph), AssociativeOperator(parents),
                        transpositions(transpositions){
                    Error err;
                    if (parents[0].dims() > 2) {
                        err = std::make_shared<InvalidArguments>
                                (parents, name, "Parent 0 is not a matrix.");
                        err->log(logger());
                        throw err;
                    }
                    if(transpositions.size() == 0){
                        for(auto i=0; i < parents.size(); ++i){
                            transpositions.push_back(false);
                        }
                    }
                    Shape shape = parents[0]->shape;
                    SymInt temp;
                    if(transpositions[0]){
                        temp = shape[0];
                        shape[0] = shape[1];
                        shape[1] = temp;
                    }
                    for (int i = 1; i < parents.size(); i++) {
                        if (parents[i].dims() > 2) {
                            err = std::make_shared<InvalidArguments>
                                    (parents, name,
                                     "Parent " + std::to_string(i) + " is not a matrix.");
                            break;
                        }
                        if(parents[i]->shape[transpositions[i]] != shape[1]){
                            err = std::make_shared<IncompatibleShapes>(parents, name);
                            break;
                        }
                        shape[1] = parents[i]->shape[1 - transpositions[i]];
                    }
                    if(err){
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixMul>(graph, ancestors, transpositions);
                }

                Shape get_shape() const{
                    return {parents[0]->shape[transpositions[0]], parents.back()->shape[not transpositions.back()], 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, short index) {
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
                    return graph->gemm(left_mats, left_trans);
                }


                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    NodeVec product;
                    for(auto i=0; i<parents.size(); ++i){
                        if(i == index){
                            product.push_back(parent_derivatives[i]);
                        } else{
                            product.push_back(parents[i]);
                        }
                    }
                    return graph->gemm(product, transpositions);
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
                        AbstractOperator("MatrixInverse", graph), UnaryOperator(parent) {
                    if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent must be a square matrix.");
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverse>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    return graph->neg(graph->gemm({owner, my_derivative, owner}, {true, false, true}));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->neg(graph->gemm({owner, parent_derivatives[index], owner}, {false, false, false}));
                }
            };

            /** MatrixInverse times a second matrix - the same as solving the linear system */
            class MatrixInverseMul : public BinaryFloatElementwiseOperator {
            public:
                bool transpose;
                MatrixInverseMul(GraphInPtr graph, Node parent1, Node parent2, bool tranpose = false) :
                        AbstractOperator("MatrixInverseMul", graph), BinaryOperator(parent1, parent2),
                        transpose(tranpose){
                    if (parent1->shape[0] != parent1->shape[1] or parent1->shape[2] != 1 or parent1->shape[2] != 1) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent1, parent2}, name, "Parent 1 must be a square matrix.");
                        err->log(logger());
                        throw err;
                    }

                    if (parent1->shape[1-tranpose] != parent2->shape[0]){
                        auto err = std::make_shared<IncompatibleShapes>(NodeVec{parent1, parent2}, name);
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MatrixInverseMul>(graph, ancestors[0], ancestors[1], transpose);
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    if(index == 0){
                        auto solve1 = graph->matrix_inverse_mul(parent1, my_derivative, true);
                        Node dot;
                        if(transpose){
                            dot =  graph->gemm({owner, solve1}, {false, true});
                        } else {
                            dot = graph->gemm({solve1, owner}, {false, true});
                        }
                        return graph->neg(dot);
                    } else {
                        return graph->matrix_inverse_mul(parent1, my_derivative, not transpose);
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    if(index == 0){
                        auto dot1 = graph->dot(parent_derivatives[index], owner, transpose);
                        auto solve1 = graph->matrix_inverse_mul(parent1, dot1, transpose);
                        return graph->neg(solve1);
                    } else {
                        return graph->matrix_inverse_mul(parent1, parent_derivatives[index], transpose);
                    }
                }
            };

            /** Determinant of a square matrix */
            class Determinant : public FloatUnaryOperator {
            public:
                Determinant(GraphInPtr graph, Node parent) :
                        AbstractOperator("Determinant", graph), UnaryOperator(parent) {
                    if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent must be a square matrix.");
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Determinant>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    return graph->mul(my_derivative, owner, graph->transpose(graph->matrix_inverse(parent)));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    auto trace = graph->trace(graph->matrix_inverse_mul(parent, parent_derivatives[index]));
                    return graph->mul(owner, trace);
                }

            };

            /** The natural logarithm of the determinant a square matrix */
            class LogDeterminant : public FloatUnaryOperator {
            public:
                LogDeterminant(GraphInPtr graph, Node parent) :
                        AbstractOperator("LogDeterminant", graph), UnaryOperator(parent) {
                    if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent must be a square matrix.");
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogDeterminant>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    return graph->mul(my_derivative, graph->transpose(graph->matrix_inverse(parent)));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->trace(graph->matrix_inverse_mul(parent, parent_derivatives[index]));
                }

            };

            /** The trace  of a square matrix */
            class Trace : public MorphOperator {
            public:
                Trace(GraphInPtr graph, Node parent) :
                        AbstractOperator("Trace", graph), UnaryOperator(parent) {
                    // TODO if parent is b8 cast it to max_int
                    if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                        auto err = std::make_shared<InvalidArguments>
                                (NodeVec{parent}, name, "Parent must be a square matrix.");
                        err->log(logger());
                        throw err;
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Trace>(graph, ancestors[0]);
                }

                Shape get_shape() const {
                    return {1, 1, 1, 1};
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    return graph->mul(my_derivative, graph->eye(parent->shape[0]));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->trace(parent_derivatives[index]);
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_LINALG_H
