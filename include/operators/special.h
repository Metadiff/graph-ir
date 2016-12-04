//
// Created by alex on 11/10/16.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_SPECIAL_H
#define METADIFF_GRAPH_IR_OPERATORS_SPECIAL_H

namespace md{
    namespace gir{
        namespace op{
            /** Indexing of MultiOutoputOperators */
            class MultiOutputIndex: public UnaryOperator{
            public:
                int index;
                MultiOutputIndex(GraphInPtr const graph, Node const parent, int index):
                        AbstractOperator(graph, "MultiOutputIndex"), UnaryOperator(parent),
                        index(index) {
                    auto parent_op = std::dynamic_pointer_cast<MultiOutputOperator>(parent->op);
                    if(not parent_op){
                        throw throw_op_ige(name, "Can not index operator " + parent->op->name);
                    }
                    if(index < 0 or index >= parent_op->outputs_number){
                        throw throw_op_iae(NodeVec{parent}, name,
                                           fmt::format("Index {} out of bounds for operator {}.", index, parent->op->name));
                    }
                }

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MultiOutputIndex>(graph, ancestors[0], index);
                }

                Shape get_shape() const {
                    auto parent_op = std::dynamic_pointer_cast<MultiOutputOperator>(parent->op);
                    return parent_op->get_shape_at(index);
                }

                DataType get_data_type() const {
                    auto parent_op = std::dynamic_pointer_cast<MultiOutputOperator>(parent->op);
                    return parent_op->get_data_type_at(index);
                }

                bool is_differentiable() const {
                    auto parent_op = std::dynamic_pointer_cast<MultiOutputOperator>(parent->op);
                    return parent_op->is_differentiable_at(index);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto parent_op = std::dynamic_pointer_cast<MultiOutputOperator>(parent->op);
                    return parent_op->forward_diff_parent_at(parent_derivatives, this->index);
                }
            };

            /** Casting to a specified DataType */
            class Cast : public UnaryElementwiseOperator {
            public:
                DataType data_type;

                Cast(GraphInPtr graph, Node parent, DataType data_type) :
                        AbstractOperator(graph, "Cast"), UnaryOperator(parent),
                        data_type(data_type) {};

                DataType get_data_type() const {
                    return data_type;
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cast>(graph, ancestors[0], data_type);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return cast(my_derivative, parent->data_type);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return cast(parent_derivatives[index], data_type);
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
                        AbstractOperator(graph, "Alias"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Alias>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
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
                        AbstractOperator(graph, "Broadcast"), UnaryOperator(parent),
                        to_shape(to_shape) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Broadcast>(graph, ancestors[0], to_shape);
                }

                Shape get_shape() const {
                    return to_shape;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    Axes axes;
                    auto parent_shape = this->parent->shape;
                    for (auto i = 0; i < 4; i++) {
                        if (parent_shape[i] != to_shape[i]) {
                            axes.push_back(i);
                        }
                    }
                    return sum(my_derivative, axes);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return broadcast(parent_derivatives[index], to_shape);
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
                        AbstractOperator(graph, "MakeConstant"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<MakeConstant>(graph, ancestors[0]);
                }

                bool is_differentiable() const{
                    return false;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    throw throw_op_ige(name, "Calling backward_diff unexpectedly.");
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
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
                        AbstractOperator(graph, "Select"), BinaryOperator(trueParent, falseParent),
                        condition(condition) {};

                DataType get_data_type() const {
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

                Node backward_diff_parent(Node my_derivative, int index){
                    Node zero = graph->constant(0);
                    if (index == 0) {
                        return select(condition, my_derivative, zero);
                    } else {
                        return select(condition, zero, my_derivative);
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    // We have to simulate this correctly
                    int c = (not parent_derivatives[0].ptr.expired()) +
                            (not parent_derivatives[0].ptr.expired());
                    if(c == 2){
                        if(index == 0){
                            return select(condition, parent_derivatives[0], parent_derivatives[1]);
                        }
                    } else {
                        if(index == 0){
                            Node zero = graph->constant(0);
                            return select(condition, parent_derivatives[0], zero);
                        } else {
                            Node zero = graph->constant(0);
                            return select(condition, zero, parent_derivatives[1]);
                        }
                    }
                    return Node();
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_SPECIAL_H
