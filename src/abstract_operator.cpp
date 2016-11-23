//
// Created by alex on 06/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{
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

            unsigned int AbstractOperator::get_grad_level() const {
                auto ancestors = get_ancestors();
                unsigned int max_grad_level = 0;
                for (auto i = 0; i < ancestors.size(); ++i) {
                    if (ancestors[i]->grad_level > max_grad_level) {
                        max_grad_level = ancestors[i]->grad_level;
                    }
                }
                return max_grad_level;
            };


            void AbstractOperator::backward_diff(std::vector<NodeVec> & derivative_messages,
                                                 std::vector<bool>& flow_tree) {
                if (derivative_messages[result->id].size() == 0) {
                    return;
                }
                // Retrieve the gradient with respect to the output of the operator
                Node my_grad = backward_diff_combine(derivative_messages[result->id]);
                op_logger(name)->debug("Generating backward diff messages from node {}", result->id);

                // Sets the current group to the group of the operator
                auto old_scope = graph->scope;
                graph->scope = result->scope;

                // Update the gradient message name
                if (my_grad->name == "Derived Node" or my_grad->name == "") {
                    my_grad->name = "Grad of " + std::to_string(result->id) + "|";
                } else {
                    my_grad->name += "Grad of " + std::to_string(result->id) + "|";
                }

                // This should not happen, but is here for a sanity check
                if (not is_differentiable() or not flow_tree[result->id]) {
                    op_logger(name)->error("Calling backward_diff unexpectedly.", name);
                    throw InternalGraphError(name, "Calling backward_diff unexpectedly.");
                }

                NodeVec parents = get_parents();
                // Compute and send gradients only to differentiable parents in the flow_tree
                for (int i = 0; i < parents.size(); ++i) {
                    if (parents[i]->is_differentiable and flow_tree[parents[i]->id]) {
                        Node parent_grad = backward_diff_parent(my_grad, i);
                        if (parent_grad->name == "Derived Node" or parent_grad->name == "") {
                            parent_grad->name = "Grad msg " + std::to_string(result->id) + "->"
                                                + std::to_string(parents[i]->id) + "|";
                        } else {
                            parent_grad->name += "Grad msg " + std::to_string(result->id) + "->"
                                                 + std::to_string(parents[i]->id) + "|";
                        }
                        op_logger(name)->debug("Sending backward diff message with id {} from {} to {}",
                                               parent_grad->id, result->id, parents[i]->id);
                        derivative_messages[parents[i]->id].push_back(parent_grad);
                    }
                }
                // Restore group
                graph->scope = old_scope;
            }

            Node AbstractOperator::backward_diff_combine(NodeVec & incoming_derivatives) const {
                if(incoming_derivatives.size() == 0){
                    op_logger(name)->error("backward_diff_combine got zero messages.");
                    throw InternalGraphError(name, "backward_diff_combine got zero messages.");
                } else if(incoming_derivatives.size() == 1){
                    return incoming_derivatives[0];
                } else {
                    return add(incoming_derivatives);
                }
            }

            void AbstractOperator::forward_diff(NodeVec & all_derivatives) {
                if(all_derivatives[result->id].ptr.expired()){
                    return;
                }

                // Retrieve the derivatives of all of the parents
                NodeVec parent_derivatives;
                NodeVec parents = get_parents();
                for(auto i=0; i<parents.size(); ++i){
                    parent_derivatives.push_back(all_derivatives[parents[i]->id]);
                }
                op_logger(name)->debug("Generating derivative for {}", result->id);

                // Sets the current group to the group of the operator
                auto old_scope = graph->scope;
                graph->scope = result->scope;

                NodeVec messages;
                for (int i = 0; i < parents.size(); ++i) {
                    if (not parent_derivatives[i].ptr.expired()) {
                        auto msg = forward_diff_parent(parent_derivatives, i);
                        if (not msg.ptr.expired()) {
                            // Change name of the message
                            if (msg->name == "Derived Node" or msg->name == "") {
                                msg->name = "Grad msg " + std::to_string(parents[i]->id) + "->"
                                            + std::to_string(result->id) + "|";
                            } else {
                                msg->name += "Grad msg " + std::to_string(result->id) + "->"
                                             + std::to_string(parents[i]->id) + "|";
                            }
                            messages.push_back(msg);
                        }
                    }
                }
                // Set the derivative of the node
                if(messages.size() == 0) {
                    // Leave to empty Node
                } else {
                    all_derivatives[result->id] = forward_diff_combine(messages);
                }

                // Restore group
                graph->scope = old_scope;
            }

            Node AbstractOperator::forward_diff_combine(NodeVec & incoming_derivatives) const {
                if(incoming_derivatives.size() == 0){
                    op_logger(name)->error("forward_diff_combine got zero messages.");
                    throw InternalGraphError(name, "forward_diff_combine got zero messages.");
                } else if(incoming_derivatives.size() == 1){
                    return incoming_derivatives[0];
                } else {
                    return add(incoming_derivatives);
                }
            }
        }
    }
}