//
// Created by alex on 03/10/16.
//

#include "graph_ir.h"
#include "sstream"

namespace md{
    namespace gir{
        GraphFunction::GraphFunction(Graph const full_graph,
                                     std::vector<Node> inputs,
                                     std::vector<Node> outputs,
                                     Updates extra_updates,
                                     bool copy_updates) {
            std::vector<Node> leafs = outputs;
            // Add the inputs to the leafs
            leafs.insert(leafs.end(), inputs.begin(), inputs.end());
            // Add the updates of the graph
            for(auto it=full_graph->updates.begin(); it != full_graph->updates.end();  ++it){
                leafs.push_back((*it).second);
            }
            // Add the extra updates
            for(auto it=extra_updates.begin(); it != extra_updates.end();  ++it){
                leafs.push_back(it->first);
                leafs.push_back(it->second);
            }
            // Get ancestor mask of all of the outputs
            auto ancestor_mask = full_graph->get_ancestors_mask(leafs);
            // Check that all of the inputs are provided
            auto all_inputs = full_graph->op_map["Input"];
            for(auto i=0; i<all_inputs.size(); ++i){
                if(ancestor_mask[all_inputs[i]->id]) {
                    auto r = std::find_if(inputs.begin(), inputs.end(), [=](const Node &n) { return n->id == i;});
                    if (r == inputs.end()) {
                        g_logger(full_graph->name)->error(
                                "The outputs requested require the input {} which has not been provided",
                                to_string(full_graph->nodes[i]));
                        throw InvalidOperatorArgument(NodeVec{}, "MakeFunction",
                                                      "The outputs requested require the input " +
                                                      to_string(full_graph->nodes[i]) + " which has not been provided.");
                    }
                }
            }
            graph = api::create_graph();
            auto mapping = full_graph->copy_into(graph, ancestor_mask, Updates{}, true, true, copy_updates);
            // Add the extra updates
            for(auto it=extra_updates.begin(); it != extra_updates.end();  ++it){
                api::update(mapping[it->first], mapping[it->second]);
            }
            // Convert outputs and inputs
            for(auto i=0; i<outputs.size(); ++i){
                this->outputs.push_back(mapping[outputs[i]]);
            }
            for(auto i=0; i<outputs.size(); ++i){
                this->inputs.push_back(mapping[inputs[i]]);
            }
        }

        std::vector<Node> GraphFunction::apply(Graph other_graph, std::vector<Node> provided_inputs,
                                               bool apply_updates) {
            if(provided_inputs.size() != inputs.size()){
                g_logger(other_graph->name)->error("Incorrect number of provided inputs");
                throw InternalGraphError("FunctionApply", "Incorrect number of provided inputs");
            }
            std::vector<bool> flow_tree;
            if(apply_updates){
                flow_tree = std::vector<bool>(graph->nodes.size(), true);
            } else {
                flow_tree = graph->get_flow_tree_mask(inputs, outputs);
            }
            Updates provided;
            for(auto i=0; i<inputs.size(); ++i){
                provided[inputs[i]] = provided_inputs[i];
            }
            auto mapping = graph->apply(other_graph, flow_tree, provided, false, apply_updates);
            std::vector<Node> outputs;
            for(auto i=0; i<outputs.size(); ++i){
                outputs.push_back(mapping[outputs[i]]);
            }
            return outputs;
        }


        DataType GraphInternal::limit_type(DataType data_type) const {
            if(data_type.type < FLOAT){
                if(data_type.precision > props.max_int){
                    return DataType(data_type.type, props.max_int);
                } else {
                    return data_type;
                }
            } else {
                if(data_type.precision > props.max_float){
                    return DataType(data_type.type, props.max_float);
                } else {
                    return data_type;
                }
            }
        }

        Updates GraphInternal::copy_into(Graph new_graph, std::vector<bool> const & mask,
                                         Updates const & provided,
                                         bool allow_input_copies, bool allow_shared_copies,
                                         bool copy_updates) const {
            // Verify that the left nodes in the mapped are from this graph and the right are from the new
            for(auto it = provided.begin(); it != provided.end(); ++it){
                if(it->first.g().get() != this){
                    g_logger(name)->error("A source node from the provided nodes is not part of this graph.");
                    throw InternalGraphError("CopyInto", "A source node from the provided nodes is not part of this graph.");
                } else if(it->second.g() != new_graph){
                    g_logger(name)->error("A target node from the provided nodes is not part of this graph.");
                    throw InternalGraphError("CopyInto", "A target node from the provided nodes is not part of this graph.");
                }
            }
            g_logger(name)->trace("Copying into graph {}", new_graph->name);
            size_t n = nodes.size();
            // Variable which maps each node (by id) of the original graph to a Node in the new graph
            Updates mapping;
            // Copy nodes
            for (auto i = 0; i < n; i++) {
                // Only nodes that are masked
                if (mask[i]) {
                    g_logger(name)->trace("Copying node {} resulting in node {}", i, new_graph->nodes.size());
                    // Check if this is a mapped node
                    auto it = provided.find(nodes[i]);
                    if(it != provided.end()){
                        mapping[nodes[i]] = it->second;
                        continue;
                    }
                    // Check if it is an input node
                    if(nodes[i]->op->name == "Input" and not allow_input_copies){
                        g_logger(name)->error("The input node {} did not had a provided mapped value during copying"
                                                      "and allow_input_copies=false.", i);
                        throw InternalGraphError("CopyInto", "The input node "
                                                             + std::to_string(i)
                                                             + " did not had a provided mapped value during copying"
                                                                     "and allow_input_copies=false.");
                    } else if(nodes[i]->op->name == "SharedInput" and not allow_shared_copies){
                        g_logger(name)->error("The shared node {} did not had a provided mapped value during copying "
                                                      "and allow_shared_copies=false.", i);
                        throw InternalGraphError("CopyInto", "The shared node "
                                                             + std::to_string(i)
                                                             + " did not had a provided mapped value during copying "
                                                                     "and allow_shared_copies=false.");
                    }
                    // Get all of the ancestors of the node and find their corresponding nodes
                    // in the new graph
                    NodeVec ancestors = nodes[i]->op->get_ancestors();
                    NodeVec new_ancestors;
                    for (size_t j = 0; j < ancestors.size(); j++) {
                        if(mapping[ancestors[j]].ptr.expired()){
                            g_logger(name)->error("Attempted to copy node {} with ancestor {}, but the parent "
                                                          "was not part of the mask.",
                                                  i, ancestors[j]->id);
                            throw InternalGraphError("Copy", "Attempted to copy node " + std::to_string(i)
                                                             + " with ancestor " + std::to_string(ancestors[j]->id)
                                                             + ", but the parent was not part of the mask.");
                        }
                        new_ancestors.push_back(mapping[ancestors[j]]);
                    }
                    // Copy the node using the new ancestors and put it in the mapping
                    Operator op = nodes[i]->op->copy_to(new_graph.get(), new_ancestors);
                    mapping[nodes[i]] = new_graph->derived_node(op, nodes[i]->name);
                    mapping[nodes[i]]->scope = nodes[i]->scope;
                    mapping[nodes[i]]->grad_level = nodes[i]->grad_level;
                }
            }
            if(copy_updates) {
                // Copy the updates, by just adding the corresponding nodes
                for (auto it = updates.begin(); it != updates.end(); ++it) {
                    auto r1 = mapping.find(it->first);
                    auto r2 = mapping.find(it->second);
                    if(r1 == mapping.end() or r2 == mapping.end()) {
                        g_logger(name)->error("Could not copy the update for node {} "
                                                      " because it was not part of the mask.", it->first->id);
                        throw InternalGraphError("Copy", "Could not copy the update for node "
                                                         + std::to_string(it->first->id)
                                                         + " because it was not part of the mask.");
                    } else {
                        api::update(mapping[r1->second], mapping[r2->second]);
                    }
                }
            }
            g_logger(name)->trace("Copy completed");
            return mapping;
        }

        Graph GraphInternal::clone(std::vector<bool> const & mask, bool copy_updates) const{
            auto new_graph = create_graph();
            new_graph->name = name + "_clone";
            new_graph->props = props;
            copy_into(new_graph, mask, Updates{} ,true, true, copy_updates);
            new_graph->scope = scope;
            new_graph->grad_level = grad_level;
            return new_graph;
        }

        Graph GraphInternal::clone(bool copy_updates) const{
            std::vector<bool> mask(nodes.size(), true);
            return clone(mask, copy_updates);
        }

        Updates GraphInternal::apply(Graph new_graph, std::vector<bool> const & mask,
                                     Updates const & provided,
                                     bool copy_updates, bool allow_shared_copies) const{
            return copy_into(new_graph, mask, provided ,false, allow_shared_copies, copy_updates);
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
            op_logger("BackwardDiff")->trace("Finished BackwardDiff");
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

            op_logger("ForwardDiff")->trace("Finished ForwardDiff");

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
                        scope
                );
                nodes.push_back(result);
                op->result = result;
                NodeVec ancestors = op->get_ancestors();
                for (int i = 0; i < ancestors.size(); i++) {
                    ancestors[i]->children.push_back(result);
                }
                // Add the node to the group map
                if(group_map.find(scope) == group_map.end()){
                    group_map[scope] = NodeVec{result};
                } else {
                    group_map[scope].push_back(result);
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

        void GraphInternal::set_scope(std::string full_name){
            scope = full_name;
        };

        void GraphInternal::push_scope(std::string name){
            scope += props.scope_delimiter;
            scope += name;
        }

        void GraphInternal::pop_scope() {
            auto last_delimiter = scope.find_last_of(props.scope_delimiter);
            if(last_delimiter == std::string::npos){
                scope = "";
            } else {
                scope = scope.substr(0, last_delimiter);
            }
        }

        void GraphInternal::reset_scope(){
            scope = "";
        }

        std::string GraphInternal::get_parent_group_name(std::string full_name){
            auto last_delimiter = full_name.find_last_of(props.scope_delimiter);
            if(last_delimiter == std::string::npos){
                return "";
            } else {
                return full_name.substr(0, last_delimiter);
            }
        }


    }
}

