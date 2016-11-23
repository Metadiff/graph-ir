//
// Created by alex on 15/02/16.
//

#ifndef METADIFF_GRAPH_IR_INDEX_H
#define METADIFF_GRAPH_IR_INDEX_H

namespace md {
    namespace gir {
        namespace op {
            /** Takes a contigous chunk of array */
            class Slice : public UnaryOperator {
            public:
                Axes axes;
                std::vector <std::pair<SymInt, SymInt>> slices;

                Slice(GraphInPtr graph, Node parent,
                      Axes axes, std::vector <std::pair<SymInt, SymInt>> slices) :
                        AbstractOperator(graph, "Slice"), UnaryOperator(parent),
                        axes(axes), slices(slices) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Slice>(graph, ancestors[0], axes, slices);
                }

                Shape get_shape() const {
                    Shape result = parent->shape;
                    for(auto i=0; i<axes.size(); ++i){
                        result[axes[i]] = slices[i].second - slices[i].first;
                    }
                    return result;
                }

                DataType get_data_type() const {
                    return parent->data_type;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return Node();
                }

                Node forward_diff_parent(NodeVec& parent_derivatives, int index){
                    return Node();
                }
            };

//            bool equals(const std::shared_ptr<Operator> op) const {
//                if (name == op->name) {
//                    std::shared_ptr<Slice> cast_op = std::static_pointer_cast<Slice>(op);
//                    return symbolic_equals(parent, cast_op->parent) and symbolic_equals(index, cast_op->index)
//                           and axis == cast_op->axis;
//                }
//                return false;
//            }

//            Node backward_diff(Node my_grad, size_t index);
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
//                    throw InvalidArguments(name, {result}, "Axis should be between [0, 3]");
//                }
//                if (not index.is_vector()) {
//                    throw InvalidArguments(name, {result}, "The indexes should be a column vector");
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
//            Node backward_diff(Node my_grad, size_t index);
//
//        };
//
//        Node Slice::backward_diff(Node my_grad, size_t index) {
//            return graph->derived_node(
//                    std::make_shared<SliceGrad>(graph, my_grad, this->index, axis, result.unwrap()->shape));
//        }
//
//        Node SliceGrad::backward_diff(Node my_grad, size_t index) {
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
//                    throw InvalidArguments(name, {result}, "Axis should be between [0, 3]");
//                }
//                for (size_t i = 0; i < 4; i++) {
//                    if (axis != i and parent.unwrap()->shape[i] != index.unwrap()->shape[i]) {
//                        throw InvalidArguments(name, {result}, "The shape of the node and its index should be equal"
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
//            Node backward_diff(Node my_grad, size_t index);
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
//                    throw InvalidArguments(name, {result}, "Axis should be between [0, 3]");
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
//            Node backward_diff(Node my_grad, size_t index);
//        };
//
//        Node Index::backward_diff(Node my_grad, size_t index) {
//            return graph->derived_node(
//                    std::make_shared<IndexGrad>(graph, my_grad, this->index, axis, result.unwrap()->shape[axis]));
//        }
//
//        Node IndexGrad::backward_diff(Node my_grad, size_t index) {
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
}
#endif //METADIFF_GRAPH_IR_INDEX_H
