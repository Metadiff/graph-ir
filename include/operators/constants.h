//
// Created by alex on 17/12/15.
//

#ifndef METADIFF_OPERATORS_CONSTANTS_H
#define METADIFF_OPERATORS_CONSTANTS_H

namespace metadiff{
    namespace op {
        using namespace core;
        using namespace exceptions;
#ifdef AFAPI
        /** Operator for constant input variables */
        class ConstantInput : public ConstantOperator {
        public:
            af::array value;
            ConstantInput(GraphInPtr graph,
                          af::array value) :
                    ConstantOperator("ConstInput", graph ,
                                     Shape{value.dims(0), value.dims(1),
                                           value.dims(2), value.dims(3)},
                                     shared::ArrayFireVariable::convert_af_dtype(value.type())) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<ConstantInput>(graph, value);
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                return false;
            }
        };
#endif

        /** Tensor filled with the same value */
        class ConstantValue : public ConstantOperator {
        public:
            double value;

            ConstantValue(GraphInPtr graph, double value, Shape shape, dType dtype) :
                    ConstantOperator("ConstValue", graph, shape, dtype),
                    value(value) { };


            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<ConstantValue>(graph, value, shape, dtype);
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (ConstantOperator::equals(op)) {
                    auto cast_op = std::static_pointer_cast<const ConstantValue>(op);
                    return value == cast_op->value;
                } else {
                    return false;
                }
            }
        };

        /** Matrix identity */
        class Eye : public ConstantOperator {
        public:
            Eye(GraphInPtr graph, SymInt size, dType dtype) :
                    ConstantOperator("Eye", graph, Shape{size, size, SymInt::one, SymInt::one}, dtype) {}

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Eye>(graph, shape[0], dtype);
            }
        };


        /** A vector of the sequence from 'start' to 'end' */
        class Sequence : public ConstantOperator {
        public:
            SymInt start;
            SymInt end;

            Sequence(GraphInPtr graph, SymInt start, SymInt end, dType dtype) :
                    ConstantOperator("Sequence", graph, Shape {end - start, SymInt::one, SymInt::one, SymInt::one}, dtype),
                    start(start), end(end) {}

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Sequence>(graph, start, end, dtype);
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (ConstantOperator::equals(op)) {
                    auto cast_op = std::static_pointer_cast<const Sequence>(op);
                    return start == cast_op->start and end == cast_op->end;
                } else {
                    return false;
                }
            }
        };

        /** The operator provides a view of the parent which is constant.
         * This implies that the gradient with respect to the result is always 0. */
        class MakeConstant : public UnaryOperator {
        public:
            MakeConstant(GraphInPtr graph,
                         Node parent) :
                    UnaryOperator("MakeConst", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<MakeConstant>(graph, ancestors[0]);
            }

            nodeType get_node_type() const {
                return CONSTANT_DERIVED;
            };

            Node get_parent_grad(Node my_grad, unsigned short index) {
                auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                logger()->error() << err.msg;
                throw err;
            }
        };
    };

    namespace core {
#ifdef AFAPI
        Node GraphInternal::constant_value(af::array value) {
            std::shared_ptr<Operator> op = std::make_shared<op::ConstantInput>(this, value);
            return derived_node(op);
        }
#endif
        Node GraphInternal::constant_value(bool value, Shape shape) {
            std::shared_ptr<Operator> op = std::make_shared<op::ConstantValue>(this, value, shape, b8);
            return derived_node(op);
        }

        Node GraphInternal::constant_value(unsigned short value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, u8); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, u16); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(unsigned int value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, u8); break;
                case i16: op = std::make_shared<op::ConstantValue>(this, value, shape, u16); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, u32); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(unsigned long value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, u8); break;
                case i16: op = std::make_shared<op::ConstantValue>(this, value, shape, u16); break;
                case i32: op = std::make_shared<op::ConstantValue>(this, value, shape, u32); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, u64); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(short value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, i8); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, i16); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(int value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, i8); break;
                case i16: op = std::make_shared<op::ConstantValue>(this, value, shape, i16); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, i32); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(long value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_int){
                case i8: op = std::make_shared<op::ConstantValue>(this, value, shape, i8); break;
                case i16: op = std::make_shared<op::ConstantValue>(this, value, shape, i16); break;
                case i32: op = std::make_shared<op::ConstantValue>(this, value, shape, i32); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, i64); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(float value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_float){
                case f8: op = std::make_shared<op::ConstantValue>(this, value, shape, f8); break;
                case f16: op = std::make_shared<op::ConstantValue>(this, value, shape, f16); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, f32); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::constant_value(double value, Shape shape) {
            std::shared_ptr<Operator> op;
            switch (max_float){
                case f8: op = std::make_shared<op::ConstantValue>(this, value, shape, f8); break;
                case f16: op = std::make_shared<op::ConstantValue>(this, value, shape, f16); break;
                case f32: op = std::make_shared<op::ConstantValue>(this, value, shape, f32); break;
                default: op = std::make_shared<op::ConstantValue>(this, value, shape, f64); break;
            }
            return derived_node(op);
        }

        Node GraphInternal::zeros(Shape shape, dType type) {
            return derived_node(std::make_shared<op::ConstantValue>(this, 0.0, shape, type));
        }

        Node GraphInternal::zeros(Shape shape) {
            return derived_node(std::make_shared<op::ConstantValue>(this, 0.0, shape, max_float));
        }

        Node GraphInternal::ones(Shape shape, dType type) {
            return derived_node(std::make_shared<op::ConstantValue>(this, 1.0, shape, type));
        }

        Node GraphInternal::ones(Shape shape) {
            return derived_node(std::make_shared<op::ConstantValue>(this, 1.0, shape, max_float));
        }

        Node GraphInternal::eye(SymInt size, dType type) {
            return derived_node(std::make_shared<op::Eye>(this, size, type));
        }

        Node GraphInternal::eye(SymInt size) {
            return derived_node(std::make_shared<op::Eye>(this, size, max_float));
        }

        Node GraphInternal::seq(SymInt start, SymInt end, dType type) {
            return derived_node(std::make_shared<op::Sequence>(this, start, end, type));
        }

        Node GraphInternal::seq(SymInt start, SymInt end) {
            return derived_node(std::make_shared<op::Sequence>(this, start, end, max_int));
        }

        Node Node::as_constant() {
            return apply<op::MakeConstant>(this);
        }
    }
}

#endif //METADIFF_OPERATORS_CONSTANTS_H
