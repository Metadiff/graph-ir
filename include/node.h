//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_NODE_H
#define METADIFF_CORE_NODE_H

namespace md{
    namespace core{

        /** This class stores all of the data for each single node of the Graph */
        class NodeData {
        public:
            GraphInPtr const graph;
            size_t id;
            std::string name;
            dataType data_type;
            Shape shape;
            bool is_input_dependent;
            bool is_differentiable;
            Operator op;
            NodeVec children;

            unsigned short grad_level;
            Device device;
            ExecutionData execution;

            Group group;

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

            /** The pointer operator is overloaded to call the unwrap method */
            std::shared_ptr<NodeData> operator->() const{
                return unwrap();
            }

            /** Copies the node to a new graph */
            void copy_to(const GraphInPtr graph, NodeVec ancestors) const;

            /** Returns the actual dimensionality of the tensor */
            int dims() const;
        };
    }
}
#endif //METADIFF_CORE_NODE_H
