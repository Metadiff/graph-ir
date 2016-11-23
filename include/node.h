//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_GRAPH_IR_NODE_H
#define METADIFF_GRAPH_IR_NODE_H

namespace md{
    namespace gir{

        /** The main storage class for all data on each node of the graph */
        class NodeData {
        public:
            std::weak_ptr<GraphInternal> const graph;
            size_t id;
            std::string name;
            DataType data_type;
            Shape shape;
            Operator op;
            NodeVec children;

            bool is_input_dependent;
            bool is_differentiable;
            Positivity positivity;
            MatrixPositivity matrix_positivity;
            MatrixSymmetry  matrix_symmetry;
            MatrixFill matrix_fill;

            unsigned int grad_level;
            Device device;
//            ExecutionData execution;

            std::string scope;

            NodeData(std::weak_ptr<GraphInternal> const graph, Device const device) :
                    graph(graph),
                    device(device) { }

            NodeData(std::weak_ptr<GraphInternal> const graph,
                     size_t id,
                     std::string name,
                     Device device,
                     Operator op,
                     unsigned int grad_level,
                     std::string group);
        };

        inline bool operator==(NodeData const & data1, NodeData const & data2){
            if(data1.graph.expired() or data2.graph.expired()){
                return false;
            }
            return data1.graph.lock() == data2.graph.lock() and data1.id == data2.id;
        }


        /** This is the API wrapper around the internal storage for each node - NodeData */
        class Node {
        public:
            std::weak_ptr<NodeData> ptr;

            Node() {};

            Node(std::shared_ptr<NodeData> const ptr) :
                    ptr(ptr) { };

            Node(Node const &node) :
                    ptr(node.ptr) { };

            /** @brief Unwraps the ptr to the underlying NodeData
             *
             * @return std::shared_ptr<NodeData> or throws an error if the pointer has expired.
             */
            std::shared_ptr<NodeData> unwrap() const;

            /** @brief See unwrap()
             *
             * @return
             */
            std::shared_ptr<NodeData> operator->() const{
                return unwrap();
            }

            /** @brief Returns the Graph from the NodeData
             *
             * @return
             */
            Graph g() const;

            /** @brief Returns the order of the tensor (currently are supported only tensors up to order 4)
             *
             * @return
             */
            int order() const;
        };
    }
}

namespace std {
    template <>
    class hash<md::gir::Node>{
    public :
        size_t operator()(md::gir::Node const & node ) const
        {
            return hash<std::shared_ptr<md::gir::NodeData>>()(node.ptr.lock());
        }
    };

    template <>
    class equal_to<md::gir::Node>{
    public :
        typedef md::gir::Node T;
        bool operator()( const T& lhs, const T& rhs ) const {
            if(lhs.ptr.expired() or rhs.ptr.expired()){
                return false;
            } else {
                return lhs.unwrap() == rhs.unwrap();
            }
        }
    };
};

#endif //METADIFF_GRAPH_IR_NODE_H
