//
// Created by alex on 17/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_CONSTANTS_H
#define METADIFF_CORE_OPERATORS_CONSTANTS_H
namespace md {
    namespace core {
        namespace op {
            /** The operator provides a view of the parent which is constant.
             * This implies that the gradient with respect to the result is always 0. */
            class ConstantValue : public ConstantOperator {
            public:
                Shape shape;
                double value;
                ConstantValue(GraphInPtr graph,
                              double value,
                              dataType data_type,
                              Shape shape) :
                        AbstractOperator("ConstValue", graph), ConstantOperator(data_type),
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

            /** Operator for wrapping Symbolic Integers */
            class SymIntWrapper : public ConstantOperator {
            public:
                SymInt value;

                SymIntWrapper(GraphInPtr graph, SymInt value) :
                        AbstractOperator("SymInt", graph), ConstantOperator(graph->max_int),
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

                Range(GraphInPtr graph, SymInt start, SymInt end, dataType data_type) :
                        AbstractOperator("Range", graph), ConstantOperator(data_type),
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
                Eye(GraphInPtr graph, SymInt size, dataType data_type) :
                        AbstractOperator("Eye", graph),  ConstantOperator(data_type),
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


//            /** Tensor filled with the same f64 value */
//            class ConstantFloat64Value : public ConstantOperator {
//            public:
//                double value;
//
//                ConstantFloat64Value(GraphInPtr graph,
//                                     double value,
//                                     Shape shape) :
//                        ConstantOperator("ConstFloat64Value", graph, shape, f64),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantFloat64Value>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same f32 value */
//            class ConstantFloat32Value : public ConstantOperator {
//            public:
//                float value;
//
//                ConstantFloat32Value(GraphInPtr graph,
//                                     float value,
//                                     Shape shape) :
//                        ConstantOperator("ConstFloat32Value", graph, shape, f32),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantFloat32Value>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same f16 value */
//            class ConstantFloat16Value : public ConstantOperator {
//            public:
//                float value;
//
//                ConstantFloat16Value(GraphInPtr graph,
//                                     float value,
//                                     Shape shape) :
//                        ConstantOperator("ConstFloat16Value", graph, shape, f16),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantFloat16Value>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same f8 value */
//            class ConstantFloat8Value : public ConstantOperator {
//            public:
//                float value;
//
//                ConstantFloat8Value(GraphInPtr graph,
//                                    float value,
//                                    Shape shape) :
//                        ConstantOperator("ConstFloat8Value", graph, shape, f8),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantFloat8Value>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same i64 value */
//            class ConstantInt64Value : public ConstantOperator {
//            public:
//                int64_t value;
//
//                ConstantInt64Value(GraphInPtr graph,
//                                   int64_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstInt64Value", graph, shape, i64),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantInt64Value>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same i32 value */
//            class ConstantInt32Value : public ConstantOperator {
//            public:
//                int32_t value;
//
//                ConstantInt32Value(GraphInPtr graph,
//                                   int32_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstInt32Value", graph, shape, i32),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same i16 value */
//            class ConstantInt16Value : public ConstantOperator {
//            public:
//                int16_t value;
//
//                ConstantInt16Value(GraphInPtr graph,
//                                   int16_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstInt16Value", graph, shape, i16),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same i8 value */
//            class ConstantInt8Value : public ConstantOperator {
//            public:
//                int8_t value;
//
//                ConstantInt8Value(GraphInPtr graph,
//                                   int8_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstInt8Value", graph, shape, i8),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same u64 value */
//            class ConstantUInt64Value : public ConstantOperator {
//            public:
//                uint64_t value;
//
//                ConstantUInt64Value(GraphInPtr graph,
//                                   uint64_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstUInt64Value", graph, shape, u64),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same u32 value */
//            class ConstantUInt32Value : public ConstantOperator {
//            public:
//                uint32_t value;
//
//                ConstantUInt32Value(GraphInPtr graph,
//                                   uint32_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstUInt32Value", graph, shape, u32),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same u16 value */
//            class ConstantUInt16Value : public ConstantOperator {
//            public:
//                uint16_t value;
//
//                ConstantUInt16Value(GraphInPtr graph,
//                                   uint16_t value,
//                                   Shape shape) :
//                        ConstantOperator("ConstUInt16Value", graph, shape, u16),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same i8 value */
//            class ConstantUInt8Value : public ConstantOperator {
//            public:
//                uint8_t value;
//
//                ConstantUInt8Value(GraphInPtr graph,
//                                  uint8_t value,
//                                  Shape shape) :
//                        ConstantOperator("ConstUInt8Value", graph, shape, u8),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
//
//            /** Tensor filled with the same b8 value */
//            class ConstantBoolValue : public ConstantOperator {
//            public:
//                bool value;
//
//                ConstantBoolValue(GraphInPtr graph,
//                                   bool value,
//                                   Shape shape) :
//                        ConstantOperator("ConstBoolValue", graph, shape, b8),
//                        value(value) {};
//
//                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                    return std::make_shared<ConstantValue>(graph, value, shape, data_type);
//                }
//
//                double get_value() const {
//                    return value;
//                }
//            };
        }
    }
}

#endif //METADIFF_CORE_OPERATORS_CONSTANTS_H
