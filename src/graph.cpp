//
// Created by alex on 03/10/16.
//

#include "metadiff.h"
#include "sstream"

namespace md{
    namespace core{
        SymInt GraphInternal::new_sym() {
            return SymInt::registry->new_variable();
        }

        dataType GraphInternal::limit_type(dataType data_type) const {
            if(data_type > i64) {
                return data_type <= props.max_float ? data_type : props.max_float;
            } else if(data_type > u64){
                // It is an int
                return data_type <= props.max_int ? data_type : props.max_int;
            } else{
                // It is an uint
                switch (props.max_int){
                    case i64:
                        return data_type <= i64 ? data_type : u64;
                    case i32:
                        return data_type <= u32 ? data_type : u32;
                    case i16:
                        return data_type <= u16 ? data_type : u16;
                    case i8:
                        return data_type <= u8 ? data_type : u8;
                    default:
                        return b8;
                }
            }
        }

        NodeVec GraphInternal::copy(GraphInPtr new_graph, std::vector<bool> const & mask) const {
            logger()->trace("Copying graph {}", name);
            new_graph->name = name + "_copy";
            new_graph->props = props;
            new_graph->groups = groups;
            size_t n = nodes.size();
            // Variable which maps each node (by id) of the original graph to a Node in the new graph
            NodeVec mapping(n, Node());
            // Copy nodes
            for (size_t i = 0; i < n; i++) {
                // Only nodes that are masked
                if (mask[i]) {
                    // Get all of the ancestors of the node and find their corresponding nodes
                    // in the new graph
                    NodeVec ancestors = nodes[i]->op->get_ancestors();
                    NodeVec new_ancestors;
                    for (size_t j = 0; j < ancestors.size(); j++) {
                        new_ancestors.push_back(mapping[ancestors[j]->id]);
                    }
                    // Copy the node using the new ancestors and put it in the mapping
                    Node(nodes[i]).copy_to(new_graph, new_ancestors);
                    mapping[nodes[i]->id] = new_graph->nodes.back();
                }
            }
            // Copy the updates, by just adding the corresponding nodes
            for (size_t i = 0; i < updates.size(); i++) {
                if (mapping[updates[i].second->id].ptr.expired()) {
                    logger()->error("Could not copy the update for node {} "
                                            " because it was not part of the mask.", updates[i].first->id);
                } else {
                    Node shared = mapping[updates[i].first->id];
                    Node update = mapping[updates[i].second->id];
                    new_graph->updates.push_back(std::pair<Node, Node>(shared, update));
                }
            }
            // Update all names containing numbers
            // Not supported because gcc 4.8.x does not support regex
            logger()->trace("Copy completed");
            return mapping;
        }

        std::vector<bool> GraphInternal::get_descendants_mask(NodeVec& roots) const {
            logger()->trace("Generating descendants mask");
            auto n = nodes.size();
            std::vector<bool> descendants_mask(n, false);

            // Mark all of the roots
            for (int i = 0; i < roots.size(); i++) {
                descendants_mask[roots[i]->id] = true;
            }

            // For each node that is already marked mark all of its children
            for (auto i = 0; i < n; ++i) {
                if (descendants_mask[i]) {
                    auto children = nodes[i]->children;
                    for (auto j = 0; j < children.size(); ++j) {
                        descendants_mask[children[j]->id] = true;
                    }
                }
            }
            return descendants_mask;
        };

        std::vector<bool> GraphInternal::get_ancestors_mask(NodeVec& leafs) const {
            logger()->trace("Generating ancestors mask");
            auto n = nodes.size();
            std::vector<bool> ancestors_mask(n, false);

            // Mark all of the leafs
            for (int i = 0; i < leafs.size(); i++) {
                ancestors_mask[leafs[i]->id] = true;
            }

            // For each node that is already marked mark all of its ancestors
            for (size_t i = n - 1; i < n; i--) {
                if (ancestors_mask[i]) {
                    auto ancestors = nodes[i]->op->get_ancestors();
                    for (auto j = 0; j < ancestors.size(); ++j) {
                        ancestors_mask[ancestors[j]->id] = true;
                    }
                }
            }
            return ancestors_mask;
        };


        std::vector<bool> GraphInternal::get_flow_tree_mask(NodeVec& roots, NodeVec& leafs) const {
            auto descendants_mask = get_descendants_mask(roots);
            auto ancestors_mask = get_ancestors_mask(leafs);
            for(auto i=0; i < descendants_mask.size(); ++i){
                descendants_mask[i] = descendants_mask[i] and ancestors_mask[i];
            }
            return descendants_mask;
        }


        Node GraphInternal::find_same_node(Operator op) {
            // TODO have to do this function properly
            if (op->get_parents().size() > 0) {
                NodeVec candidates = op->get_parents()[0]->children;
                for (int i = 0; i < candidates.size(); i++) {
                    Operator candidate_op = candidates[i]->op;
                    if (candidate_op->equals(op) or op->equals(candidate_op)) {
                        logger()->debug("Found node with id {} equal to operator {}",candidates[i]->id, op->name);
                        return candidates[i];
                    }
                }
            }
            return Node();
        };

        void GraphInternal::add_temporary_updates(const Updates &temp_updates) {
            for (int i = 0; i < temp_updates.size(); i++) {
                logger()->trace("Adding a temporary update {} := {}",
                                temp_updates[i].first->id, temp_updates[i].second->id);
                temporary_updates.push_back(temp_updates[i]);
            }
        };

        void GraphInternal::clear_temporary_updates() {
            // Potentially might need to sort temporary_updates
            logger()->trace("Clearing temporary updates");
            temporary_updates.clear();
        }

        std::vector<Node> GraphInternal::gradient(Node objective, NodeVec with_respect_to, bool backward_diff) {
            logger()->trace("Getting gradients of {}", objective->id);
            if (objective.dims() != 0) {
                auto err = std::make_shared<UnsupportedGradient>(objective);
                err->log(logger());
                throw err;
            }

            // Extract the flow tree between the objective and with_respect_to
            NodeVec mock = {objective};
            std::vector<bool> flow_tree = get_flow_tree_mask(with_respect_to, mock);

            // The gradient mode is one higher than that of the objective
            grad_level = objective->grad_level + ((unsigned short)(1));

            // Stores the current group in order to recreate it
            Group old_current = current_group;
            current_group = objective->group;
            // Set the current group to the corresponding gradients group

            // Contains all gradient messages sent around
            std::vector<NodeVec> grad_messages(nodes.size(), NodeVec{});

            // Send the first message as 1 to the objective
            grad_messages[objective->id].push_back(constant(1));

            // This will contain the final gradients
            NodeVec grads;
            if(backward_diff) {
                // Send all gradient messages
                for (auto i = flow_tree.size(); i > 0; --i) {
                    if (flow_tree[i]) {
                        nodes[i]->op->backward_diff(grad_messages, flow_tree);
                    }
                }
                for (auto i = 0; i < with_respect_to.size(); ++i) {
                    // TODO Have a policy if you request gradient and something don't get one
                    grads.push_back(grad_messages[with_respect_to[i]->id][0]);
                }
            } else {
                throw 1;
            }

            // Reset the gradient mode
            grad_level = 0;

            // Restore the current group
            current_group = old_current;

            return grads;
        };


        Node GraphInternal::derived_node(Operator op, std::string name) {
            Node same_node = find_same_node(op);
            if (same_node.ptr.expired()) {
                auto result = std::make_shared<NodeData>(
                        shared_from_this().get(),
                        nodes.size(),
                        name,
                        props.default_device,
                        op,
                        grad_level > op->get_grad_level() ? grad_level : op->get_grad_level(),
                        current_group
                );
                nodes.push_back(result);
                op->owner = result;
                NodeVec ancestors = op->get_ancestors();
                for (int i = 0; i < ancestors.size(); i++) {
                    ancestors[i]->children.push_back(result);
                }
                return result;
            } else {
                return alias(same_node);
            }
        }

        void GraphInternal::update_node(Node shared, Node update) {
            // Check the first node is a shared variable
            if (shared->op->name != "Shared") {
                auto err = std::make_shared<InvalidArguments>(NodeVec {shared, update},
                                                              "Update",
                                                              "First argument must be a SHARED_VARIABLE");
                err->log(logger());
                throw err;
            }
            auto shared_shape = shared->shape;
            auto update_shape = update->shape;

            // Check that the shapes are correct
            for (int i = 0; i < 4; i++) {
                if (shared_shape[i] != update_shape[i]) {
                    auto err = std::make_shared<IncompatibleShapes>(NodeVec{shared, update}, "Update");
                    err->log(logger());
                    throw err;
                }
            }

            // Check that the value types are correct
            if (shared->data_type != update->data_type) {
                auto err = std::make_shared<InvalidArguments>(NodeVec{shared, update},
                                                              "Update",
                                                              "The Shared variable and the update should have the same dtype");
                err->log(logger());
                throw err;
            }
            // Add it to the updates
            updates.push_back(std::pair<Node, Node>(shared, update));
        }


        Group GraphInternal::get_or_create_group(std::string full_name){
            Group group = groups[0];
            std::stringstream name_stream(full_name);
            std::string item;
            while (std::getline(name_stream, item, props.group_delimiter)) {
                auto index = group.lock()->children.size();
                for (auto i = 0; i < index; ++i) {
                    if (item.compare(group.lock()->children[i].lock()->name) == 0) {
                        index = i;
                        break;
                    }
                }
                // If not found make a new group
                if (index == group.lock()->children.size()) {
                    groups.push_back(std::make_shared<NodeGroup>(item, group));
                    auto new_group = groups.back();
                    group.lock()->children.push_back(new_group);
                    logger()->trace("Creating group {} with parent {} and name {}",
                                    new_group->full_name, group.lock()->name, item);
                    group = new_group;
                } else {
                    group = group.lock()->children[index];
                }
            }
            return group;
        }

        void GraphInternal::set_group(std::string full_name){
            current_group = get_or_create_group(full_name);
        };

        void GraphInternal::push_group(std::string name){
            std::string full_name = current_group.lock()->full_name;
            full_name += props.group_delimiter;
            full_name += name;
            current_group = get_or_create_group(full_name);
        }

        void GraphInternal::pop_group() {
            if(not current_group.lock()->is_base()){
                current_group = current_group.lock()->parent;
            } else {
                // TODO maybe throw an error
            }
        }

        void GraphInternal::reset_group(){
            current_group = groups[0];
        }
    }
}

