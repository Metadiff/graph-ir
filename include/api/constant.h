//
// Created by alex on 27/10/16.
//

#ifndef GRAPH_IR_API_CONSTANT_H
#define GRAPH_IR_API_CONSTANT_H

namespace md{
    namespace api{
//        /** @brief Creates a variable wrapping a SharedVar
//         *
//         * @param var
//         * @param g
//         * @return
//         */
//        Node shared_var(SharedVar var, Graph g = default_graph);
//
//        /** Creates a variable wrapping a SymInt
//         *
//         * @param value
//         * @param g
//         * @return
//         */
//        Node sym_int_node(SymInt value, Graph g = default_graph);
//
//        /** @brief Returns a variable representing 'pi', with the maximum allowed floating point precision
//         *
//         * @param g
//         * @return
//         */
//        Node PI(Graph g = default_graph);
//
//        /** @brief Returns a variable representing 'e', with the maximum allowed floating point precision
//         *
//         * @param g
//         * @return
//         */
//        Node E(Graph g = default_graph);
//
//        /** @brief Returns a variable representing 'ln(2)', with the maximum allowed floating point precision
//         *
//         * @param g
//         * @return
//         */
//        Node LN_2(Graph g = default_graph);
//
//        /** @brief Returns a variable representing 'ln(10)', with the maximum allowed floating point precision
//         *
//         * @param g
//         * @return
//         */
//        Node LN_10(Graph g = default_graph);
//
//        /** @brief Returns a variable filled with zeros with the provied Shape and DataType
//         *
//         * @param g
//         * @return
//         */
//        Node zeros(Shape shape, DataType data_type, Graph g = default_graph);
//
//        /** @brief Returns a variable filled with zeros with the provied Shape
//         *
//         * @param g
//         * @return
//         */
//        Node zeros(Shape shape, Graph g = default_graph);
//
//        /** @brief Returns a variable filled with ones with the provied Shape and DataType
//         *
//         * @param shape
//         * @param data_type
//         * @param g
//         * @return
//         */
//        Node ones(Shape shape, DataType data_type, Graph g = default_graph);
//
//        /** @brief Returns a variable filled with ones with the provied Shape
//         *
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node ones(Shape shape, Graph g = default_graph);
//
//        /** @brief Returns a vector with the sequence of [start, end-1] with the provided DataType
//         *
//         * @param start
//         * @param end
//         * @param data_type
//         * @param g
//         * @return
//         */
//        Node range(SymInt start, SymInt end, DataType data_type, Graph g = default_graph);
//
//        /** @brief Returns a vector with the sequence of [start, end-1] with the maximum integer precision allowed
//         *
//         * @param start
//         * @param end
//         * @param g
//         * @return
//         */
//        Node range(SymInt start, SymInt end, Graph g = default_graph);
//
//        /** @brief Returns an identity matrix with the provied DataType and size
//         *
//         * @param size
//         * @param data_type
//         * @param g
//         * @return
//         */
//        Node eye(SymInt size, DataType data_type, Graph g = default_graph);
//
//        /** @brief Returns an identity matrix with the provied size
//         *
//         * @param size
//         * @param g
//         * @return
//         */
//        Node eye(SymInt size, Graph g = default_graph);

//        /** @brief  Creates a constant variable which is filled with the provied value,
//         * and has the DataType and Shape provided.
//         *
//         * @param value
//         * @param data_type
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(double value, DataType data_type, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a b8 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(bool value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a u8 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(uint8_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a u16 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(uint16_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a u32 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(uint32_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a u64 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(uint64_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a i8 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(int8_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a i16 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(int16_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief @brief Creates a i32 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(int32_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief @brief Creates a i64 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(int64_t value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief Creates a f32 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(float value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        /** @brief @brief Creates a f64 constant variable with the specified shape and value
//         *
//         * @param value
//         * @param shape
//         * @param g
//         * @return
//         */
//        Node constant(double value, Shape shape = {1, 1, 1, 1}, Graph g = default_graph);
//
//        // Used for wrapping any outside variables in a unified way
//        inline Node wrap(SharedVar value, Graph g = default_graph){
//            return shared_var(value, g);
//        }
//
//        // Used for wrapping any outside variables in a unified way
//        inline Node wrap(SymInt value, Graph g = default_graph){
//            return sym_int_node(value, g);
//        }
//
//        // Used for wrapping any outside variables in a unified way
//        template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value>>
//        Node wrap(T value, Graph g = default_graph){
//            return constant(value, {1, 1, 1, 1}, g);
//        }
    }
}

#endif //GRAPH_IR_API_CONSTANT_H
