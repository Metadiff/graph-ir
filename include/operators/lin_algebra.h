//
// Created by alex on 16/12/15.
//

#ifndef METADIFF_OPERATORS_LIN_ALGEBRA_H
#define METADIFF_OPERATORS_LIN_ALGEBRA_H

namespace metadiff{
    namespace op {
        using namespace core;
        using namespace exceptions;

        /** Inverts the order of all non singular dimensions */
        class Transpose : public UnaryOperator {
        public:
            Transpose(GraphInPtr graph, Node parent) :
                    UnaryOperator("Transpose", graph, parent) {}

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Transpose>(graph, ancestors[0]);
            }

            Shape get_shape() const {
                Shape shape{SymInt::one, SymInt::one, SymInt::one, SymInt::one};
                int last_non_zero = 0;
                for (int i = 3; i >= 0; i--) {
                    if (parent->shape[i] != 1) {
                        last_non_zero = i;
                        break;
                    }
                }
                for (int i = 0; i <= last_non_zero; i++) {
                    shape[i] = parent->shape[last_non_zero - i];
                }
                return shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return my_grad.transpose();
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (parent->op->name == name) {
                    std::shared_ptr<Operator> base_op = parent->op->get_parents()[0]->op;
                    return base_op->equals(op) or op->equals(base_op);
                }
                if (name == op->name) {
                    auto cast_op = std::static_pointer_cast<const Transpose>(op);
                    return symbolic_equals(parent, cast_op->parent);
                }
                return false;
            }
        };

        /**
         * General Matrix-Matrix Multiplication (GEMM)
         */
        class MatrixMultiplication : public NaryOperator {
        public:
            MatrixMultiplication(GraphInPtr graph,
                                 NodeVec parents) :
                    NaryOperator("MatrixMul", graph, parents) {
                if (not parents[0].is_matrix()) {
                    auto err = InvalidArguments(parents, name, "Parent 0 is not a matrix.");
                    logger()->error() << err.msg;
                    throw err;
                }
                for (int i = 1; i < parents.size(); i++) {
                    if (not parents[i].is_matrix()) {
                        auto err = InvalidArguments(parents, name, "Parent " + std::to_string(i) + " is not a matrix.");
                        logger()->error() << err.msg;
                        throw err;
                    }
                    if (parents[i - 1]->shape[1] != parents[i]->shape[0]) {
                        auto err = IncompatibleShapes(parents, name);
                        logger()->error() << err.msg;
                        throw err;
                    }
                }
                shape = Shape{parents[0]->shape[0], parents.back()->shape[1], 1, 1};
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MatrixMultiplication>(graph, ancestors);
            }

            MatrixMultiplication(GraphInPtr graph,
                                 Node parent1,
                                 Node parent2) :
                    MatrixMultiplication(graph, {parent1, parent2}) { }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                NodeVec left_nodes;
                NodeVec right_nodes;
                for (size_t p = 0; p < index; p++) {
                    left_nodes.push_back(parents[p]);
                }
                for (size_t p = index + 1; p < parents.size(); p++) {
                    right_nodes.push_back(parents[p]);
                }
                Node left_tr = Node();
                Node right_tr = Node();
                if (left_nodes.size() == 1) {
                    left_tr = left_nodes[0].transpose();
                } else if (left_nodes.size() > 1) {
                    left_tr = apply<MatrixMultiplication>(left_nodes);
                    left_tr = left_tr.transpose();
                }

                if (right_nodes.size() == 1) {
                    right_tr = right_nodes[0].transpose();
                } else if (right_nodes.size() > 1) {
                    right_tr = apply<MatrixMultiplication>(right_nodes);
                    right_tr = right_tr.transpose();
                }

                std::shared_ptr<Operator> op;
                if (left_tr.ptr.expired()) {
                    return apply<MatrixMultiplication>(my_grad, right_tr);
                } else if (right_tr.ptr.expired()) {
                    return apply<MatrixMultiplication>(left_tr, my_grad);
                } else {
                    return apply<MatrixMultiplication>(NodeVec{left_tr, my_grad, right_tr});
                }
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (name == op->name) {
                    if (parents.size() != op->get_parents().size()) {
                        return false;
                    }
                    for (int i = 0; i < parents.size(); i++) {
                        if (not symbolic_equals(parents[i], op->get_parents()[i])) {
                            return false;
                        }
                    }
                    return true;
                }
                return false;
            }
        };

        /** MatrixInverse */
        class MatrixInverse : public UnaryOperator {
        public:
            MatrixInverse(GraphInPtr graph, Node parent) :
                    UnaryOperator("MatrixInv", graph, parent) {
                if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent must be a square matrix.");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MatrixInverse>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node this_tr = owner.transpose();
                return Node::dot(NodeVec{this_tr, my_grad, this_tr}).neg();
            }
        };

        /** Determinant of a square matrix */
        class Determinant : public UnaryOperator {
        public:
            Determinant(GraphInPtr graph, Node parent) :
                    UnaryOperator("Det", graph, parent) {
                if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent must be a square matrix.");
                    logger()->error() << err.msg;
                    throw err;
                }
                if (parent->dtype == dType::b8) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent can not be a b8");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Determinant>(graph, ancestors[0]);
            }

            Shape get_shape() const {
                return scalar_shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, owner, parent.minv().transpose()});
            }

        };

        /** The natural logarithm of the determinant a square matrix */
        class LogDeterminant : public UnaryOperator {
        public:
            LogDeterminant(GraphInPtr graph, Node parent) :
                    UnaryOperator("LogDet", graph, parent) {
                if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent must be a square matrix.");
                    logger()->error() << err.msg;
                    throw err;
                }
                if (parent->dtype == dType::b8) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent can not be a b8");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<LogDeterminant>(graph, ancestors[0]);
            }

            dType get_dtype() const {
                return graph->max_float;
            }

            Shape get_shape() const {
                return scalar_shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.minv().transpose()});
            }

        };

        /** The trace  of a square matrix */
        class Trace : public UnaryOperator {
        public:
            Trace(GraphInPtr graph, Node parent) :
                    UnaryOperator("Trace", graph, parent) {
                if (parent->shape[0] != parent->shape[1] or parent->shape[2] != 1 or parent->shape[2] != 1) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent must be a square matrix.");
                    logger()->error() << err.msg;
                    throw err;
                }
                if (parent->dtype == dType::b8) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent can not be a b8");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Trace>(graph, ancestors[0]);
            }



            Shape get_shape() const {
                return scalar_shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node eye = graph->eye(parent->shape[0]);
                eye->grad_level = my_grad->grad_level;
                return Node::mul(NodeVec{my_grad, eye});
            }
        };
    }
    namespace core{
        Node Node::transpose() {
            // TODO a.transpose().transpose() = a
            return apply<op::Transpose>(this);
        }

        Node Node::dot(NodeVec nodes) {
            // TODO a dot a.inv() = eye
            // TODO a.transpose() */dot b.transpose() = (a */dot b).transpose()
            return apply<op::MatrixMultiplication>(nodes);
        };

        Node Node::dot(Node node1, Node node2) {
            return Node::dot(NodeVec{node1, node2});
        };

        Node Node::minv() {
            // TODO a.minv().minv() = a
            return apply<op::MatrixInverse>(this);
        }

        Node Node::det() {
            return apply<op::Determinant>(this);
        }

        Node Node::logdet() {
            return apply<op::LogDeterminant>(this);
        }

        Node Node::trace() {
            return apply<op::Trace>(this);
        }
    }
}
#endif //METADIFF_OPERATORS_LIN_ALGEBRA_H
