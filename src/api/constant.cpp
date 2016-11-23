//
// Created by alex on 06/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{
        Node GraphInternal::parameter(std::string scope, std::string name, DataType data_type, Shape shape){
            auto var_name = scope;
            var_name += props.scope_delimiter;
            var_name += name;
            Operator op = std::make_shared<op::Parameter>(this, var_name, data_type, shape);
            Node node = derived_node(op);
            node->name = var_name;
            return node;
        }

        Node GraphInternal::parameter(std::string name, DataType data_type, Shape shape){
            return parameter(scope, name, data_type, shape);
        }

        Node GraphInternal::sym_int_node(SymInt value) {
            Operator op = std::make_shared<op::SymIntWrapper>(this, value);
            return derived_node(op);
        }

        Node GraphInternal::constant(double value, DataType data_type, Shape shape) {
            // Limit the data type based on the max allowed
            DataType limit = limit_type(data_type);
            // Standard
            return derived_node(std::make_shared<op::ConstantValue>(this, value, limit, shape));
        }

        Node GraphInternal::PI() {
            return constant(M_PI, DataType(FLOAT, props.max_float), {1, 1, 1, 1});
        }
        Node GraphInternal::E(){
            return constant(M_E, DataType(FLOAT, props.max_float), {1, 1, 1, 1});
        }

        Node GraphInternal::LN_2(){
            return constant(M_LN2, DataType(FLOAT, props.max_float), {1, 1, 1, 1});
        }

        Node GraphInternal::LN_10(){
            return constant(M_LN10, DataType(FLOAT, props.max_float), {1, 1, 1, 1});
        }

        Node GraphInternal::zeros(Shape shape, DataType data_type){
            return constant(0.0, data_type, shape);
        }

        Node GraphInternal::zeros(Shape shape){
            return constant(0.0, DataType(FLOAT, props.max_float), shape);
        }

        Node GraphInternal::ones(Shape shape, DataType data_type){
            return constant(1.0, data_type, shape);
        }

        Node GraphInternal::ones(Shape shape){
            return constant(1.0, DataType(FLOAT, props.max_float), shape);
        }

        Node GraphInternal::range(SymInt start, SymInt end, DataType data_type){
            auto op = std::make_shared<op::Range>(this, start, end, data_type);
            return derived_node(op);
        }

        Node GraphInternal::range(SymInt start, SymInt end){
            return range(start, end, DataType(SIGNED_INT, props.max_int));
        }

        Node GraphInternal::eye(SymInt size, DataType data_type){
            auto op = std::make_shared<op::Eye>(this, size, data_type);
            return derived_node(op);
        }

        Node GraphInternal::eye(SymInt size){
            return eye(size, DataType(FLOAT, props.max_float));
        }

        Node GraphInternal::constant(bool value, Shape shape) {
            return constant(value, b8, shape);
        }

        Node GraphInternal::constant(uint8_t value, Shape shape) {
            return constant(value, u8, shape);
        }

        Node GraphInternal::constant(uint16_t value, Shape shape) {
            return constant(value, u16, shape);
        }

        Node GraphInternal::constant(uint32_t value, Shape shape) {
            return constant(value, u32, shape);
        }

        Node GraphInternal::constant(uint64_t value, Shape shape) {
            return constant(value, u64, shape);
        }

        Node GraphInternal::constant(int8_t value, Shape shape) {
            return constant(value, i8, shape);
        }

        Node GraphInternal::constant(int16_t value, Shape shape) {
            return constant(value, i16, shape);
        }

        Node GraphInternal::constant(int32_t value, Shape shape) {
            return constant(value, i32, shape);
        }

        Node GraphInternal::constant(int64_t value, Shape shape) {
            return constant(value, i64, shape);
        }

        Node GraphInternal::constant(float value, Shape shape) {
            return constant(value, f32, shape);
        }

        Node GraphInternal::constant(double value, Shape shape) {
            return constant(value, f64, shape);
        }
    }
//    namespace api {
//        Node constant(double value, DataType data_type, Shape shape) {
//            // Limit the data type based on the max allowed
//            DataType limit = limit_type(data_type);
//            // Standard
//            return derived_node(std::make_shared<op::ConstantValue>(g.get(), value, limit, shape));
//        }
//
//        Node shared_var(SharedVar var){
//            Operator op = std::make_shared<op::SharedInput>(g.get(), var);
//            Node node = derived_node(op);
//            node->name = var->name;
//            return node;
//        }
//
//        Node sym_int_node(SymInt value) {
//            Operator op = std::make_shared<op::SymIntWrapper>(g.get(), value);
//            return derived_node(op);
//        }
//
//        Node PI() {
//            return constant(M_PI, props.max_float, {1, 1, 1, 1});
//        }
//        Node E(){
//            return constant(M_E, props.max_float, {1, 1, 1, 1});
//        }
//
//        Node LN_2(){
//            return constant(M_LN2, props.max_float, {1, 1, 1, 1});
//        }
//
//        Node LN_10(){
//            return constant(M_LN10, props.max_float, {1, 1, 1, 1});
//        }
//
//        Node zeros(Shape shape, DataType data_type){
//            return constant(0.0, data_type, shape);
//        }
//
//        Node zeros(Shape shape){
//            return constant(0.0, props.max_float, shape);
//        }
//
//        Node ones(Shape shape, DataType data_type){
//            return constant(1.0, data_type, shape);
//        }
//
//        Node ones(Shape shape){
//            return constant(1.0, props.max_float, shape);
//        }
//
//        Node range(SymInt start, SymInt end, DataType data_type){
//            auto op = std::make_shared<op::Range>(g.get(), start, end, data_type);
//            return derived_node(op);
//        }
//
//        Node range(SymInt start, SymInt end){
//            return range(start, end, props.max_int);
//        }
//
//        Node eye(SymInt size, DataType data_type){
//            auto op = std::make_shared<op::Eye>(g.get(), size, data_type);
//            return derived_node(op);
//        }
//
//        Node eye(SymInt size){
//            return eye(size, props.max_float);
//        }
//
//        Node constant(bool value, Shape shape) {
//            return constant(value, b8, shape);
//        }
//
//        Node constant(uint8_t value, Shape shape) {
//            return constant(value, u8, shape);
//        }
//
//        Node constant(uint16_t value, Shape shape) {
//            return constant(value, u16, shape);
//        }
//
//        Node constant(uint32_t value, Shape shape) {
//            return constant(value, u32, shape);
//        }
//
//        Node constant(uint64_t value, Shape shape) {
//            return constant(value, u64, shape);
//        }
//
//        Node constant(int8_t value, Shape shape) {
//            return constant(value, i8, shape);
//        }
//
//        Node constant(int16_t value, Shape shape) {
//            return constant(value, i16, shape);
//        }
//
//        Node constant(int32_t value, Shape shape) {
//            return constant(value, i32, shape);
//        }
//
//        Node constant(int64_t value, Shape shape) {
//            return constant(value, i64, shape);
//        }
//
//        Node constant(float value, Shape shape) {
//            return constant(value, f32, shape);
//        }
//
//        Node constant(double value, Shape shape) {
//            return constant(value, f64, shape);
//        }
//    }
}