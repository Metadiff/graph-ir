//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_OPERATORS_MULTY_NODE_H
#define METADIFF_OPERATORS_MULTY_NODE_H

namespace metadiff{
    namespace op {
        using namespace core;
        using namespace exceptions;
        
        /**
         * A common super class for special operators with more than 1 output
         * Because of how the gradients are set up only one node can be differentiable
         * TODO any use cases where this is not the case, or can we change it?
         * See: MaxAndArgMax, SortAndArgSort
         */
        class MultiNode : public UnaryOperator {
        public:
            unsigned short size;
            MultiNode(std::string const name,
                      GraphInPtr graph,
                      Node parent,
                      unsigned short size) :
                    UnaryOperator(name, graph, parent),
                size(size){
                if(size < 1){
                    auto err = InvalidArguments(NodeVec{parent}, name, "The size should be at least 1");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            virtual Shape get_shape(unsigned short index) const = 0;
            virtual dType get_dtype(unsigned short index) const = 0;
            virtual nodeType get_node_type(unsigned short index) const = 0;
            virtual Node child_to_my_grad(Node my_grad, unsigned short index)  = 0;

            Shape get_shape() const {
                return get_shape(0);
            }

            dType get_dtype() const {
                return get_dtype(0);
            }

            nodeType get_node_type() const{
                return get_node_type(0);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return my_grad;
            }
        };

        /**
         * An operator which selects one of the children of a MultiNode operator
         */
        class MultiNodeIndex : public Operator {
        public:
            Node parent;
            unsigned short index;
            
            MultiNodeIndex(GraphInPtr graph,
                           Node parent,
                           size_t index) :
                    Operator("MultyNodeIndex", graph),
                    parent(parent),
                    index(index) {
                std::shared_ptr<MultiNode> multi_op = std::dynamic_pointer_cast<MultiNode>(parent->op);
                if (not multi_op) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent must be a result of an operator of type 'MultiNode'.");
                    logger()->error() << err.msg;
                    throw err;
                }
                if (index >= multi_op->size) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Provided index is too big: " + std::to_string(index));
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MultiNodeIndex>(graph, ancestors[0], index);
            }

            Shape get_shape() const {
                std::shared_ptr<MultiNode> multi_op = std::static_pointer_cast<MultiNode>(parent->op);
                return multi_op->get_shape(index);
            }

            dType get_dtype() const {
                std::shared_ptr<MultiNode> multi_op = std::static_pointer_cast<MultiNode>(parent->op);
                return multi_op->get_dtype(index);
            }

            nodeType get_node_type() const {
                std::shared_ptr<MultiNode> multi_op = std::static_pointer_cast<MultiNode>(parent->op);
                return multi_op->get_node_type(index);
            };

            unsigned short get_grad_level() const {
                return parent->grad_level;
            }

            NodeVec get_parents() const {
                return {parent};
            }

            NodeVec get_arguments() const {
                return NodeVec {};
            }

            /** The MultiNode class is responsible for fetching correctly the child gradients */
            Node get_parent_grad(Node my_grad, unsigned short index) {
                std::shared_ptr<MultiNode> multi_op = std::static_pointer_cast<MultiNode>(parent->op);
                return multi_op->child_to_my_grad(my_grad, this->index);
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (name == op->name) {
                    auto cast_op = std::static_pointer_cast<const MultiNodeIndex>(op);
                    return symbolic_equals(parent, cast_op->parent) and index == cast_op->index;
                }
                return false;
            }
        };

        /** Return the max as first child and the argmax as second */
        class MaxAndArgMax : public MultiNode {
        public:
            short axis;
            MaxAndArgMax(GraphInPtr graph,
                         Node parent, short axis) :
                    MultiNode("MaxAndArgMax", graph, parent, 2),
                    axis(axis) {
                if (parent->dtype == dType::b8) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent can not be of type b8");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            Shape get_shape(unsigned short index) const{
                Shape shape = parent->shape;
                shape[axis] = SymInt::one;
                return shape;
            }

            dType get_dtype(unsigned short index) const{
                if(index == 0){
                    return parent->dtype;
                } else {
                    return graph->max_int;
                }
            }

            nodeType get_node_type(unsigned short index) const{
                if(parent->node_type == INPUT or parent->node_type == INPUT_DERIVED){
                    if(index == 0){
                        return INPUT_DERIVED;
                    } else {
                        return CONSTANT_DERIVED;
                    }
                } else {
                    return CONSTANT_DERIVED;
                }
            }

            Node child_to_my_grad(Node my_grad, unsigned short index){
                // The max should always be the first child
                if(index == 0){
                    Node argmax;
                    if(parent->children[0]->id != owner->id){
                        argmax = parent->children[0];
                    } else {
                        argmax = parent->children[1];
                    }
//                    return graph->derived_node(std::make_shared<IndexGrad>(graph,
//                                                                           my_grad,
//                                                                           owner.argmax(axis), axis,
//                                                                           owner->shape[axis]));
                    return graph->constant_value(22.0);
                } else {
                    auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MaxAndArgMax>(graph, ancestors[0], axis);
            }
        };

        /**
         * Sort and argsort operator
         */
        class SortAndArgSort : public MultiNode {
        public:
            short axis;
            SortAndArgSort(GraphInPtr graph,
                         Node parent, short axis) :
                    MultiNode("SortAndArgSort", graph, parent, 2),
                    axis(axis) {
                if (parent->dtype == dType::b8) {
                    auto err = InvalidArguments(NodeVec{parent}, name, "Parent can not be of type b8");
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            Shape get_shape(unsigned short index) const{
                Shape shape = parent->shape;
                shape[axis] = SymInt::one;
                return shape;
            }

            dType get_dtype(unsigned short index) const{
                if(index == 0){
                    return parent->dtype;
                } else {
                    return graph->max_int;
                }
            }

            nodeType get_node_type(unsigned short index) const{
                if(parent->node_type == INPUT or parent->node_type == INPUT_DERIVED){
                    if(index == 0){
                        return INPUT_DERIVED;
                    } else {
                        return CONSTANT_DERIVED;
                    }
                } else {
                    return CONSTANT_DERIVED;
                }
            }

            Node child_to_my_grad(Node my_grad, unsigned short index){
                // The max should always be the first child
                if(index == 0){
                    Node argsort;
                    if(parent->children[0]->id != owner->id){
                        argsort = parent->children[0];
                    } else {
                        argsort = parent->children[1];
                    }
//                    return graph->derived_node(std::make_shared<IndexGrad>(graph,
//                                                                           my_grad,
//                                                                           owner.argmax(axis), axis,
//                                                                           owner->shape[axis]));
                    return graph->constant_value(22.0);
                } else {
                    auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                    logger()->error() << err.msg;
                    throw err;
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MaxAndArgMax>(graph, ancestors[0], axis);
            }
        };
    }

    namespace core{
        Node Node::max(short axis) {
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node max_and_arg_max = graph->derived_node(std::make_shared<op::MaxAndArgMax>(graph, this, axis));
            return graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, max_and_arg_max, 0));
        }

        Node Node::argMax(short axis) {
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node max_and_arg_max = graph->derived_node(std::make_shared<op::MaxAndArgMax>(graph, this, axis));
            return graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, max_and_arg_max, 1));
        }

        std::pair<Node, Node> Node::maxAndArgMax(short axis){
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node max_and_arg_max = graph->derived_node(std::make_shared<op::MaxAndArgMax>(graph, this, axis));
            return {graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, max_and_arg_max, 0)),
                    graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, max_and_arg_max, 1))};
        }

        Node Node::sort(short axis) {
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node sort_and_arg_sort = graph->derived_node(std::make_shared<op::SortAndArgSort>(graph, this, axis));
            return graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, sort_and_arg_sort, 0));
        }

        Node Node::argSort(short axis) {
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node sort_and_arg_sort = graph->derived_node(std::make_shared<op::SortAndArgSort>(graph, this, axis));
            return graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, sort_and_arg_sort, 1));
        }

        std::pair<Node, Node> Node::sortAndArgSort(short axis){
            GraphInPtr graph = unwrap()->graph;
            if (axis == AUTO_INFER_AXIS) {
                for (size_t i = 0; i < 4; i++) {
                    if (unwrap()->shape[3 - i] != 1) {
                        axis = 3 - i;
                        break;
                    }
                }
            }
            Node sort_and_arg_sort = graph->derived_node(std::make_shared<op::SortAndArgSort>(graph, this, axis));
            return {graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, sort_and_arg_sort, 0)),
                    graph->derived_node(std::make_shared<op::MultiNodeIndex>(graph, sort_and_arg_sort, 1))};
        }
    }
}
#endif //METADIFF_OPERATORS_MULTY_NODE_H
