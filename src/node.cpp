//
// Created by alex on 03/10/16.
//

#include "metadiff.h"
namespace md{
    namespace core{

        NodeData::NodeData(GraphInPtr graph,
                           size_t id,
                           std::string name,
                           Device device,
                           Operator op,
                           unsigned short grad_level,
                           Group group):
                graph(graph),
                id(id),
                name(name),
                device(device),
                op(op),
                node_type(op->get_node_type()),
                data_type(op->get_data_type()),
                shape(op->get_shape()),
                group(group),
                grad_level(grad_level) { }

        std::shared_ptr<spdlog::logger> Node::logger() const {
            return utils::logger(unwrap()->graph->name + "::node::" + std::to_string(unwrap()->id));
        }

        void Node::copy_to(const GraphInPtr graph, NodeVec ancestors) const {
            logger()->trace("Copying to node {}#{}", graph->name, graph->nodes.size());
            std::shared_ptr<NodeData> ptr = unwrap();
            std::shared_ptr<NodeData> node = std::make_shared<NodeData>(graph, ptr->device);
            node->id = graph->nodes.size();
            graph->nodes.push_back(node);
            node->device = ptr->device;
            node->name = ptr->name;
            node->node_type = ptr->node_type;
            node->data_type = ptr->data_type;
            node->shape = ptr->shape;
            node->op = ptr->op->copy_to(graph, ancestors);
            node->grad_level = ptr->grad_level;
//        node->value = ptr->value;
//        node->shared = ptr->shared;
            node->execution = ptr->execution;
            node->group = ptr->group;
            for (size_t i = 0; i < ancestors.size(); i++) {
                ancestors[i]->children.push_back(node);
            }
        }

        bool Node::is_constant() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            if (ptr->node_type == CONSTANT or ptr->node_type == CONSTANT_DERIVED) {
                return true;
            }
            return ptr->graph->is_temporary_constant(this);
        }

        bool Node::is_scalar() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 0; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_vector() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 1; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_vector_strict() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 0; i < 1; i++) {
                if (ptr->shape[i] == 1) {
                    return false;
                }
            }
            for (int i = 1; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_matrix() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 2; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_matrix_strict() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 0; i < 2; i++) {
                if (ptr->shape[i] == 1) {
                    return false;
                }
            }
            for (int i = 2; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_tensor3() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 3; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_tensor3_strict() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 0; i < 3; i++) {
                if (ptr->shape[i] == 1) {
                    return false;
                }
            }
            for (int i = 3; i < 4; i++) {
                if (ptr->shape[i] != 1) {
                    return false;
                }
            }
            return true;
        }

        bool Node::is_tensor4_strict() const {
            std::shared_ptr<NodeData> ptr = unwrap();
            for (int i = 0; i < 4; i++) {
                if (ptr->shape[i] == 1) {
                    return false;
                }
            }
            return true;
        }
    }
}