//
// Created by alex on 01/10/16.
//

#ifndef METADIFF_GRAPH_IR_GRAPH_H
#define METADIFF_GRAPH_IR_GRAPH_H

#include "math.h"

namespace md{
    namespace gir{

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
            std::unordered_map<size_t, Node> updates;
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

            /** @brief Copies the nodes to the new_graph for which mask[node->id] = true
             *
             * @param new_graph
             * @param mask
             * @return
             */
            NodeVec copy(Graph new_graph, std::vector<bool> const & mask) const;

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

//            /** @brief Adds the updates to for a temporary
//             *
//             * @param updates
//             */
//            void add_temporary_updates(Updates const &updates);
//
//            /** @brief Clears all the updates in the temporary
//             *
//             */
//            void clear_temporary_updates();

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

            /** @brief Adds an update to the shared node. Throws an exception if there is already an update to that
             * node or if the shared node is not the result of a SharedInput operator.
             *
             * @param shared
             * @param update
             */
            void update_node(Node shared, Node update);

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

//            /** See md::api::tensor4(DataType, std::array<SymInt, 4>, std::string, Graph) */
//            Node tensor4(DataType data_type,
//                         std::array<SymInt, 4> shape,
//                         std::string name = "Tensor4"){
//                return api::tensor4(data_type, shape, name, shared_from_this());
//            }
//            
//            /** See md::api::tensor4(DataType, SymInt, SymInt, SymInt, SymInt, std::string, Graph) */
//            Node tensor4(DataType data_type,
//                         SymInt shape0,
//                         SymInt shape1,
//                         SymInt shape2,
//                         SymInt shape3,
//                         std::string name = "Tensor4"){
//                return api::tensor4(data_type, shape0, shape1, shape2, shape3, name, shared_from_this());
//            }
//
//            /** See md::api::tensor4(DataType, std::string, Graph) */
//            Node tensor4(DataType data_type,
//                         std::string name = "Tensor4"){
//                return api::tensor4(data_type, name, shared_from_this());
//            }
//
//            /** See md::api::tensor3(DataType, std::array<SymInt, 3>, std::string, Graph) */
//            Node tensor3(DataType data_type,
//                         std::array<SymInt, 3> shape,
//                         std::string name = "Tensor3"){
//                return api::tensor3(data_type, shape, name);
//            }
//
//            /** See md::api::tensor3(DataType, SymInt, SymInt, SymInt, std::string, Graph) */
//            Node tensor3(DataType data_type,
//                         SymInt shape0,
//                         SymInt shape1,
//                         SymInt shape2,
//                         std::string name = "Tensor3"){
//                return api::tensor3(data_type, shape0, shape1, shape2, name);
//            }
//
//            /** See md::api::tensor3(DataType, std::string, Graph) */
//            Node tensor3(DataType data_type,
//                         std::string name = "Tensor3"){
//                return api::tensor3(data_type, name);
//            }
//
//            /** See md::api::matrix(DataType, std::array<SymInt, 2>, std::string, Graph) */
//            Node matrix(DataType data_type,
//                        std::array<SymInt, 2> shape,
//                        std::string name = "Matrix"){
//                return api::matrix(data_type, shape, name);
//            }
//
//            /** See md::api::matrix(DataType, SymInt, SymInt, std::string, Graph) */
//            Node matrix(DataType data_type,
//                        SymInt shape0,
//                        SymInt shape1,
//                        std::string name = "Matrix"){
//                return api::matrix(data_type, shape0, shape1, name);
//            }
//
//            /** See md::api::matrix(DataType, std::string, Graph) */
//            Node matrix(DataType data_type,
//                        std::string name = "Matrix"){
//                return api::matrix(data_type, name);
//            }
//
//            /** See md::api::square_matrix(DataType, SymInt, std::string, Graph) */
//            Node square_matrix(DataType data_type,
//                               SymInt shape,
//                               std::string name = "Matrix"){
//                return api::square_matrix(data_type, shape, name);
//            }
//
//            /** See md::api::vector(DataType, SymInt, std::string, Graph) */
//            Node vector(DataType data_type,
//                        SymInt shape,
//                        std::string name = "Vector"){
//                return api::vector(data_type, shape, name);
//            }
//
//            /** See md::api::vector(DataType, std::string, Graph) */
//            Node vector(DataType data_type,
//                        std::string name = "Vector"){
//                return api::vector(data_type, name);
//            }
//
//            /** See md::api::scalar(DataType, std::string, Graph) */
//            Node scalar(DataType data_type,
//                        std::string name = "Scalar"){
//                return api::scalar(data_type, name);
//            }
//
//            /** See md::api::tensor_as(Node, std::string, Graph) */
//            Node tensor_as(Node node, std::string name = "Tensor"){
//                return api::tensor_as(node, name);
//            }
//
//            /** See md::api::constant(value, DataType, Shape, Graph)  */
//            Node constant(double value, DataType data_type, Shape shape = {1, 1, 1, 1}){
//                return api::constant(value, data_type, shape, shared_from_this());
//            }
//
//            /** See md::api::shared_var(SharedVar, Graph)  */
//            Node shared_var(SharedVar var){
//                return api::shared_var(var, shared_from_this());
//            }
//
//            /** See md::api::sym_int_node(SymInt, Graph)  */
//            Node sym_int_node(SymInt value){
//                return api::sym_int_node(value, shared_from_this());
//            }
//
//            template <typename T>
//            /** See md::api::constant(T, Shape)  */
//            Node constant(T value, Shape shape = {1, 1, 1, 1}){
//                return api::constant(value, shape);
//            }
//
//            /** See md::api::PI(Graph)  */
//            Node PI(){
//                return api::PI(shared_from_this());
//            }
//
//            /** See md::api::E(Graph)  */
//            Node E(){
//                return api::E(shared_from_this());
//            }
//
//            /** See md::api::LN_2(Graph)  */
//            Node LN_2(){
//                return api::LN_2(shared_from_this());
//            }
//
//            /** See md::api::LN_10(Graph)  */
//            Node LN_10(){
//                return api::LN_10(shared_from_this());
//            }
//
//            /** See md::api::zeros(Shape, DataType, Graph)  */
//            Node zeros(Shape shape, DataType data_type){
//                return api::zeros(shape, data_type, shared_from_this());
//            }
//
//            /** See md::api::zeros(Shape, Graph)  */
//            Node zeros(Shape shape){
//                return api::zeros(shape, shared_from_this());
//            }
//
//            /** See md::api::ones(Shape, DataType, Graph)  */
//            Node ones(Shape shape, DataType data_type){
//                return api::ones(shape, data_type, shared_from_this());
//            }
//
//            /** See md::api::ones(Shape, Graph)  */
//            Node ones(Shape shape){
//                return api::ones(shape, shared_from_this());
//            }
//
//            /** See md::api::range(SymInt, SymInt, DataType, Graph)  */
//            Node range(SymInt start, SymInt end, DataType data_type){
//                return api::range(start, end, data_type, shared_from_this());
//            }
//
//            /** See md::api::range(SymInt, SymInt, Graph)  */
//            Node range(SymInt start, SymInt end){
//                return api::range(start, end, shared_from_this());
//            }
//
//            /** See md::api::eye(SymInt,DataType, Graph)  */
//            Node eye(SymInt size, DataType data_type){
//                return api::eye(size, data_type, shared_from_this());
//            }
//
//            /** See md::api::eye(SymInt, Graph)  */
//            Node eye(SymInt size){
//                return api::eye(size, shared_from_this());
//            }




//            Node gemm(NodeVec nodes, std::vector<bool> transpositions);
//
//            Node dot(Node left, Node right, bool transpose_left = false, bool transpose_right = false);
//
//            Node matrix_inverse(Node node);
//
//            Node matrix_inverse_mul(Node node1, Node node2, bool transpose = false);
//
//            Node determinant(Node node);
//
//            Node log_det(Node node);
//
//            Node trace(Node );

//            Node softplus(Node node, double threshold = 40);
//
//            Node log_sum_exp(Node node, Axes axes, double threshold = 10);
//
//            Node log_sum_exp(Node node, int axis = 100, double threshold = 10);
//
//            Node softmax(Node node);
//
            Node binary_cross_entropy_logits(Node p, Node q_logits);

            Node categorical_cross_entropy_logits(Node p, Node q_logits);



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

//        /** Convenience for applying an unary operator for a derived node */
//        template<typename T>
//        Node apply(Node node) {
//            auto g = node.g();
//            return g->derived_node(std::make_shared<T>(g.get(), node));
//        }
//
//        /** Convenience for applying a binary operator trough template */
//        template<typename T>
//        Node apply(Node node1, Node node2) {
//            // TODO verify the nodes are part of the same graph
//            auto g = node1.g();
//            return g->derived_node(std::make_shared<T>(g.get(), node1, node2));
//        }
//
//        /** Convenience for applying a binary operator trough template */
//        template<typename T>
//        Node apply(Node node1, Node node2, Node node3) {
//            // TODO verify the nodes are part of the same graph
//            auto g = node1.g();
//            return g->derived_node(std::make_shared<T>(g.get(), node1, node2, node3));
//        }
//
//        /** Convenience for applying a nary operator trough template */
//        template<typename T>
//        Node apply(NodeVec nodes) {
//            // TODO verify the nodes are part of the same graph
//            auto g = nodes[0].g();
//            return g->derived_node(std::make_shared<T>(g.get(), nodes));
//        }
    }
}
#endif //METADIFF_GRAPH_IR_GRAPH_H
