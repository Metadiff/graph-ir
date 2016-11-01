//
// Created by alex on 03/10/16.
//

#include "metadiff.h"
namespace md{
    namespace core{

        NodeData::NodeData(std::weak_ptr<GraphInternal> const graph,
                           size_t id,
                           std::string name,
                           Device device,
                           Operator op,
                           unsigned int grad_level,
                           Group group):
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
                group(group) { }

        std::shared_ptr<spdlog::logger> Node::logger() const {
            return utils::logger(unwrap()->g()->name + "::node::" + std::to_string(unwrap()->id));
        }

        void Node::copy_to(const Graph graph, NodeVec ancestors) const {
            logger()->trace("Copying to node {}#{}", graph->name, graph->nodes.size());
            std::shared_ptr<NodeData> ptr = unwrap();
            std::shared_ptr<NodeData> node = std::make_shared<NodeData>(graph, ptr->device);
            node->id = graph->nodes.size();
            graph->nodes.push_back(node);
            node->device = ptr->device;
            node->name = ptr->name;
            node->is_input_dependent = ptr->is_input_dependent;
            node->is_differentiable = ptr->is_differentiable;
            node->data_type = ptr->data_type;
            node->shape = ptr->shape;
            node->op = ptr->op->copy_to(graph.get(), ancestors);
            node->grad_level = ptr->grad_level;
            node->execution = ptr->execution;
            node->group = ptr->group;
            for (size_t i = 0; i < ancestors.size(); i++) {
                ancestors[i]->children.push_back(node);
            }
        }

        int Node::dims() const {
            for(auto i=0; i<4; ++i){
                if(unwrap()->shape[3-i] != 1){
                    return 4-i;
                }
            }
            return 0;
        }
    }
}