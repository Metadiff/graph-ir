//
// Created by alex on 01/10/16.
//

#ifndef METADIFF_GRAPH_IR_GRAPH_H
#define METADIFF_GRAPH_IR_GRAPH_H

#include "math.h"

namespace md{
    namespace gir{
        /** A small wrapper class wrapping a Graph, but also having a specified inputs and their ordering,
         * as well as outputs and their ordering.
         */
        class GraphFunction{
        public:
            Graph graph;
            std::vector<Node> inputs;
            std::vector<Node> outputs;

            GraphFunction(Graph const full_graph,
                          std::vector<Node> inputs,
                          std::vector<Node> outputs,
                          Updates extra_updates = Updates(),
                          bool copy_updates = true);

            /** @brief Applies the function to another graph
             *
             * @param other_graph
             * @param provided_inputs
             * @param apply_updates
             * @return
             */
            std::vector<Node> apply(Graph other_graph, std::vector<Node> provided_inputs,
                                    bool apply_updates = true);
        };

        /**
         * The internal computation graph class
         * TODO: Should think what to be made private
         * TODO: Should add an ordering to the computation, so that it does not  necessarily follows the order of creation of the variables.
         * TODO: Should add both group_map and operator_map
         */
        class GraphInternal : public std::enable_shared_from_this<GraphInternal> {
        public:
            /** Current gradient level */
            unsigned int grad_level = 0;
            /** The list of all of the nodes */
            std::vector<std::shared_ptr<NodeData>> nodes;
            /** List of all of the updates */
            Updates updates;
            /** Current group */
            std::string current_group;
            /** Mapping group name to all Nodes in that group */
            std::unordered_map<std::string, NodeVec> group_map;
            /** Mapping operator name to all Nodes who are results of that op */
            std::unordered_map<std::string, NodeVec> op_map;
        public:
            /** The name of the graph */
            std::string name;
            /** Properties the user can assign of the graph */
            Properties props;

            GraphInternal(std::string name = "graph"):
                    name(name),
                    props(default_properties()){
                current_group = "";
            }

            /** @brief Copies the computation of this graph into another
             *
             * @param new_graph
             * @param mask
             * @return
             */
            Updates copy_into(Graph new_graph, std::vector<bool> const & mask,
                              Updates const & provided,
                              bool allow_input_copies, bool allow_shared_copies,
                              bool copy_updates = true) const;

            /** @brief clones this graph
             *
             * @param mask
             * @param copy_updates
             * @return
             */
            Graph clone( std::vector<bool> const & mask, bool copy_updates = true) const;

            /** @brief clones this graph
             *
             * @param copy_updates
             * @return
             */
            Graph clone(bool copy_updates = true) const;

            /** @brief Aplies the computation of this graph to another
             *
             * @param new_graph
             * @param mask
             * @param mapped
             * @param copy_updates
             * @return
             */
            Updates apply(Graph new_graph, std::vector<bool> const & mask,
                          Updates const & provided,
                          bool copy_updates = true, bool allow_shared_copies = true) const;


            /** @brief Returns a boolean mask over the nodes of the graph, specifiying which nodes are descendants of roots
             *  Includes the roots in the mask as well
             *
             * @param roots
             * @return
             */
            std::vector<bool> get_descendants_mask(NodeVec const & roots) const;

            /** @brief Returns a boolean mask over the nodes of the graph, specifiying which nodes are ancestors of leafs
             *  Includes the leafs in the mask as well
             *
             * @param leafs
             * @return
             */
            std::vector<bool> get_ancestors_mask(NodeVec const &  leafs) const;

            /** @brief Returns the intersection of get_ancestors_mask() and get_descendants_mask()
             *
             * @param roots
             * @param leafs
             * @return
             */
            std::vector<bool> get_flow_tree_mask(NodeVec const & roots, NodeVec const & leafs) const;

            /** @brief Performs a backward differentiation of f with respect to w at evaluation poins u
             * Formally this computes u^T J_f, where J_f is the Jacobian of f with respect to w (Theano's Lop)
             *
             * @param f
             * @param u
             * @param w
             * @return
             */
            NodeVec backward_diff(NodeVec const & f, NodeVec const & u, NodeVec const & w);

            /** @brief Performs a forward differentiation of f with respect to w at evaluation poins v
             * Formally this computes J_f v, where J_f is the Jacobian of f with respect to w (Theano's Rop)
             *
             * @param f
             * @param v
             * @param w
             * @return
             */
            NodeVec forward_diff(NodeVec const & f, NodeVec const & v, NodeVec const & w);

            /** @brief Returns the name of the parent group
             *
             * @param full_name
             * @return
             */
            std::string get_parent_group_name(std::string full_name);

            /** @brief Sets the current group
             *
             * @param full_name
             */
            void set_group(std::string full_name);

            /** @brief Adds the group as a child to the current group and changes the current to it
             *
             * @param name
             */
            void push_group(std::string name);

            /** @brief Sets the current group to the parent of the current group
             *
             */
            void pop_group();

            /** @brief Sets the current group to the null group
             *
             */
            void reset_group();

            /** @brief Creates a new dervied node from the Operator. This is strictly for internal usage.
             *
             * @param op
             * @param name
             * @return
             */
            Node derived_node(Operator op, std::string name = "Derived");

            /** @brief Finds a Node which exists and is symbolically equivalent to the result of the Operator.
             * If such node does not exist returns empty node.
             *
             * @param op
             * @return
             */
            Node find_same_node(Operator op);

            /** @brief Looks up the max_float and max_int and limits the data_type accordingly
             *
             * @param data_type
             * @return
             */
            DataType limit_type(DataType data_type) const;

            /** @brief Creates a four dimensional variable in the default_graph
             *
             * @param data_type
             * @param shape
             * @param name
             * @return
             */
            Node tensor4(DataType data_type,
                         std::array<SymInt, 4> shape,
                         std::string name = "Tensor4");

            /** @brief Creates a four dimensional variable in the default_graph
             *
             * @param data_type
             * @param shape0
             * @param shape1
             * @param shape2
             * @param shape3
             * @param name
             * @return
             */
            Node tensor4(DataType data_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         SymInt shape3,
                         std::string name = "Tensor4");

            /** @brief Creates a four dimensional variable in default_graph
             *
             * @param data_type
             * @param name
             * @return
             */
            Node tensor4(DataType data_type,
                         std::string name = "Tensor4");

            /** @brief Creates a three dimensional variable in the default_graph
             *
             * @param data_type
             * @param shape
             * @param name
             * @return
             */
            Node tensor3(DataType data_type,
                         std::array<SymInt, 3> shape,
                         std::string name = "Tensor3");

            /** @brief Creates a three dimensional variable in the default_graph
             *
             * @param data_type
             * @param shape0
             * @param shape1
             * @param shape2
             * @param name
             * @return
             */
            Node tensor3(DataType data_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         std::string name = "Tensor3");

            /** @brief Creates a three dimensional variable in the default_graph
             *
             * @param data_type
             * @param name
             * @return
             */
            Node tensor3(DataType data_type,
                         std::string name = "Tensor3");

            /** @brief Creates a matrix variable in the default_graph
             *
             * @param data_type
             * @param shape
             * @param name
             * @return
             */
            Node matrix(DataType data_type,
                        std::array<SymInt, 2> shape,
                        std::string name = "Matrix");

            /** @brief Creates a matrix variable in the default_graph
             *
             * @param data_type
             * @param shape0
             * @param shape1
             * @param name
             * @return
             */
            Node matrix(DataType data_type,
                        SymInt shape0,
                        SymInt shape1,
                        std::string name = "Matrix");

            /** @brief Creates a matrix variable in the default_graph
             *
             * @param data_type
             * @param name
             * @return
             */
            Node matrix(DataType data_type,
                        std::string name = "Matrix");

            /** @brief Creates a square matrix variable in the default_graph
             *
             * @param data_type
             * @param shape
             * @param name
             * @return
             */
            Node square_matrix(DataType data_type,
                               SymInt shape,
                               std::string name = "Matrix");

            /** @brief Creates a row vector variable in the default_graph
             *
             * @param data_type
             * @param shape
             * @param name
             * @return
             */
            Node vector(DataType data_type,
                        SymInt shape,
                        std::string name = "Vector");

            /** @brief Creates a row vector variable in the default_graph
             *
             * @param data_type
             * @param name
             * @return
             */
            Node vector(DataType data_type,
                        std::string name = "Vector");

            /** @brief Creates a scalar variable in the default_graph
             *
             * @param data_type
             * @param name
             * @return
             */
            Node scalar(DataType data_type,
                        std::string name = "Scalar");

            /** @brief Creates an input variable with the same attributes as the node
             * Note that this operation does not require the Node to be from this graph.
             *
             * @param node
             * @param name
             * @return
             */
            Node tensor_as(Node node, std::string name = "");

            /** @brief Creates a variable wrapping a SharedVar
             *
             * @param var
             * @param g
             * @return
             */
            Node shared_var(SharedVar var);

            /** Creates a variable wrapping a SymInt
             *
             * @param value
             * @param g
             * @return
             */
            Node sym_int_node(SymInt value);

            /** @brief  Creates a constant variable which is filled with the provied value,
             * and has the DataType and Shape provided.
             *
             * @param value
             * @param data_type
             * @param shape
             * @param g
             * @return
             */
            Node constant(double value, DataType data_type, Shape shape = {1, 1, 1, 1});

            /** @brief Returns a variable representing 'pi', with the maximum allowed floating point precision
             *
             * @param g
             * @return
             */
            Node PI();

            /** @brief Returns a variable representing 'e', with the maximum allowed floating point precision
             *
             * @param g
             * @return
             */
            Node E();

            /** @brief Returns a variable representing 'ln(2)', with the maximum allowed floating point precision
             *
             * @param g
             * @return
             */
            Node LN_2();

            /** @brief Returns a variable representing 'ln(10)', with the maximum allowed floating point precision
             *
             * @param g
             * @return
             */
            Node LN_10();

            /** @brief Returns a variable filled with zeros with the provied Shape and DataType
             *
             * @param g
             * @return
             */
            Node zeros(Shape shape, DataType data_type);

            /** @brief Returns a variable filled with zeros with the provied Shape
             *
             * @param g
             * @return
             */
            Node zeros(Shape shape);

            /** @brief Returns a variable filled with ones with the provied Shape and DataType
             *
             * @param shape
             * @param data_type
             * @param g
             * @return
             */
            Node ones(Shape shape, DataType data_type);

            /** @brief Returns a variable filled with ones with the provied Shape
             *
             * @param shape
             * @param g
             * @return
             */
            Node ones(Shape shape);

            /** @brief Returns a vector with the sequence of [start, end-1] with the provided DataType
             *
             * @param start
             * @param end
             * @param data_type
             * @param g
             * @return
             */
            Node range(SymInt start, SymInt end, DataType data_type);

            /** @brief Returns a vector with the sequence of [start, end-1] with the maximum integer precision allowed
             *
             * @param start
             * @param end
             * @param g
             * @return
             */
            Node range(SymInt start, SymInt end);

            /** @brief Returns an identity matrix with the provied DataType and size
             *
             * @param size
             * @param data_type
             * @param g
             * @return
             */
            Node eye(SymInt size, DataType data_type);

            /** @brief Returns an identity matrix with the provied size
             *
             * @param size
             * @param g
             * @return
             */
            Node eye(SymInt size);

            /** @brief Creates a b8 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(bool value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a u8 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(uint8_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a u16 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(uint16_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a u32 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(uint32_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a u64 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(uint64_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a i8 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(int8_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a i16 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(int16_t value, Shape shape = {1, 1, 1, 1});

            /** @brief @brief Creates a i32 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(int32_t value, Shape shape = {1, 1, 1, 1});

            /** @brief @brief Creates a i64 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(int64_t value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a f32 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(float value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a f64 constant variable with the specified shape and value
             *
             * @param value
             * @param shape
             * @param g
             * @return
             */
            Node constant(double value, Shape shape = {1, 1, 1, 1});

            /** @brief Creates a variable with the specified shape, where each element is drawn
             * from the uniform distribution [0, 1]
             *
             * @param shape
             * @return
             */
            Node random_uniform(Shape shape);

            /** @brief Creates a variable with the specified shape, where each element is drawn
             * from the standard normal distribution
             *
             * @param shape
             * @return
             */
            Node random_normal(Shape shape);
        };

        // Used for wrapping any outside variables in a unified way
        inline Node wrap(SharedVar value, Graph g){
            return g->shared_var(value);
        }

        // Used for wrapping any outside variables in a unified way
        inline Node wrap(SymInt value, Graph g){
            return g->sym_int_node(value);
        }

        // Used for wrapping any outside variables in a unified way
        template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value>>
        Node wrap(T value, Graph g) {
            return g->constant(value, {1, 1, 1, 1});
        }
    }
}
#endif //METADIFF_GRAPH_IR_GRAPH_H
