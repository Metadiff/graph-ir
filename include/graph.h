//
// Created by alex on 01/10/16.
//

#ifndef METADIFF_CORE_GRAPH_H
#define METADIFF_CORE_GRAPH_H

#include "math.h"

namespace md{
    namespace core{

        /**
         * The internal computation graph class
         * TODO: Should think what to be made private
         * TODO: Should add an ordering to the computation, so that it does not
         * necessarily follows the order of creation of the variables.
         */
        class GraphInternal : public std::enable_shared_from_this<GraphInternal> {
        public:
            /** Current gradient level */
            unsigned short grad_level = 0;
            /** The list of all of the nodes */
            std::vector<std::shared_ptr<NodeData>> nodes;
            /** List of all of the updates */
            Updates updates;
            /** List of temporary updates */
            Updates temporary_updates;

            /** List of groups */
            std::vector<std::shared_ptr<NodeGroup>> base_groups;
            std::vector<std::shared_ptr<NodeGroup>> groups;
            /** Current group */
            Group current_group;

            std::shared_ptr<spdlog::logger> logger() const {
                return md::utils::logger("graph::" + name);
            }
        public:
            /** The name of the graph */
            std::string name;
            /** Properties the user can assign of the graph */
            Properties props;

            GraphInternal(std::string name = "Function"):
                    name(name),
                    props(default_properties()){
//                base_groups.push_back(std::make_shared<NodeGroup>(props.base_group_prefix + "0", this));
//                current_base_group = base_groups[0];
                groups.push_back(std::make_shared<NodeGroup>("", this));
                current_group = groups[0];
            }

            /** Copies the computations with value `true` in the mask to the new_graph */
            NodeVec copy(GraphInPtr new_graph, std::vector<bool> const & mask) const;

            /** Returns an array masking all descendants of the marked nodes */
            std::vector<bool> get_descendants_mask(NodeVec& roots) const;

            /** Returns an array masking all ancestors of the marked nodes */
            std::vector<bool> get_ancestors_mask(NodeVec&  leafs) const;

            /** Returns the intersection of the descendants mask of the roots and the ancestor mask of the leafs */
            std::vector<bool> get_flow_tree_mask(NodeVec& roots, NodeVec& leafs) const;

            /** Adds the updates to the temporary updates of the graph */
            void add_temporary_updates(Updates const &updates);

            /** Removes all temporary updates of the graph */
            void clear_temporary_updates();

            /** Returns the gradients of f with respect to the parameters provided */
            NodeVec gradient(Node f, NodeVec with_respect_to, bool backward_diff = true);

//            /** Returns the base group with the speicifed name. If it does not exists creates it. */
//            Group get_or_create_base_group(std::string name);
//
//            /** Sets the current base group to name, creates it if it does not exist */
//            void set_base_group(std::string name);
//
//            /** Sets the current base group */
//            void reset_base_group();

            Group get_or_create_group(std::string full_name);

            void set_group(std::string full_name);

            void push_group(std::string name);

            void pop_group();

            void reset_group();

//            /** Returns the group specified by full_name. If it does not exist creates it. */
//            Group get_or_create_group(std::string name, Group parent_group);
//
//            /** Returns the group specified by full_name. If it does not exist creates it. */
//            Group get_or_create_group(std::string name, std::string parent_group);
//
//            /** Returnst
//            Group get_or_create_group_full(std::string full_name);
//
//            Group get_or_create_group(std::string name);
//
//            /** Sets the current group to the group specified by base_name and its parent */
//            void set_group(std::string name, Group parent_group);
//
//            /** Sets the current group to the specified by the name. If it does not exists creates it */
//            void set_group(std::string name, std::string parent_group);
//
//            /** Sets the current group to the group specified by base_name and its parent */
//            void set_group(std::string name);

            /** Creates a new derived node (INTERNAL) */
            Node derived_node(Operator op, std::string name = "Derived");

            /** Adds an update for the shared node */
            void update_node(Node shared, Node update);

            /**
             * Finds a node which performs the same operation
             * TODO Not implemented correctly
             */
            Node find_same_node(Operator op);

            /** Looks up the max_float and max_int and limits the data_type accordingly */
            dataType limit_type(dataType data_type) const;

            /** Returns a new symbolic integer */
            SymInt new_sym();

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType data_type,
                         std::array<SymInt, 4> shape,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType data_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         SymInt shape3,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable */
            Node tensor4(dataType data_type,
                         std::string name = "InputTensor");

            /** Creates a four dimensional #INPUT variable, with the same specs as the one provided */
            Node tensor4_as(Node node,
                            std::string name = "InputTensor");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType data_type,
                         std::array<SymInt, 3> shape,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType data_type,
                         SymInt shape0,
                         SymInt shape1,
                         SymInt shape2,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable */
            Node tensor3(dataType data_type,
                         std::string name = "InputTensor3");

            /** Creates a three dimensional #INPUT variable, with the same specs as the one provided */
            Node tensor3_as(Node node,
                            std::string name = "InputTensor3");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType data_type,
                        std::array<SymInt, 2> shape,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType data_type,
                        SymInt shape0,
                        SymInt shape1,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix  */
            Node matrix(dataType data_type,
                        std::string name = "InputMatrix");

            /** Creates an #INPUT matrix, with the same specs as the one provided */
            Node matrix_as(Node node,
                           std::string name = "InputMatrix");

            /** Creates a square #INPUT matrix  */
            Node square_matrix(dataType data_type,
                               SymInt shape,
                               std::string name = "InputMatrix");

            /** Creates an #INPUT vector  */
            Node vector(dataType data_type,
                        SymInt shape,
                        std::string name = "InputVector");

            /** Creates an #INPUT vector  */
            Node vector(dataType data_type,
                        std::string name = "InputVector");

            /** Creates an #INPUT vector, with the same specs as the one provided */
            Node vector_as(Node node,
                           std::string name = "InputVector");

            /** Creates an #INPUT scalar */
            Node scalar(dataType data_type,
                        std::string name = "InputScalar");

            /** Returns a Node wrapper around a shared variable */
            Node wrap(SharedVar var);

            /** Returns a Node wrapper around a SymInt */
            Node wrap(SymInt var);

            /** Returns a Node wrapper around the value. */
            Node constant(double value, dataType data_type, Shape shape = {1, 1, 1, 1});

            template <typename T>
            /** Returns a Node wrapper around the double value. */
            Node constant(T value, Shape shape = {1, 1, 1, 1}){
                if (std::is_same<T, double>::value){
                    return constant(value, f64, shape);
                } else if (std::is_same<T, float>::value){
                    return constant(value, f32, shape);
                } else if (std::is_same<T, long>::value or std::is_same<T, int64_t >::value){
                    return constant(value, i64, shape);
                } else if (std::is_same<T, int>::value or std::is_same<T, int32_t >::value){
                    return constant(value, i32, shape);
                } else if (std::is_same<T, short>::value or std::is_same<T, int16_t >::value){
                    return constant(value, i16, shape);
                } else if (std::is_same<T, int8_t >::value){
                    return constant(value, i8, shape);
                } else if (std::is_same<T, unsigned long>::value or std::is_same<T, uint64_t >::value){
                    return constant(value, u64, shape);
                } else if (std::is_same<T, unsigned int>::value or std::is_same<T, uint32_t >::value){
                    return constant(value, u32, shape);
                } else if (std::is_same<T, unsigned short>::value or std::is_same<T, uint16_t >::value){
                    return constant(value, u16, shape);
                } else if (std::is_same<T, uint8_t >::value){
                    return constant(value, u8, shape);
                } else if (std::is_same<T, bool>::value){
                    return constant(value, b8, shape);
                } else {
                    throw 1;
                }
            }

            /** Returns a Node representing 'pi', with the maximum allowed floating point precision */
            Node PI();

            /** Returns a Node representing 'e', with the maximum allowed floating point precision */
            Node E();

            /** Returns a Node representing ln(2), with the maximum allowed floating point precision */
            Node LN_2();

            /** Returns a Node representing ln(10), with the maximum allowed floating point precision */
            Node LN_10();

            /** Returns a matrix filled with zeros with the given shape */
            Node zeros(Shape shape, dataType data_type);

            /** Returns a matrix filled with zeros with the given shape */
            Node zeros(Shape shape);

            /** Returns a matrix filled with ones with the given shape */
            Node ones(Shape shape, dataType data_type);

            /** Returns a matrix filled with ones with the given shape */
            Node ones(Shape shape);

            /** Returns a new node, which has the same value, but is considered as constant */
            Node make_constant(Node node);

            /** Returns a vector representing the sequence from start to end. */
            Node range(SymInt start, SymInt end, dataType data_type);

            /** Returns a vector representing the sequence from start to end. */
            Node range(SymInt start, SymInt end);

            /** Returns an identity matrix of the given dimension size */
            Node eye(SymInt size, dataType data_type);

            /** Returns an identity matrix of the given dimension size */
            Node eye(SymInt size);

            /** Casts the node to the type specified */
            Node cast(Node node, dataType data_type);

            /** Broadcasts the node to a specific shape */
            Node broadcast(Node node, Shape shape);

            /** Makes an alias of the original node */
            Node alias(Node node);

            /** Extracts the diagonal of a matrix or for a vector makes a matrix whose diagonal is that */
            Node diag(Node node);

            /** Reshapes a tensor */
            Node reshape(Node node, Shape shape);

            /** Reorders the dimensions of a tensor */
            Node reorder(Node node, Axes axes);

            /** Transposes a matrix or a vector, for higher order tensors, switches the last 2 dimensions */
            Node transpose(Node node);

            /** Applies not elementwise */
            Node logical_not(Node node);

            /** Applies and elementwise */
            Node logical_and(Node node1, Node node2);

            /** Applies or elementwise */
            Node logical_or(Node node1, Node node2);

            /** node1 > node2 */
            Node greater_than(Node node1, Node node2);

            /** node1 < node2 */
            Node less_than(Node node1, Node node2);

            /** node1 >= node2 */
            Node greater_than_or_equal(Node node1, Node node2);

            /** node1 <= node2 */
            Node less_than_or_equal(Node node1, Node node2);

            /** node1 == node2 */
            Node equals(Node node1, Node node2);

            /** node1 != node2 */
            Node not_equals(Node node1, Node node2);

            /** node1 == node2 up to a tolerance */
            Node approx_equals(Node node1, Node node2, double tol = 1e-9);

            /** isNan */
            Node isNan(Node node);

            /** isInf */
            Node isInf(Node node);

            /** Selects elementwise each chinels */
            Node select(Node condition, Node if_true, Node if_false);

            /** Performs addition on all of the nodes */
            Node add(NodeVec nodes);

            /** Performs addition on the two nodes */
            Node add(Node node1, Node node2);

            /** Performs addition on the two nodes */
            Node add(Node node1, Node node2, Node node3);

            /** Performs addition on the two nodes */
            Node add(Node node1, Node node2, Node node3, Node node4);

            /** Returns the negation of the node */
            Node neg(Node node);

            /** node1 - node2 **/
            Node neg(Node node1, Node node2);

            /** Multiplies nodes */
            Node mul(NodeVec nodes);

            /** Multiplies nodes */
            Node mul(Node node1, Node node2);

            /** Multiplies nodes */
            Node mul(Node node1, Node node2, Node node3);

            /** Multiplies nodes */
            Node mul(Node node1, Node node2, Node node3, Node node4);

            /** Returns the inverse  */
            Node div(Node node);

            /** node1 / node2 **/
            Node div(Node node1, Node node2);

            /** node1 // node2 **/
            Node int_div(Node node1, Node node2);

            /** node1 % node2 **/
            Node int_mod(Node node1, Node node2);

            /** Sums the node along the axis */
            Node sum(Node node, Axes axes);

            /** Sums the node along the axis */
            Node sum(Node node, short axis = auto_infer);

            /** Takes the mean along the axis */
            Node mean(Node node, Axes axes);

            /** Takes the mean along the axis */
            Node mean(Node node, short axis = auto_infer);

            /** Takes the product along the axis */
            Node prod(Node node, Axes axes);

            /** Takes the product along the axis */
            Node prod(Node node, short axis = auto_infer);

            /** Reduction with operator AND */
            Node all_true(Node node, Axes axes);

            /** Reduction with operator AND */
            Node all_true(Node node, short axis = auto_infer);

            /** Reduction with operator OR */
            Node any_true(Node node, Axes axes);

            /** Reduction with operator OR */
            Node any_true(Node node, short axis = auto_infer);

            Node square(Node node);

            Node sqrt(Node node);

            Node exp(Node node);

            Node log(Node node);

            Node log10(Node node);

            Node abs(Node node);

            Node log1p(Node node);

            Node sin(Node node);

            Node cos(Node node);

            Node tan(Node node);

            Node cot(Node node);

            Node sinh(Node node);

            Node cosh(Node node);

            Node tanh(Node node);

            Node coth(Node node);

            Node pow(Node node, Node power);

            Node gemm(NodeVec nodes, std::vector<bool> transpositions);

            Node dot(Node left, Node right, bool transpose_left = false, bool transpose_right = false);

            Node matrix_inverse(Node node);

            Node determinant(Node node);

            Node log_det(Node node);

            Node trace(Node );

            Node softplus(Node node, double threshold = 40);

            Node log_sum_exp(Node node, Axes axes, double threshold = 10);

            Node log_sum_exp(Node node, short axis = auto_infer, double threshold = 10);

            Node sigmoid(Node node);

            Node softmax(Node node);

            Node binary_cross_entropy_logits(Node p, Node q_logits);

            Node categorical_cross_entropy_logits(Node p, Node q_logits);

            Node random_uniform(Shape shape);

            Node random_normal(Shape shape);

        };

        inline Graph create_graph() {
            return std::make_shared<GraphInternal>();
        }

        /** Convenience for applying an unary operator for a derived node */
        template<typename T>
        Node apply(Node node) {
            auto g = node->graph;
            return g->derived_node(std::make_shared<T>(g, node));
        }

        /** Convenience for applying a binary operator trough template */
        template<typename T>
        Node apply(Node node1, Node node2) {
            // TODO verify the nodes are part of the same graph
            auto g = node1->graph;
            return g->derived_node(std::make_shared<T>(g, node1, node2));
        }

        /** Convenience for applying a binary operator trough template */
        template<typename T>
        Node apply(Node node1, Node node2, Node node3) {
            // TODO verify the nodes are part of the same graph
            auto g = node1->graph;
            return g->derived_node(std::make_shared<T>(g, node1, node2, node3));
        }

        /** Convenience for applying a nary operator trough template */
        template<typename T>
        Node apply(NodeVec nodes) {
            // TODO verify the nodes are part of the same graph
            auto g = nodes[0]->graph;
            return g->derived_node(std::make_shared<T>(g, nodes));
        }
    }
}
#endif //METADIFF_CORE_GRAPH_H
