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
            unsigned int grad_level = 0;
            /** The list of all of the nodes */
            std::vector<std::shared_ptr<NodeData>> nodes;
            /** List of all of the updates */
            Updates updates;
            /** List of temporary updates */
            Updates temporary_updates;

            /** List of groups */
//            std::vector<std::shared_ptr<NodeGroup>> base_groups;
            std::vector<std::shared_ptr<NodeGroup>> groups;
            /** Current group */
            Group current_group;

            std::shared_ptr<spdlog::logger> logger() const {
                return md::utils::logger("graph::" + name);
            }

            std::shared_ptr<spdlog::logger> op_logger(std::string name) const {
                return md::utils::logger("op::" + name);
            }
        public:
            /** The name of the graph */
            std::string name;
            /** Properties the user can assign of the graph */
            Properties props;

            GraphInternal(std::string name = "Function"):
                    name(name),
                    props(default_properties()){
                groups.push_back(std::make_shared<NodeGroup>("", this));
                current_group = groups[0];
            }

            /** Copies the computations with value `true` in the mask to the new_graph */
            NodeVec copy(Graph new_graph, std::vector<bool> const & mask) const;

            /** Returns an array masking all descendants of the marked nodes */
            std::vector<bool> get_descendants_mask(NodeVec const & roots) const;

            /** Returns an array masking all ancestors of the marked nodes */
            std::vector<bool> get_ancestors_mask(NodeVec const &  leafs) const;

            /** Returns the intersection of the descendants mask of the roots and the ancestor mask of the leafs */
            std::vector<bool> get_flow_tree_mask(NodeVec const & roots, NodeVec const & leafs) const;

            /** Adds the updates to the temporary updates of the graph */
            void add_temporary_updates(Updates const &updates);

            /** Removes all temporary updates of the graph */
            void clear_temporary_updates();

            /** Returns the gradient of f with respect to w. f should be a scalar */
            NodeVec gradient(Node const f, NodeVec const & w, bool backward_diff = true);

            /** Computes u^T J_f, where J_f is the Jacobian of f with respect to w (Theano's Lop) */
            NodeVec backward_diff(NodeVec const & f, NodeVec const & u, NodeVec const & w);

            /** Computes J_f v, where J_f is the Jacobian of f with respect to w (Theano's Rop) */
            NodeVec forward_diff(NodeVec const & f, NodeVec const & v, NodeVec const & w);

            Group get_or_create_group(std::string full_name);

            void set_group(std::string full_name);

            void push_group(std::string name);

            void pop_group();

            void reset_group();

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

        inline Graph create_graph() {
            return std::make_shared<GraphInternal>();
        }

        /** Convenience for applying an unary operator for a derived node */
        template<typename T>
        Node apply(Node node) {
            auto g = node->g();
            return g->derived_node(std::make_shared<T>(g.get(), node));
        }

        /** Convenience for applying a binary operator trough template */
        template<typename T>
        Node apply(Node node1, Node node2) {
            // TODO verify the nodes are part of the same graph
            auto g = node1->g();
            return g->derived_node(std::make_shared<T>(g.get(), node1, node2));
        }

        /** Convenience for applying a binary operator trough template */
        template<typename T>
        Node apply(Node node1, Node node2, Node node3) {
            // TODO verify the nodes are part of the same graph
            auto g = node1->g();
            return g->derived_node(std::make_shared<T>(g.get(), node1, node2, node3));
        }

        /** Convenience for applying a nary operator trough template */
        template<typename T>
        Node apply(NodeVec nodes) {
            // TODO verify the nodes are part of the same graph
            auto g = nodes[0]->g();
            return g->derived_node(std::make_shared<T>(g.get(), nodes));
        }
    }
}
#endif //METADIFF_CORE_GRAPH_H
