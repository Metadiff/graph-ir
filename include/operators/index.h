//
// Created by alex on 15/02/16.
//

#ifndef METADIFF_INDEX_H
#define METADIFF_INDEX_H

namespace metadiff{
    namespace op {
        using namespace core;
        using namespace exceptions;

        // TODO Think well how to design the indexing operations
//        /**
//         * Takes the slice of the parent on the axis specified
//         * according to the index argument
//         */
//        class Slice : public UnaryOperator {
//        public:
//            Node index;
//            size_t axis;
//
//            Slice(GraphInPtr graph, Node parent, Node index, size_t axis) :
//                    UnaryOperator("Slice", graph, parent),
//                    index(index),
//                    axis(axis) {
//                if (axis > 3) {
//                    throw InvalidArguments(name, {owner}, "Axis should be between [0, 3]");
//                }
//                if (not index.is_vector()) {
//                    throw InvalidArguments(name, {owner}, "The indexes should be a column vector");
//                }
//            };
//
//            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
//                return std::make_shared<Slice>(graph, ancestors[0], ancestors[1], axis);
//            }
//
//            Shape get_shape() const {
//                Shape result = parent.unwrap()->shape;
//                result[axis] = index.unwrap()->shape[0];
//                return result;
//            }
//
//            NodeVec get_arguments() const {
//                return NodeVec {index};
//            }
//
//            bool equals(const std::shared_ptr<Operator> op) const {
//                if (name == op->name) {
//                    std::shared_ptr<Slice> cast_op = std::static_pointer_cast<Slice>(op);
//                    return symbolic_equals(parent, cast_op->parent) and symbolic_equals(index, cast_op->index)
//                           and axis == cast_op->axis;
//                }
//                return false;
//            }
//
//            Node get_parent_grad(Node my_grad, size_t index);
//        };
//
//        /**
//         * This is the gradient of the slice.
//         * To some extend this can be thought of as sub-increment operation
//         */
//        class SliceGrad : public UnaryOperator {
//        public:
//            Node index;
//            size_t axis;
//            Shape result;
//
//            SliceGrad(GraphInPtr graph, Node parent, Node index, size_t axis, Shape result) :
//                    UnaryOperator("SliceGrad", graph, parent),
//                    index(index),
//                    axis(axis),
//                    result(result) {
//                if (axis > 3) {
//                    throw InvalidArguments(name, {owner}, "Axis should be between [0, 3]");
//                }
//                if (not index.is_vector()) {
//                    throw InvalidArguments(name, {owner}, "The indexes should be a column vector");
//                }
//            };
//
//            Shape get_shape() const {
//                return result;
//            }
//
//            NodeVec get_arguments() const {
//                return NodeVec {index};
//            }
//
//            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
//                return std::make_shared<SliceGrad>(graph, ancestors[0], ancestors[1], axis, result);
//            }
//
//            bool equals(const std::shared_ptr<Operator> op) const {
//                if (name == op->name) {
//                    std::shared_ptr<SliceGrad> cast_op = std::static_pointer_cast<SliceGrad>(op);
//                    return symbolic_equals(parent, cast_op->parent) and symbolic_equals(index, cast_op->index)
//                           and axis == cast_op->axis and result == cast_op->result;
//                }
//                return false;
//            }
//
//            Node get_parent_grad(Node my_grad, size_t index);
//
//        };
//
//        Node Slice::get_parent_grad(Node my_grad, size_t index) {
//            return graph->derived_node(
//                    std::make_shared<SliceGrad>(graph, my_grad, this->index, axis, owner.unwrap()->shape));
//        }
//
//        Node SliceGrad::get_parent_grad(Node my_grad, size_t index) {
//            return graph->derived_node(std::make_shared<Slice>(graph, my_grad, this->index, axis));
//        }
//
//        Node Node::slice(Node index, size_t axis) {
//            return unwrap()->graph->derived_node(std::make_shared<Slice>(unwrap()->graph, this, index, axis));
//        }
//
//        /**
//         * Takes the argument index as index of each entry along the axis speicified
//         */
//        class Index : public UnaryOperator {
//        public:
//            Node index;
//            size_t axis;
//
//            Index(GraphInPtr graph, Node parent, Node index, size_t axis) :
//                    UnaryOperator("Index", graph, parent),
//                    index(index),
//                    axis(axis) {
//                if (axis > 3) {
//                    throw InvalidArguments(name, {owner}, "Axis should be between [0, 3]");
//                }
//                for (size_t i = 0; i < 4; i++) {
//                    if (axis != i and parent.unwrap()->shape[i] != index.unwrap()->shape[i]) {
//                        throw InvalidArguments(name, {owner}, "The shape of the node and its index should be equal"
//                                " along all of the dimensions except the one specified by axis.");
//                    }
//                }
//            };
//
//            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
//                return std::make_shared<Index>(graph, ancestors[0], ancestors[1], axis);
//            }
//
//            Shape get_shape() const {
//                Shape result = parent.unwrap()->shape;
//                result[axis] = index.unwrap()->shape[axis];
//                return result;
//            }
//
//            NodeVec get_arguments() const {
//                return NodeVec {index};
//            }
//
//            bool equals(const std::shared_ptr<Operator> op) const {
//                if (name == op->name) {
//                    std::shared_ptr<Index> cast_op = std::static_pointer_cast<Index>(op);
//                    return symbolic_equals(parent, cast_op->parent) and symbolic_equals(index, cast_op->index)
//                           and axis == cast_op->axis;
//                }
//                return false;
//            }
//
//            Node get_parent_grad(Node my_grad, size_t index);
//        };
//
//        /**
//         * This is the gradient of the index.
//         * To some extend this can be thought of as sub-increment operation
//         */
//        class IndexGrad : public UnaryOperator {
//        public:
//            Node index;
//            size_t axis;
//            SymInt result;
//
//            IndexGrad(GraphInPtr graph, Node parent, Node index, size_t axis, SymInt result) :
//                    UnaryOperator("IndexGrad", graph, parent),
//                    index(index),
//                    axis(axis),
//                    result(result) {
//                if (axis > 3) {
//                    throw InvalidArguments(name, {owner}, "Axis should be between [0, 3]");
//                }
//            };
//
//            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
//                return std::make_shared<IndexGrad>(graph, ancestors[0], ancestors[1], axis, result);
//            }
//
//            Shape get_shape() const {
//                Shape result = parent.unwrap()->shape;
//                result[axis] = this->result;
//                return result;
//            }
//
//            NodeVec get_arguments() const {
//                return NodeVec {index};
//            }
//
//            bool equals(const std::shared_ptr<Operator> op) const {
//                if (name == op->name) {
//                    std::shared_ptr<IndexGrad> cast_op = std::static_pointer_cast<IndexGrad>(op);
//                    return symbolic_equals(parent, cast_op->parent) and symbolic_equals(index, cast_op->index)
//                           and axis == cast_op->axis and result == cast_op->result;
//                }
//                return false;
//            }
//
//            Node get_parent_grad(Node my_grad, size_t index);
//        };
//
//        Node Index::get_parent_grad(Node my_grad, size_t index) {
//            return graph->derived_node(
//                    std::make_shared<IndexGrad>(graph, my_grad, this->index, axis, owner.unwrap()->shape[axis]));
//        }
//
//        Node IndexGrad::get_parent_grad(Node my_grad, size_t index) {
//            return graph->derived_node(std::make_shared<Index>(graph, my_grad, this->index, axis));
//        }
//
//        Node Node::index(Node index, size_t axis) {
//            // Auto infer axis
//            if (axis == AUTO_INFER_AXIS) {
//                for (size_t i = 0; i < 4; i++) {
//                    if (unwrap()->shape[i] != index.unwrap()->shape[i]) {
//                        axis = i;
//                        break;
//                    }
//                }
//            }
//            return unwrap()->graph->derived_node(std::make_shared<Slice>(unwrap()->graph, this, index, axis));
//        }
    }
}
#endif //METADIFF_INDEX_H
