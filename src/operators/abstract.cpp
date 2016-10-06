//
// Created by alex on 06/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        namespace op{
            void AbstractOperator::send_grad_message(size_t target, Node msg,
                                                     std::vector<Node> &messages) const {
                logger()->debug("Sending gradient message with id {} from {} to {}",
                                msg->id, owner->id, target);

                if (not messages[target].ptr.expired()) {
                    // If not first message add them and then send the sum
                    messages[target] = msg->graph->add({messages[target], msg});
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
                Group top_level = graph->get_group("Gradients " + std::to_string(graph->grad_level));
                Group owner_group = this->owner->group;
                std::string group_name = top_level.lock()->full_name;
                group_name += utils::props()->group_delimiter;
                group_name += owner_group.lock()->full_name;
                Group grad_group = graph->get_group(group_name);
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
                    if (not parents[i].is_constant()) {
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
                    if (not parents[i].is_constant()) {
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

            NodeVec AbstractOperator::get_ancestors() const {
                NodeVec parents = get_parents();
                NodeVec arguments = get_arguments();
                for (int i = 0; i < arguments.size(); i++) {
                    parents.push_back(arguments[i]);
                }
                return parents;
            }
        }
    }
}