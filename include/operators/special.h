//
// Created by alex on 11/10/16.
//

#ifndef METADIFF_CORE_OPERATORS_SPECIAL_H
#define METADIFF_CORE_OPERATORS_SPECIAL_H

namespace md{
    namespace core{
        namespace op{
            /** Casting to a specified dataType */
            class Cast : public UnaryElementwiseOperator {
            public:
                dataType data_type;

                Cast(GraphInPtr graph, Node parent, dataType data_type) :
                        AbstractOperator("Cast", graph), UnaryOperator(parent),
                        data_type(data_type) {};

                dataType get_data_type() const {
                    return data_type;
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cast>(graph, ancestors[0], data_type);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->cast(my_derivative, parent->data_type);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->cast(parent_derivatives[index], data_type);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Cast>(op);
//                        return symbolic_equals(parent, cast_op->parent) and data_type == cast_op->data_type;
//                    }
//                    return false;
//                }
            };

            /** An alias/view of another node. */
            class Alias : public MorphElementwiseOperator {
            public:
                Alias(GraphInPtr graph, Node parent) :
                        AbstractOperator("Alias", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Alias>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return parent_derivatives[index];
                }

//                bool equals(Operator const op) const {
//                    Operator my_op = get_base_op(parent->op);
//                    return my_op->equals(op) or op->equals(my_op);
//                    return false;
//                }
            };

            /** Broadcasts the node to the specified shape */
            class Broadcast : public MorphOperator {
            public:
                Shape to_shape;

                Broadcast(GraphInPtr graph,
                          Node parent,
                          Shape to_shape) :
                        AbstractOperator("Broadcast", graph), UnaryOperator(parent),
                        to_shape(to_shape) {
                    for (int i = 0; i < 4; i++) {
                        if (parent->shape[i] != 1 and parent->shape[i] != to_shape[i]) {
                            auto err = std::make_shared<InvalidArguments>
                                    (NodeVec{parent}, name, "Can not broadcast to shape " +
                                                            core::to_string(to_shape));
                            err->log(logger());
                            throw err;
                        }
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Broadcast>(graph, ancestors[0], to_shape);
                }

                Shape get_shape() const {
                    return to_shape;
                }

                dataType get_data_type() const {
                    return parent->data_type;
                }

                Axes get_broadcast_axes() const {
                    Axes axes;
                    auto p1_shape = this->parent->shape;
                    for (size_t i = 0; i < 4; i++) {
                        if (p1_shape[i] != to_shape[i]) {
                            axes.push_back(i);
                        }
                    }
                    return axes;
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->sum(my_derivative, get_broadcast_axes());
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->broadcast(parent_derivatives[index], to_shape);
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Broadcast>(op);
//                        return symbolic_equals(parent, cast_op->parent) and to_shape == cast_op->to_shape;
//                    }
//                    return false;
//                }
            };

            /** View of the node, which is non-differentiable */
            class MakeConstant : public MorphElementwiseOperator {
            public:
                MakeConstant(GraphInPtr graph,
                             Node parent) :
                        AbstractOperator("MakeConstant", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MakeConstant>(graph, ancestors[0]);
                }

                bool is_differentiable() const{
                    return false;
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_derivative}, name);
                    err->log(logger());
                    throw err;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return Node();
                }
            };

            /** Elementwise selects one of the two parents based on the condition */
            class Select : public BinaryElementwiseOperator {
            public:
                Node condition;

                Select(GraphInPtr graph,
                       Node condition,
                       Node trueParent,
                       Node falseParent) :
                        AbstractOperator("Select", graph), BinaryOperator(trueParent, falseParent),
                        condition(condition) {
                    if (condition->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{condition, trueParent, falseParent},
                                                                      name,
                                                                      "Calling Select with condition of type "
                                                                      + to_string(condition->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->condition = graph->cast(condition, b8);
                    }

                    Shape shape = verify_elementwise_shapes({condition, trueParent, falseParent}, logger());
                    if (condition.dims() == 0) {
                        this->condition = graph->broadcast(condition, shape);
                    }
                    if (trueParent.dims() == 0) {
                        this->parent1 = graph->broadcast(parent1, shape);
                    }
                    if (falseParent.dims() == 0) {
                        this->parent1 = graph->broadcast(parent2, shape);
                    }
                };

                dataType get_data_type() const {
                    return parent1->data_type;
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Select>(graph, ancestors[2], ancestors[0], ancestors[1]);
                }

                NodeVec get_arguments() const {
                    return NodeVec {condition};
                }

                Shape get_shape() const{
                    return parent1->shape;
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    Node zero = graph->constant(0);
                    if (index == 0) {
                        return graph->select(condition, my_derivative, zero);
                    } else {
                        return graph->select(condition, zero, my_derivative);
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    // We have to simulate this correctly
                    int c = (not parent_derivatives[0].ptr.expired()) +
                            (not parent_derivatives[0].ptr.expired());
                    if(c == 2){
                        if(index == 0){
                            return graph->select(condition, parent_derivatives[0], parent_derivatives[1]);
                        }
                    } else {
                        if(index == 0){
                            Node zero = graph->constant(0);
                            return graph->select(condition, parent_derivatives[0], zero);
                        } else {
                            Node zero = graph->constant(0);
                            return graph->select(condition, zero, parent_derivatives[1]);
                        }
                    }
                    return Node();
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_SPECIAL_H
