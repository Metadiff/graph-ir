//
// Created by alex on 06/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        namespace op{
            NodeVec AbstractOperator::get_ancestors() const {
                auto ancestors = get_parents();
                auto arguments = get_arguments();
                std::copy (ancestors.begin(), ancestors.end(), std::back_inserter(arguments));
                return ancestors;
            }

            NodeVec AbstractOperator::get_arguments() const {
                return NodeVec{};
            }

            bool AbstractOperator::is_input_dependent() const {
                auto arguments = get_arguments();
                for(auto i=0; i<arguments.size(); ++i){
                    if(arguments[i]->is_input_dependent){
                        return true;
                    }
                }
                return false;
            }

            bool AbstractOperator::is_differentiable() const {
                auto parents = get_parents();
                for(auto i=0; i<parents.size(); ++i){
                    if(parents[i]->is_differentiable){
                        return true;
                    }
                }
                return false;
            }
//            nodeType AbstractOperator::get_node_type() const {
//                auto is_non_diff = false;
//                auto parents = get_parents();
//                for(auto i=0; i < parents.size(); ++i){
//                    if(parents[i]->node_type > INPUT_DERIVED_NON_DIFF){
//                        return INPUT_DERIVED;
//                    }
//                    if(parents[i]->node_type == INPUT_DERIVED_NON_DIFF){
//                        is_non_diff = true;
//                    }
//                }
//                if(is_non_diff){
//                    return INPUT_DERIVED_NON_DIFF;
//                }
//                auto arguments = get_arguments();
//                for(auto i=0; i < arguments.size(); ++i){
//                    if(arguments[i]->node_type > CONSTANT_DERIVED){
//                        return INPUT_DERIVED_NON_DIFF;
//                    }
//                }
//                return CONSTANT_DERIVED;
//            };

//            dataType AbstractOperator::get_data_type() const {
//                dataType max = b8;
//                auto parents = get_parents();
//                for (size_t i = 0; i < parents.size(); i++) {
//                    if(parents[i]->data_type > max){
//                        max = parents[i]->data_type;
//                    }
//                }
//                if(max < i8){
//                    return static_cast<dataType >(1 + (graph->max_int - 1) % 4);
//                } else if(max < f8){
//                    return graph->max_int;
//                } else {
//                    return graph->max_float;
//                }
//            }

            /** Calculates what should be the resulting NodeData#grad_level */
            unsigned short AbstractOperator::get_grad_level() const {
                auto ancestors = get_ancestors();
                unsigned short max_grad_level = 0;
                for (auto i = 0; i < ancestors.size(); ++i) {
                    if (ancestors[i]->grad_level > max_grad_level) {
                        max_grad_level = ancestors[i]->grad_level;
                    }
                }
                return max_grad_level;
            };

            void AbstractOperator::send_grad_message(size_t target, Node msg,
                                                     std::vector<Node> &messages) const {
                logger()->debug("Sending gradient message with id {} from {} to {}",
                                msg->id, owner->id, target);
                if (not messages[target].ptr.expired()) {
                    // If not first message add them and then send the sum
                    messages[target] = msg->graph->add(messages[target], msg);
                } else {
                    // If first just send it
                    messages[target] = msg;
                }
            }

            void AbstractOperator::generate_gradients(NodeVec &messages) {
                // Check if there are any incoming gradient messages
                if (messages[owner->id].ptr.expired()) {
                    return;
                }
                logger()->debug("Generating gradients for {}", owner->id);

                // Sets the current group to _root/gradient0/parent_group
                Group current_group = graph->current_group;
                Group owner_group = this->owner->group;
                Group grad_group = graph->get_or_create_group_from_base(owner_group.lock()->full_name);
                graph->current_group = grad_group;

                // Retrieves the gradient messages representing the total gradient with respect to this node
                Node my_grad = messages[owner->id];

                // Update the gradient message name
                if (my_grad->name == "Derived Node" or my_grad->name == "") {
                    my_grad->name = "Grad of " + std::to_string(owner->id) + "|";
                } else {
                    my_grad->name += "Grad of " + std::to_string(owner->id) + "|";
                }

                // Check for any surprises, where all parents are constants
                // If that is the case this node should have been constant as well
                // and no message should have been sent to it, however if the node is
                // an input node than it is never constant
                NodeVec parents = get_parents();
                bool constant = name != "Input" and name != "Shared";
                for (int i = 0; i < parents.size(); i++) {
                    if (parents[i]->is_differentiable) {
                        constant = false;
                        break;
                    }
                }
                if (constant) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }

                // Compute and send gradients only to non constant nodes
                for (short i = 0; i < parents.size(); i++) {
                    if (parents[i]->is_differentiable) {
                        Node parent_grad = get_parent_grad(my_grad, i);
                        if (parent_grad->name == "Derived Node" or parent_grad->name == "") {
                            parent_grad->name = "Grad msg " + std::to_string(owner->id) + "->"
                                                + std::to_string(parents[i]->id) + "|";
                        } else {
                            parent_grad->name += "Grad msg " + std::to_string(owner->id) + "->"
                                                 + std::to_string(parents[i]->id) + "|";
                        }
                        send_grad_message(parents[i]->id, parent_grad, messages);
                    }
                }
                graph->current_group = current_group;
            }
        }
    }
}