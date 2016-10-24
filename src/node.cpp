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
                data_type(op->get_data_type()),
                shape(op->get_shape()),
                is_input_dependent(op->is_input_dependent()),
                is_differentiable(op->is_differentiable()),
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
            node->is_input_dependent = ptr->is_input_dependent;
            node->is_differentiable = ptr->is_differentiable;
            node->data_type = ptr->data_type;
            node->shape = ptr->shape;
            node->op = ptr->op->copy_to(graph, ancestors);
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

//        bool Node::is_scalar() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 0; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_vector() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 1; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_vector_strict() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 0; i < 1; i++) {
//                if (ptr->shape[i] == 1) {
//                    return false;
//                }
//            }
//            for (int i = 1; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_matrix() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 2; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_matrix_strict() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 0; i < 2; i++) {
//                if (ptr->shape[i] == 1) {
//                    return false;
//                }
//            }
//            for (int i = 2; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_tensor3() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 3; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_tensor3_strict() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 0; i < 3; i++) {
//                if (ptr->shape[i] == 1) {
//                    return false;
//                }
//            }
//            for (int i = 3; i < 4; i++) {
//                if (ptr->shape[i] != 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
//
//        bool Node::is_tensor4_strict() const {
//            std::shared_ptr<NodeData> ptr = unwrap();
//            for (int i = 0; i < 4; i++) {
//                if (ptr->shape[i] == 1) {
//                    return false;
//                }
//            }
//            return true;
//        }
    }
}