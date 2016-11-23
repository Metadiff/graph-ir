//
// Created by alex on 03/10/16.
//

#include "graph_ir.h"
namespace md{
    namespace gir{

        NodeData::NodeData(std::weak_ptr<GraphInternal> const graph,
                           size_t id,
                           std::string name,
                           Device device,
                           Operator op,
                           unsigned int grad_level,
                           std::string scope):
                graph(graph),
                id(id),
                name(name),
                data_type(op->get_data_type()),
                shape(op->get_shape()),
                op(op),
                is_input_dependent(op->is_input_dependent()),
                is_differentiable(op->is_differentiable()),
                grad_level(grad_level),
                device(device),
                scope(scope) { }

        Graph  Node::g() const {
            auto ptr = unwrap();
            if (ptr->graph.expired()) {
                logger("XXX::node::XXX")->error("Trying to access the graph of a Node, but the pointer has expired");
                ptr->graph.lock();
            }
            return ptr->graph.lock();
        }

//        void Node::copy_to(const Graph graph, NodeVec ancestors) const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            g_logger(ptr.g()->name)->trace("Copying node {} to graph {} resulting in node {}",
//                                            ptr->id, graph->name, graph->nodes.size());
//            std::shared_ptr<NodeData> node = std::make_shared<NodeData>(graph, ptr->device);
//            node->id = graph->nodes.size();
//            graph->nodes.push_back(node);
//            node->device = ptr->device;
//            node->name = ptr->name;
//            node->is_input_dependent = ptr->is_input_dependent;
//            node->is_differentiable = ptr->is_differentiable;
//            node->data_type = ptr->data_type;
//            node->shape = ptr->shape;
//            node->op = ptr->op->copy_to(graph.get(), ancestors);
//            node->grad_level = ptr->grad_level;
//            node->execution = ptr->execution;
//            node->group = ptr->group;
//            for (size_t i = 0; i < ancestors.size(); i++) {
//                ancestors[i]->children.push_back(node);
//            }
//        }

        std::shared_ptr<NodeData> Node::unwrap() const{
            if (ptr.expired()) {
                logger("XXX::node::XXX")->error("Trying to access the NodeData of a Node, but pointer has expired");
                ptr.lock();
            }
            return ptr.lock();
        }

        int Node::order() const {
            for(auto i=0; i<4; ++i){
                if(unwrap()->shape[3-i] != 1){
                    return 4-i;
                }
            }
            return 0;
        }
    }
}