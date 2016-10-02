//
// Created by alex on 01/10/16.
//

#ifndef METADIFF_CORE_GRAPH_H
#define METADIFF_CORE_GRAPH_H

namespace md{
    namespace core{

        /**
         * The internal computation graph class
         * TODO: Should think what to be made private
         * TODO: Should add an ordering to the computation, so that it does not
         * necessarily follows the order of creation of the variables.
         */
        class GraphInternal : public std::enable_shared_from_this<GraphInternal> {
        private:
            std::shared_ptr<spdlog::logger> logger() const {
                return md::utils::logger("graph::" + name);
            }
        public:
            /** The name of the graph */
            std::string name;
            /** The default device to use for the graph */
            Device default_device;
            /** The maximum floating point precision to allow (See #dType) */
            dataType max_float;
            /** The maximum integer precision to allow (See #dType) */
            dataType max_int;
            /** Error policy for implicit broadcasts */
            errorPolicy broadcast_err_policy;
            /** Error policy for type promotions */
            errorPolicy type_promotion_err_policy;
            /** Error policy for implicit cast */
            errorPolicy cast_err_policy;
            /** TODO maybe I don't need this anymore integer count */
            size_t sym_integer_count;
            /** Current gradient level */
            unsigned short grad_level;
            /** The promotion table */
            dataType promotion_table[13][13];

            std::vector<std::shared_ptr<NodeInternal>> nodes;
            Updates updates;

            std::vector<std::shared_ptr<NodeGroup>> groups;
            Group current_group;

            NodeVec temporary_constants;
            Updates temporary_updates;

            GraphInternal():
                    name("Function"),
                    default_device(host()),
                    max_float(f32),
                    max_int(i32),
                    broadcast_err_policy(WARN),
                    type_promotion_err_policy(WARN),
                    cast_err_policy(WARN),
                    sym_integer_count(0),
                    grad_level(0){
                for(auto i=0; i<13; ++i){
                    for(auto j=0; j<13; ++j){
                        promotion_table[i][j] = default_promotion_table[i][j];
                    }
                }
                groups.push_back(std::make_shared<NodeGroup>());
            }

            /** Checks if the corresponding NodeInternal is in #temporary_constants. */
            bool is_temporary_constant(Node node) const;

            /** Copies the computations with value `true` in the mask to the new_graph */
            NodeVec copy(GraphInPtr new_graph, std::vector<bool> mask) const;

            /** Returns an array masking all descendants of the marked nodes */
            std::vector<bool> get_descendants_mask(NodeVec marked) const;

            /** Returns an array masking all ancestors of the marked nodes */
            std::vector<bool> get_ancestors_mask(NodeVec marked) const;

            /**
             * Finds a node which performs the same operation
             * TODO Not implemented correctly
             */
            Node find_same_node(std::shared_ptr<Operator> op);

            /** Adds the updates to the temporary updates of the graph */
            void add_temporary_updates(Updates const &updates);

            /** Removes all temporary updates of the graph */
            void clear_temporary_updates();

            /** Returns the gradients of the objective with respect to the parameters provided */
            NodeVec gradient(Node objective, NodeVec params);

            /** Optimizes a graph with respect to the given nodes (INTERNAL) */
            Graph optimize(NodeVec &targets, Updates &updates, NodeVec &inputs,
                           NodeVec &new_targets, Updates &new_updates, NodeVec &new_inputs);

            /** Creates a new derived node (INTERNAL) */
            Node derived_node(std::shared_ptr<Operator> op);

            /** Adds an update for the shared node */
            void update_node(Node shared, Node update);

            /** Returns the next unused symbolic integer */
            SymInt get_new_symbolic_integer();

            /** Returns the group specified by full_name. If it does not exist creates it. */
            Group get_group(std::string full_name);

            /** Sets the current group to the specified by the name. If it does not exists creates it */
            void set_group(std::string full_name);

            /**
             * Sets the current group to the group specified by base_name and its parent */
            void set_group(std::string base_name, Group parent);

            /** Sets the current group to #GROUP_ROOT */
            void reset_group();

            /** Returns a Node representing 'pi', with the maximum allowed floating point precision */
            Node PI();

            /** Returns a Node representing 'e', with the maximum allowed floating point precision */
            Node E();

            /** Returns a Node representing ln(2), with the maximum allowed floating point precision */
            Node LN_2();

            /** Returns a Node representing ln(10), with the maximum allowed floating point precision */
            Node LN_10();

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType v_type,
                         std::array<SymInt, 4> shape,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType v_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         SymInt shape3,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType v_type,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable, with the same specs as the one provided */
            Node tensor4_as(Node node,
                            std::string name = "InputTensor");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType v_type,
                         std::array<SymInt, 3> shape,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType v_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType v_type,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable, with the same specs as the one provided */
            Node tensor3_as(Node node,
                            std::string name = "InputTensor3");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType v_type,
                        std::array<SymInt, 2> shape,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType v_type,
                        SymInt shape0,
                        SymInt shape1,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType v_type,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix, with the same specs as the one provided */
            Node matrix_as(Node node,
                           std::string name = "InputMatrix");

            /** Creates a square #INPUT matrix  */
            Node square_matrix(dataType v_type,
                               SymInt shape,
                               std::string name = "InputMatrix");

            /** Creates an #INPUT vector  */
            Node vector(dataType v_type,
                        SymInt shape,
                        std::string name = "InputVector");

            /** Creates an #INPUT vector  */
            Node vector(dataType dtype,
                        std::string name = "InputVector");

            /** Creates an #INPUT vector, with the same specs as the one provided */
            Node vector_as(Node node,
                           std::string name = "InputVector");

            /** Creates an #INPUT scalar */
            Node scalar(dataType dtype,
                        std::string name = "InputScalar");

            /** Returns an identity matrix of the given dimension size */
            Node eye(SymInt size, dataType type);

            /** Returns an identity matrix of the given dimension size */
            Node eye(SymInt size);

            /** Returns a matrix filled with ones with the given shape */
            Node ones(Shape shape, dataType type);

            /** Returns a matrix filled with ones with the given shape */
            Node ones(Shape shape);

            /** Returns a matrix filled with zeros with the given shape */
            Node zeros(Shape shape, dataType type);

            /** Returns a matrix filled with zeros with the given shape */
            Node zeros(Shape shape);

            /** Returns a Node wrapper around a shared variable */
            Node shared_variable(SharedVar var, std::string name = "SharedVar");

            /** Returns a Node wrapper around the value. */
            Node constant_value(bool value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the short value. */
            Node constant_value(unsigned short value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the int value. */
            Node constant_value(unsigned int value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the long value. */
            Node constant_value(unsigned long value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the short value. */
            Node constant_value(short value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the int value. */
            Node constant_value(int value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the long value. */
            Node constant_value(long value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the float value. */
            Node constant_value(float value, Shape shape = {1, 1, 1, 1});

            /** Returns a Node wrapper around the double value. */
            Node constant_value(double value, Shape shape = {1, 1, 1, 1});

            Node wrap(Node value){
                return value;
            }

            Node wrap(SharedVar value){
                return shared_variable(value);
            }

            Node wrap(SymInt value);

            Node wrap(bool value){
                return constant_value(value);
            }

            Node wrap(unsigned short value){
                return constant_value(value);
            }

            Node wrap(unsigned int value){
                return constant_value(value);
            }

            Node wrap(unsigned long value){
                return constant_value(value);
            }

            Node wrap(short value){
                return constant_value(value);
            }

            Node wrap(int value){
                return constant_value(value);
            }

            Node wrap(long value){
                return constant_value(value);
            }

            Node wrap(float value){
                return constant_value(value);
            }

            Node wrap(double value){
                return constant_value(value);
            }

            /** Returns a vector representing the sequence from start to end. */
            Node seq(SymInt start, SymInt end, dataType dtype);

            /** Returns a vector representing the sequence from start to end. */
            Node seq(SymInt start, SymInt end);
        };
    }
}
#endif //METADIFF_CORE_GRAPH_H
