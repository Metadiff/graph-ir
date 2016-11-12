//
// Created by alex on 17/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_CONSTANTS_H
#define METADIFF_GRAPH_IR_OPERATORS_CONSTANTS_H
namespace md {
    namespace gir {
        namespace op {

            /** Single value repeated over some Shape */
            class ConstantValue : public ConstantOperator {
            public:
                Shape shape;
                double value;
                ConstantValue(GraphInPtr graph,
                              double value,
                              DataType data_type,
                              Shape shape) :
                        AbstractOperator(graph, "ConstantValue"), ConstantOperator(data_type),
                        shape(shape), value(value) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<ConstantValue>(graph, value, data_type, shape);
                }

                Shape get_shape() const {
                    return shape;
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const ConstantValue>(op);
//                        return shape == cast_op->shape and data_type == cast_op->data_type and
//                               value == cast_op->value;
//                    }
//                    return false;
//                }
            };

            /** Wrapper arround a SymInt */
            class SymIntWrapper : public ConstantOperator {
            public:
                SymInt value;

                SymIntWrapper(GraphInPtr graph, SymInt value) :
                        AbstractOperator(graph, "SymInt"), ConstantOperator(DataType(UNSIGNED_INT, graph->props.max_int)),
                        value(value) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<SymIntWrapper>(graph, value);
                }

                Shape get_shape() const {
                    return Shape{1, 1, 1, 1};
                }

//                bool equals(Operator const op) const {
//                    if (this->name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const SymIntWrapper>(op);
//                        return cast_op->value == value;
//                    }
//                    return false;
//                }
            };

            /** A vector of the sequence from 'start' to 'end' */
            class Range : public ConstantOperator {
            public:
                SymInt start;
                SymInt end;

                Range(GraphInPtr graph, SymInt start, SymInt end, DataType data_type) :
                        AbstractOperator(graph, "Range"), ConstantOperator(data_type),
                        start(start), end(end) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Range>(graph, start, end, data_type);
                }

                Shape get_shape() const {
                    return {end - start, 1, 1, 1};
                }

//                bool equals(Operator const op) const {
//                    if (ConstantOperator::equals(op)) {
//                        auto cast_op = std::static_pointer_cast<const Range>(op);
//                        return start == cast_op->start and end == cast_op->end;
//                    } else {
//                        return false;
//                    }
//                }
            };

            /** Matrix identity */
            class Eye : public ConstantOperator {
            public:
                SymInt size;
                Eye(GraphInPtr graph, SymInt size, DataType data_type) :
                        AbstractOperator(graph, "Eye"),  ConstantOperator(data_type),
                        size(size){}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Eye>(graph, size, data_type);
                }

                Shape get_shape() const {
                    return {size, size, 1, 1};
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const Eye>(op);
//                        return size == cast_op->size;
//                    }
//                    return false;
//                }
            };
        }
    }
}

#endif //METADIFF_GRAPH_IR_OPERATORS_CONSTANTS_H
