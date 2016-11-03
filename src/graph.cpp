//
// Created by alex on 03/10/16.
//

#include "graph_ir.h"
#include "sstream"

namespace md{
    namespace gir{
        DataType GraphInternal::limit_type(DataType data_type) const {
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

        NodeVec GraphInternal::copy(Graph new_graph, std::vector<bool> const & mask) const {
            g_logger(name)->trace("Copying graph {}", name);
            new_graph->name = name + "_copy";
            new_graph->props = props;
            size_t n = nodes.size();
            // Variable which maps each node (by id) of the original graph to a Node in the new graph
            NodeVec mapping(n, Node());
            // Copy nodes
            for (auto i = 0; i < n; i++) {
                // Only nodes that are masked
                if (mask[i]) {
                    g_logger(name)->trace("Copying node {} to graph {} resulting in node {}",
                                          i, new_graph->name, new_graph->nodes.size());
                    // Get all of the ancestors of the node and find their corresponding nodes
                    // in the new graph
                    NodeVec ancestors = nodes[i]->op->get_ancestors();
                    NodeVec new_ancestors;
                    for (size_t j = 0; j < ancestors.size(); j++) {
                        new_ancestors.push_back(mapping[ancestors[j]->id]);
                    }
                    // Copy the node using the new ancestors and put it in the mapping
                    Operator op = nodes[i]->op->copy_to(new_graph.get(), new_ancestors);
                    mapping[nodes[i]->id] = new_graph->derived_node(op, nodes[i]->name);
                }
            }
            // Copy the updates, by just adding the corresponding nodes
            for(auto it = updates.begin(); it != updates.end(); ++it){
                if(mapping[it->second->id].ptr.expired()){
                    g_logger(name)->error("Could not copy the update for node {} "
                                                  " because it was not part of the mask.", it->first);
                    throw InternalGraphError("Copy", "Could not copy the update for node "
                                                     + std::to_string(it->first)
                                                     + " because it was not part of the mask.");
                } else {
                    Node shared = mapping[it->first];
                    Node update = mapping[it->second->id];
                    api::update(shared, update);
                }
            }
            g_logger(name)->trace("Copy completed");
            return mapping;
        }

        std::vector<bool> GraphInternal::get_descendants_mask(NodeVec const & roots) const {
            g_logger(name)->trace("Generating descendants mask");
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

        std::vector<bool> GraphInternal::get_ancestors_mask(NodeVec const & leafs) const {
            g_logger(name)->trace("Generating ancestors mask");
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


        std::vector<bool> GraphInternal::get_flow_tree_mask(NodeVec const & roots, NodeVec const & leafs) const {
            auto descendants_mask = get_descendants_mask(roots);
            auto ancestors_mask = get_ancestors_mask(leafs);
            for(auto i=0; i < descendants_mask.size(); ++i){
                descendants_mask[i] = descendants_mask[i] and ancestors_mask[i];
            }
            return descendants_mask;
        }


        Node GraphInternal::find_same_node(Operator op) {
            // TODO This needs more rigorous approach together with symbolically_equals() and AbstractOperator::equals()
            if (op->get_parents().size() > 0) {
                NodeVec candidates = op->get_parents()[0]->children;
                for (int i = 0; i < candidates.size(); i++) {
                    Operator candidate_op = candidates[i]->op;
                    if (candidate_op->equals(op) or op->equals(candidate_op)) {
                        g_logger(name)->debug("Found node with id {} equal to operator {}",candidates[i]->id, op->name);
                        return candidates[i];
                    }
                }
            }
            return Node();
        };

//        void GraphInternal::add_temporary_updates(const Updates &temp_updates) {
//            for (int i = 0; i < temp_updates.size(); i++) {
//                g_logger(name)->trace("Adding a temporary update {} := {}",
//                                      temp_updates[i].first->id, temp_updates[i].second->id);
//                temporary_updates.push_back(temp_updates[i]);
//            }
//        };
//
//        void GraphInternal::clear_temporary_updates() {
//            // Potentially might need to sort temporary_updates
//            g_logger(name)->trace("Clearing temporary updates");
//            temporary_updates.clear();
//        }

        NodeVec GraphInternal::backward_diff(NodeVec const & f, NodeVec const & u_in, NodeVec const & w){
            // If no parameters return empty vector as well
            if(w.size() == 0){
                return NodeVec();
            }
            // Verify same number of f's and u's
            if(f.size() != u_in.size()){
                op_logger("BackwardDiff")->error("Different number of functions (f) and evaluation points (u) provided.");
                throw InvalidOperatorArgument(f, "BackwardDiff",
                                              "Different number of functions (f) and evaluation points (u) provided.");
            }
            // Verify at least one f
            if(f.size() == 0){
                op_logger("BackwardDiff")->error("Zero number of functions (f) provided.");
                throw InvalidOperatorArgument(f, "BackwardDiff",
                                              "Zero number of functions (f) provided.");
            }
            // Verify all nodes are from this graph
            Graph g = shared_from_this();
            for(auto i=0; i<f.size(); ++i){
                if(g != f[i].g()){
                    op_logger("BackwardDiff")->error("f[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "BackwardDiff",
                                                  "f[" + std::to_string(i) + "] is not part of this graph.");
                }
                if(g != u_in[i].g()){
                    op_logger("BackwardDiff")->error("u[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "BackwardDiff",
                                                  "u[" + std::to_string(i) + "] is not part of this graph.");
                }
            }
            for(auto i=0; i<w.size(); ++i){
                if(g != w[i].g()){
                    op_logger("BackwardDiff")->error("w[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "BackwardDiff",
                                                  "w[" + std::to_string(i) + "] is not part of this graph.");
                }
            }
            // Verify all of the u's are correct shapes
            NodeVec u = u_in;
            for(auto i=0; i<f.size(); ++i){
                u[i] = implicit_broadcast(u[i], f[i]->shape, "BackwardDiff");
            }

            op_logger("BackwardDiff")->trace("Starting BackwardDiff");

            std::vector<bool> flow_tree = get_flow_tree_mask(w, f);

            // Contains all of the backward messages
            std::vector<NodeVec> messages(nodes.size(), NodeVec{});

            // The first messages are u_i -> f_i
            size_t max_id = 0;
            size_t min_id = nodes.size();
            for(auto i=0; i<f.size(); ++i){
                op_logger("BackwardDiff")->trace("Initial message u[{}] -> f[{}] is {} -> {}", i, i, u[i]->id, f[i]->id);
                messages[f[i]->id].push_back(u[i]);
                max_id = max_id < f[i]->id ? f[i]->id : max_id;
                min_id = min_id > f[i]->id ? f[i]->id : min_id;
                grad_level = grad_level < (f[i]->grad_level + (unsigned int)(1)) ? (f[i]->grad_level + (unsigned int)(1)) : grad_level;
            }
            for(auto i=0; i<w.size(); ++i){
                min_id = min_id > w[i]->id ? w[i]->id : min_id;
            }

            // Generate all the messages around
            for (auto i = max_id; i >= min_id; --i) {
                if (flow_tree[i]) {
                    nodes[i]->op->backward_diff(messages, flow_tree);
                }
            }

            // Reset the grad level
            grad_level = 0;

            NodeVec outputs;
            for (auto i = 0; i < w.size(); ++i) {
                if(messages[w[i]->id].size() == 0){
                    switch (props.policies.independent_derivative){
                        case RAISE: {
                            op_logger("BackwardDiff")->error("None of the functions (f) depend on the parameter w[{}].", i);
                            throw InvalidOperatorArgument(f, "BackwardDiff",
                                                          "None of the functions (f) depend on the parameter w[" +
                                                          std::to_string(i) + "]");
                        }
                        case WARN:
                            op_logger("BackwardDiff")->warn("None of the functions (f) depend on the parameter w[{}].", i);
                        default: ;
                    }
                    outputs.push_back(constant(0));
                } else if(messages[w[i]->id].size() == 1){
                    outputs.push_back(messages[w[i]->id][0]);
                } else {
                    op_logger("BackwardDiff")->error("w[{}] has more than one backward message left.", i);
                    throw InternalGraphError("BackwardDiff", "w[" + std::to_string(i) +
                                                             "] has more than one backward message left.");
                }
            }

            return outputs;
        }

        NodeVec GraphInternal::forward_diff(NodeVec const & f, NodeVec const & v_in, NodeVec const & w){
            // If no parameters return empty vector as well
            if(w.size() == 0){
                return NodeVec();
            }
            // Verify same number of f's and u's
            if(w.size() != v_in.size()){
                op_logger("ForwardDiff")->error("Different number of parameters (w) and evaluation points (v) provided.");
                throw InvalidOperatorArgument(f, "ForwardDiff",
                                              "Different number of parameters (w) and evaluation points (v) provided.");
            }
            // Verify at least one f
            if(f.size() == 0){
                op_logger("ForwardDiff")->error("Zero number of functions (f) provided.");
                throw InvalidOperatorArgument(f, "ForwardDiff",
                                              "Zero number of functions (f) provided.");
            }
            // Verify all nodes are from this graph
            Graph g = shared_from_this();
            for(auto i=0; i<f.size(); ++i) {
                if (g != f[i].g()) {
                    op_logger("ForwardDiff")->error("f[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "ForwardDiff",
                                                  "f[" + std::to_string(i) + "] is not part of this graph.");
                }
            }
            for(auto i=0; i<w.size(); ++i){
                if(g != w[i].g()){
                    op_logger("ForwardDiff")->error("w[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "ForwardDiff",
                                                  "w[" + std::to_string(i) + "] is not part of this graph.");
                }
                if(g != v_in[i].g()){
                    op_logger("ForwardDiff")->error("v[{}] is not part of this graph.", i);
                    throw InvalidOperatorArgument(f, "ForwardDiff",
                                                  "v[" + std::to_string(i) + "] is not part of this graph.");
                }
            }
            // Verify all of the v's are correct shapes
            NodeVec v = v_in;
            for(auto i=0; i<w.size(); ++i){
                if(v[i]->shape != w[i]->shape) {
                    v[i] = implicit_broadcast(v[i], w[i]->shape, "ForwardDiff");
                }
            }
            op_logger("ForwardDiff")->trace("Starting ForwardDiff");

            std::vector<bool> flow_tree = get_flow_tree_mask(w, f);
            // Contains all of the backward messages
            NodeVec derivatives(nodes.size(), Node());

            size_t max_id = 0;
            size_t min_id = nodes.size();
            for(auto i=0; i<f.size(); ++i){
                max_id = max_id < f[i]->id ? f[i]->id : max_id;
            }
            // The directional derivatives at w[i] are just v[i]
            for(auto i=0; i<w.size(); ++i){
                op_logger("ForwardDiff")->trace("Initial derivatives for w[{}] = {}", w[i]->id, v[i]->id);
                derivatives[w[i]->id] = v[i];
                min_id = min_id > f[i]->id ? f[i]->id : min_id;
                grad_level = grad_level < (w[i]->grad_level + (unsigned int)(1)) ? (w[i]->grad_level + (unsigned int)(1)) : grad_level;
                // Remove w[i] from the flow_tree
                flow_tree[w[i]->id] = false;
            }

            // Generate all the directional derivatives
            for (auto i = min_id; i <= max_id; ++i) {
                if (flow_tree[i]) {
                    nodes[i]->op->forward_diff(derivatives);
                }
            }

            // Reset the grad level
            grad_level = 0;

            NodeVec outputs;
            for (auto i = 0; i < f.size(); ++i) {
                if(derivatives[f[i]->id].ptr.expired()){
                    switch (props.policies.independent_derivative){
                        case RAISE: {
                            op_logger("ForwardDiff")->error("The function f[{}] does not depend on any of the parameters (w)", i);
                            throw InvalidOperatorArgument(f, "ForwardDiff",
                                                          "The function f[" +
                                                          std::to_string(i) + "] does not depend on any of the parameters (w)");
                        }
                        case WARN:
                            op_logger("ForwardDiff")->warn("The function f[{}] does not depend on any of the parameters (w)", i);
                        default: ;
                    }
                    outputs.push_back(constant(0));
                } else {
                    outputs.push_back(derivatives[f[i]->id]);
                }
            }
            return outputs;
        }


        Node GraphInternal::derived_node(Operator op, std::string name) {
            Node same_node = find_same_node(op);
            if (same_node.ptr.expired()) {
                auto result = std::make_shared<NodeData>(
                        shared_from_this(),
                        nodes.size(),
                        name,
                        props.default_device,
                        op,
                        grad_level > op->get_grad_level() ? grad_level : op->get_grad_level(),
                        current_group
                );
                nodes.push_back(result);
                op->result = result;
                NodeVec ancestors = op->get_ancestors();
                for (int i = 0; i < ancestors.size(); i++) {
                    ancestors[i]->children.push_back(result);
                }
                // Add the node to the group map
                if(group_map.find(current_group) == group_map.end()){
                    group_map[current_group] = NodeVec{result};
                } else {
                    group_map[current_group].push_back(result);
                }
                // Add the node to the op map
                if(op->name != "Alias"){
                    if(op_map.find(op->name) == op_map.end()){
                        op_map[op->name] = NodeVec{result};
                    } else {
                        group_map[op->name].push_back(result);
                    }
                }
                return result;
            } else {
                return alias(same_node);
            }
        }

        void GraphInternal::update_node(Node shared, Node update) {
            throw NotImplementedError(__LINE__, __FILE__);
//            // Check the first node is a shared variable
//            if (shared->op->name != "Shared") {
//                auto err = std::make_shared<InvalidArguments>(NodeVec {shared, update},
//                                                              "Update",
//                                                              "First argument must be a SHARED_VARIABLE");
//                err->log(logger());
//                throw err;
//            }
//            auto shared_shape = shared->shape;
//            auto update_shape = update->shape;
//
//            // Check that the shapes are correct
//            for (int i = 0; i < 4; i++) {
//                if (shared_shape[i] != update_shape[i]) {
//                    auto err = std::make_shared<IncompatibleShapes>(NodeVec{shared, update}, "Update");
//                    err->log(logger());
//                    throw err;
//                }
//            }
//
//            // Check that the value types are correct
//            if (shared->data_type != update->data_type) {
//                auto err = std::make_shared<InvalidArguments>(NodeVec{shared, update},
//                                                              "Update",
//                                                              "The Shared variable and the update should have the same dtype");
//                err->log(logger());
//                throw err;
//            }
//            // Add it to the updates
//            updates.push_back(std::pair<Node, Node>(shared, update));
        }


//        Group GraphInternal::get_or_create_group(std::string full_name){
//            Group group = groups[0];
//            std::stringstream name_stream(full_name);
//            std::string item;
//            while (std::getline(name_stream, item, props.group_delimiter)) {
//                auto index = group.lock()->children.size();
//                for (auto i = 0; i < index; ++i) {
//                    if (item.compare(group.lock()->children[i].lock()->name) == 0) {
//                        index = i;
//                        break;
//                    }
//                }
//                // If not found make a new group
//                if (index == group.lock()->children.size()) {
//                    groups.push_back(std::make_shared<NodeGroup>(item, group));
//                    auto new_group = groups.back();
//                    group.lock()->children.push_back(new_group);
//                    logger()->trace("Creating group {} with parent {} and name {}",
//                                    new_group->full_name, group.lock()->name, item);
//                    group = new_group;
//                } else {
//                    group = group.lock()->children[index];
//                }
//            }
//            return group;
//        }

        void GraphInternal::set_group(std::string full_name){
            current_group = full_name;
        };

        void GraphInternal::push_group(std::string name){
            current_group += props.group_delimiter;
            current_group += name;
        }

        void GraphInternal::pop_group() {
            auto last_delimiter = current_group.find_last_of(props.group_delimiter);
            if(last_delimiter == std::string::npos){
                current_group = "";
            } else {
                current_group = current_group.substr(0, last_delimiter);
            }
        }

        void GraphInternal::reset_group(){
            current_group = "";
        }

        std::string GraphInternal::get_parent_group_name(std::string full_name){
            auto last_delimiter = full_name.find_last_of(props.group_delimiter);
            if(last_delimiter == std::string::npos){
                return "";
            } else {
                return full_name.substr(0, last_delimiter);
            }
        }
    }
}

