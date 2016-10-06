//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_NODE_H
#define METADIFF_CORE_NODE_H

namespace md{
    namespace core{

        /**
         * This class stores all of the data for each single node of the GraphInternal
         */
        class NodeData {
        public:
            GraphInPtr const graph;
            size_t id;
            std::string name;
            Device device;
            Operator op;
            nodeType node_type;
            dataType data_type;
            Shape shape;
            NodeVec children;
            Group group;
            unsigned short grad_level;
            // Data populated by the optimizer
            ExecutionData execution;

            NodeData(GraphInPtr const graph, Device const device) :
                    graph(graph),
                    device(device) { }

            NodeData(GraphInPtr graph,
                     size_t id,
                     std::string name,
                     Device device,
                     Operator op,
                     unsigned short grad_level,
                     Group group);
        };


        /** This is the API wrapper around the internal storage for each node - NodeData */
        class Node {
        private:
            std::shared_ptr<spdlog::logger> logger() const;
        public:
            std::weak_ptr<NodeData> ptr;

            Node() {};

            Node(std::shared_ptr<NodeData> const ptr) :
                    ptr(ptr) { };

            Node(Node const &node) :
                    ptr(node.ptr) { };

            Node(Node const *node) :
                    ptr(node->ptr) { };

            /** Unwraps the pointer to the underlying NodeData.
             * Exits the program if the pointer has expired */
            std::shared_ptr<NodeData> unwrap() const{
                if (ptr.expired()) {
                    utils::logger("XXX::node::XXX")->error("Trying to access a Node whose pointer has expired");
                    throw 1;
                }
                return ptr.lock();
            }

            /**
             * This operator is overloaded to call class: unwrap()
             */
            std::shared_ptr<NodeData> operator->() const{
                return unwrap();
            }

            /** Copies the node to another graph, by using the ancestors
             * provided from the new graph */
            void copy_to(const GraphInPtr graph, NodeVec ancestors) const;

            bool is_constant() const;

            bool is_scalar() const;

            bool is_vector() const;

            bool is_vector_strict() const;

            bool is_matrix() const;

            bool is_matrix_strict() const;

            bool is_tensor3() const;

            bool is_tensor3_strict() const;

            bool is_tensor4_strict() const;

            void update(Node update);

            Node alias();

            Node cast(dataType data_type);

            Node broadcast(Shape shape);

            Node broadcast_to(Node other);

            Node neg();

            Node div();

            Node sum(Axes axes = {0, 1, 2, 3});

            Node square();

            Node as_constant();

            Node logical_not();

            Node logical_and(Node node);

            Node logical_or(Node node);

            Node gt(Node node);

            Node ge(Node node);

            Node lt(Node node);

            Node le(Node node);

            Node eq(Node node);

            Node neq(Node node);

            Node approx_eq(Node node, double tol = 0.000001);

            Node approx_neq(Node node, double tol = 0.000001);

            Node is_nan();

            Node is_inf();

            Node all();

            Node any();

            Node select(Node result_true, Node result_false);

            Node exp();

            Node log();

            Node log10();

            Node log1p();

            Node softplus(int threshold = 50);

            Node abs();

            Node sigmoid();

            Node relu();

            Node sin();

            Node cos();

            Node tan();

            Node cot();

            Node sinh();

            Node cosh();

            Node tanh();

            Node coth();

            Node pow(Node power);

            Node transpose();

            Node dot(Node b, bool transpose_a = false, bool transpose_b = false);

            Node minv();

            Node det();

            Node logdet();

            Node trace();

            Node diag();

            Node reshape(Shape shape);

            Node flatten(short dims = 1);

            Node reorder(Axes order);

            Node reorder(short dim0, short dim1, short dim2 = 2, short dim3 = 3);

//            Node slice(Node index, short axis = 0);
//
//            Node index(Node index, short axis = auto_infer);

            Node max(short axis = auto_infer);

            Node argMax(short axis = auto_infer);

            std::pair<Node, Node> maxAndArgMax(short axis = auto_infer);

            Node sort(short axis = auto_infer);

            Node argSort(short axis = auto_infer);

            std::pair<Node, Node> sortAndArgSort(short axis = auto_infer);

            Node binary_cross_entropy_logit(Node node);

        };
    }
}
#endif //METADIFF_CORE_NODE_H
