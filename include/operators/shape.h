//
// Created by alex on 18/12/15.
//

#ifndef METADIFF_OPERATORS_SHAPE_H
#define METADIFF_OPERATORS_SHAPE_H

namespace metadiff{
    namespace op {
        using namespace core;
        using namespace exceptions;

        /**
         * 1. If parent is a square matrix returns a vector of the diagonal elements
         * 2. If parent is a vector returns a square matrix, whose diagonal is equal to the parent
         */
        class Diagonal : public UnaryOperator {
        public:
            Shape shape;
            Diagonal(GraphInPtr graph, Node parent) :
                    UnaryOperator("Diag", graph, parent) {
                if (not parent.is_matrix()) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent is not a matrix or a vector.");
                    logger()->error() << err.msg;
                    throw err;
                }
                if (parent.is_vector()) {
                    shape = {parent->shape[0], parent->shape[0], SymInt::one, SymInt::one};
                } else if (parent->shape[0] != parent->shape[1]) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent is not a square matrix.");
                    logger()->error() << err.msg;
                    throw err;
                } else {
                    shape = {parent->shape[0], SymInt::one, SymInt::one, SymInt::one};
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Diagonal>(graph, ancestors[0]);
            }

            Shape get_shape() const {
                return shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return my_grad.diag();
            }
        };

        /** Reshapes the input to a specified shape */
        class Reshape : public UnaryOperator {
        public:
            Shape shape;
            Reshape(GraphInPtr graph, Node parent, Shape shape) :
                    UnaryOperator("Reshape", graph, parent),
                    shape(shape) {
                SymInt product_parent = number_of_elements(parent->shape);
                SymInt product_shape = number_of_elements(shape);
                if (product_parent != product_shape) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Total number of elements must not change.");
                    logger()->error() << err.msg;
                    throw err;
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Reshape>(graph, ancestors[0], shape);
            }

            Shape get_shape() const {
                return shape;
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return my_grad.reshape(parent->shape);
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (name == op->name) {
                    auto cast_op = std::static_pointer_cast<const Reshape>(op);
                    return symbolic_equals(parent, cast_op->parent) and shape == cast_op->shape;
                }
                return false;
            }
        };


        /**
         * Reorders the axis of tensor
         */
        class Reorder : public UnaryOperator {
        public:
            Axes order;
            Reorder(GraphInPtr graph,
                    Node parent,
                    Axes order) :
                    UnaryOperator("Reorder", graph, parent),
                    order(order) {
                InvalidArguments err = InvalidArguments();
                if(order.size() > 4){
                    err = InvalidArguments(NodeVec{parent}, name,
                                           "The ordering must contain no more than 4 elements");
                } else if(parent.is_tensor4_strict() and order.size() < 4){
                    throw InvalidArguments(NodeVec{parent}, name,
                                           "The ordering for a 4 dimensional tensor should contain exactly 4 elements");
                } else if(parent.is_tensor3_strict() and order.size() < 3){
                    throw InvalidArguments(NodeVec{parent}, name,
                                           "The ordering for a 3 dimensional tensor should contain at least 3 elements");
                } else if(parent.is_matrix_strict() and order.size() < 2){
                    throw InvalidArguments(NodeVec{parent}, name,
                                           "The ordering for a matrix should contain at least 2 elements");
                } else if(order.size() == 0){
                    throw InvalidArguments(NodeVec{parent}, name,
                                           "The ordering must contain at least 1 element");
                }
                if (err.msg.compare("") != 0) {
                    logger()->error() << err.msg;
                    throw err;
                }
                std::vector<bool> checks;
                for(int i=0; i<order.size(); i++){
                    checks.push_back(false);
                }
                for (int i = 0; i < order.size(); i++) {
                    if (0 > order[i] or order[i] > 4) {
                        auto err = InvalidArguments(NodeVec{this->parent}, name, "The ordering must contain elements in the range [0,3]");
                        logger()->error() << err.msg;
                        throw err;
                    }
                    if (checks[order[i]]) {
                        auto err = InvalidArguments(NodeVec{this->parent}, name, "The ordering must not have repeating elements");
                        logger()->error() << err.msg;
                        throw err;
                    }
                    checks[i] = true;
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Reorder>(graph, ancestors[0], order);
            }

            Shape get_shape() const {
                Shape shape = scalar_shape;
                for(int i=0; i<4; i++){
                    if(i < order.size()){
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

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return my_grad.reorder(reverse_order(order));
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (name == op->name) {
                    auto cast_op = std::static_pointer_cast<const Reorder>(op);
                    return symbolic_equals(parent, cast_op->parent) and order == cast_op->order;
                }
                return false;
            }
        };
    }

    namespace core{
        Node Node::diag() {
            // TODO a.diag().diag() = a
            return apply<op::Diagonal>(this);
        }

        Node Node::reshape(Shape shape) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::Reshape>(graph, this, shape));
        }

        Node Node::flatten(unsigned short dims) {
            std::shared_ptr<NodeInternal> ptr = unwrap();
            if (dims == 0 or dims > 4) {
                auto err = InvalidArguments(ptr->op->get_parents(), "Flatten", "dims = " + std::to_string(dims) + " is outside [1,4]");
                ptr->op->logger()->error() << "Flatten" << "] " << err.msg;
                throw err;
            }
            Shape shape = ptr->shape;
            for (int i = 3; i >= dims; i--) {
                shape[i - 1] = shape[i] * shape[i - 1];
                shape[i] = SymInt::one;
            }
            return reshape(shape);
        }

        Node Node::reorder(Axes order) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::Reorder>(graph, this, order));
        }

        Node Node::reorder(short dim0, short dim1, short dim2, short dim3) {
            return reorder(Axes{dim0, dim1, dim2, dim3});
        }
    }
}
#endif //METADIFF_OPERATORS_SHAPE_H
