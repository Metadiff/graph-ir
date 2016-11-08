//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_GRAPH_IR_UTILS_OS_H
#define METADIFF_GRAPH_IR_UTILS_OS_H

namespace md{
    namespace gir{
        /** @brief Returns the sink used by Graph IR
         *
         * @return
         */
        inline LogSink gir_sink(){
            static LogSink gir_sink;
            if(not gir_sink){
                gir_sink = std::make_shared<spdlog::sinks::dist_sink_st>();
            }
            return gir_sink;
        }

        /** @brief Enables or disables the console logging of the gir_sink
         *
         * @param enable
         */
        inline void console_logging(bool enable){
            static std::shared_ptr<spdlog::sinks::stdout_sink_st> console_sink;
            static bool state = false;
            if(enable and not state){
                if(not console_sink){
                    console_sink = std::make_shared<spdlog::sinks::stdout_sink_st>();
                }
                gir_sink()->add_sink(console_sink);
            } else if(state and not enable) {
                gir_sink()->remove_sink(console_sink);
            }
        }

        /** @brief Returns a logger registered to the GIR sink
         *
         * @param name
         * @param level
         * @return
         */
       Logger logger(std::string const name, LogLevel const level = LogLevel::trace, std::string const prefix = "");

        /** @brief Returns a logger registered to the GIR sink, with a prefix graph::
         *
         * @param name
         * @param level
         * @return
         */
        Logger g_logger(std::string const name, LogLevel const level = LogLevel::trace);

        /** @brief Returns a logger registered to the GIR sink, with a prefix op::
         *
         * @param name
         * @param level
         * @return
         */
        Logger op_logger(std::string const name, LogLevel const level = LogLevel::trace);

        /** @brief Returns a logger registered to the GIR sink, with a prefix backend::
         *
         * @param name
         * @param level
         * @return
         */
        Logger backend_logger(std::string const name, LogLevel const level = LogLevel::trace);

        /** @brief Calculates the number total number of elements in a Shape
         *
         * @param shape
         * @return
         */
        SymInt number_of_elements(Shape const shape);

        /** @brief Returns the base Operator, skipping any Alias parents
         *
         * @param op
         * @return
         */
        Operator get_base_op(Operator const op);

        /** @brief Returns the base Node, skipping any Alias parents
         *
         * @param op
         * @return
         */
        Node get_base_node(Node const node);

        /** @brief Evaluates whether the two nodes are symbolically equals
         *
         * @param node1
         * @param node2
         * @return
         */
        bool symbolic_equals(Node const & node1, Node const & node2);

        // Helpers

        /** @brief Returns back the maximum Shape for broadcsating. Throws an error if some of the nodes have conflicting ones.
         *
         * @param nodes
         * @param op_name
         * @return
         */
        Shape get_max_shape(NodeVec const & nodes, std::string const op_name);

        /** @brief If the input Node matches the Shape does nothing. Otherwise broadcasts the Node to the shape
         * or throws an exception based on the Node's Graph policy for implicit_broadcast.
         *
         * @param node
         * @param shape
         * @param op_name
         * @return
         */
        Node implicit_broadcast(Node const node, Shape const shape, std::string const op_name);

        /** @brief Get's the maximum shape from all nodes (see get_max_shape()) and than broadcasts each node
         * to that shape or throws an exception (see implicit_broadcast())
         *
         * @param nodes
         * @param op_name
         * @return
         */
        void verify_shapes_and_broadcast(NodeVec & nodes, std::string const op_name);

        /** @brief If the input node matches the DataType does nothing. Otherwise casts the Node to the data_type
         * or throws an exception based on the Node's Graph policy form implicit_cast
         *
         * @param node
         * @param data_type
         * @param op_name
         * @return
         */
        Node implicit_cast(Node const node, DataType const data_type, std::string const op_name);

        /** @brief Sorts the Axes in ascending order and validates that they are coherent with the Shape.
         *
         * @param shape
         * @param axes
         * @param op_name
         * @return
         */
        void validate_axes(Shape const shape, Axes & axes, std::string const op_name);

        /** @brief Infers which axes should a reduction be applied to in order the result to be a scalar
         *
         * @param shape
         * @return
         */
        Axes auto_infer_axes(Shape const shape);

        /** @brief Returns whether the anchor is dependanble on the monitored value
         *
         * @param anchor
         * @param monitored
         * @return
         */
        bool is_dependent(Node const anchor, Node const monitored);
    }
}

#endif //METADIFF_GRAPH_IR_UTILS_OS_H
