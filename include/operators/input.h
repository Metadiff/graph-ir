//
// Created by alex on 03/05/16.
//

#ifndef METADIFF_CORE_OPERATORS_INPUT_H
#define METADIFF_CORE_OPERATORS_INPUT_H

namespace md {
    namespace core {
        namespace op {
            /** Operator for input variables */
            class Input : public InputOperator {
            public:
                Shape shape;

                Input(GraphInPtr graph, dataType data_type, Shape shape) :
                        InputOperator("Input", graph, data_type),
                        shape(shape) {}

                Operator copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
                    return std::make_shared<Input>(graph, data_type, shape);
                }

                Shape get_shape() const {
                    return shape;
                }
            };

            /** Operator for shared input variables */
            class SharedInput : public InputOperator {
            public:
                SharedVar var;

                SharedInput(GraphInPtr graph, SharedVar var) :
                        InputOperator("Shared", graph, var->data_type),
                        var(var) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<SharedInput>(graph, var);
                }

                Shape get_shape() const {
                    return var->shape;
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const SharedInput>(op);
                        return var->id == cast_op->var->id;
                    }
                    return false;
                }
            };

            /** Operator for wrapping Symbolic Integers */
            class SymIntWrapper : public InputOperator {
            public:
                SymInt value;

                SymIntWrapper(GraphInPtr graph, SymInt value) :
                        InputOperator("SymInt", graph, graph->max_int),
                        value(value) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<SymIntWrapper>(graph, value);
                }

                Shape get_shape() const {
                    return Shape{1, 1, 1, 1};
                }

                nodeType get_node_type() const {
                    return value.is_constant() ? CONSTANT : CONSTANT_DERIVED;
                };

                bool equals(Operator const op) const {
                    if (this->name == op->name) {
                        auto cast_op = std::static_pointer_cast<const SymIntWrapper>(op);
                        return cast_op->value == value;
                    }
                    return false;
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_H
