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


            void AbstractOperator::backward_diff(std::vector<NodeVec> &messages, std::vector<bool>& flow_tree) {
                // Retrieve the gradient with respect to the output of the operator
                if (messages[owner->id].size() == 0) {
                    return;
                }
                Node my_grad = backward_diff_combine(messages[owner->id]);
                logger()->debug("Generating gradients for {}", owner->id);

                // Sets the current group to _root/gradient0/parent_group
                Group current_group = graph->current_group;
                graph->current_group = owner->group;

                // Update the gradient message name
                if (my_grad->name == "Derived Node" or my_grad->name == "") {
                    my_grad->name = "Grad of " + std::to_string(owner->id) + "|";
                } else {
                    my_grad->name += "Grad of " + std::to_string(owner->id) + "|";
                }

                // This should not happen, but is here for a sanity check
                if (not is_differentiable() or not flow_tree[owner->id]) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }

                NodeVec parents = get_parents();
                // Compute and send gradients only to differentiable nodes
                for (short i = 0; i < parents.size(); ++i) {
                    if (parents[i]->is_differentiable and flow_tree[parents[i]->id]) {
                        Node parent_grad = backward_diff(my_grad, i);
                        if (parent_grad->name == "Derived Node" or parent_grad->name == "") {
                            parent_grad->name = "Grad msg " + std::to_string(owner->id) + "->"
                                                + std::to_string(parents[i]->id) + "|";
                        } else {
                            parent_grad->name += "Grad msg " + std::to_string(owner->id) + "->"
                                                 + std::to_string(parents[i]->id) + "|";
                        }
                        logger()->debug("Sending gradient message with id {} from {} to {}",
                                        parent_grad->id, owner->id, parents[i]->id);
                        messages[parents[i]->id].push_back(parent_grad);
                    }
                }
                graph->current_group = current_group;
            }

            Node AbstractOperator::backward_diff_combine(NodeVec grads) const {
                if(grads.size() == 0){
                    // TODO make an actual error
                    throw NotImplementedException("error when 0 grads in backward_diff_combine");
                } else if(grads.size() == 1){
                    return grads[0];
                } else {
                    return graph->add(grads);
                }
            }

            Node AbstractOperator::forward_diff(Node my_grad, short index){
                throw NotImplementedException("forward_diff");
            }
        }
    }
}