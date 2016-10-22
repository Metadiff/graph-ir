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
                return data_type <= max_float ? data_type : max_float;
            } else if(data_type > u64){
                // It is an int
                return data_type <= max_int ? data_type : max_int;
            } else{
                // It is an uint
                switch (max_int){
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

        bool GraphInternal::is_temporary_constant(Node node) const {
            std::shared_ptr<NodeData> ptr = node.unwrap();
            for (size_t i = 0; i < temporary_constants.size(); i++) {
                if (temporary_constants[i].unwrap() == ptr) {
                    return true;
                }
            }
            return false;
        }

        NodeVec GraphInternal::copy(GraphInPtr new_graph, std::vector<bool> mask) const {
            logger()->trace("Copying graph {}", name);
            new_graph->name = name + "_copy";
            new_graph->default_device = default_device;
            new_graph->max_float = max_float;
            new_graph->max_int = max_int;
            for(auto i=0; i<13; ++i){
                for(auto j=0;j<13; ++j){
                    new_graph->promotion_table[i][j] = promotion_table[i][j];
                }
            }
            new_graph->broadcast_err_policy = broadcast_err_policy;
            new_graph->promotion_err_policy = promotion_err_policy;
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
//        std::regex ids("\\s[[:digit:]]+(?=[\\|-])");
//        std::vector<size_t> positions, lengths;
//        for(size_t i=0;i<new_graph->nodes.size();i++){
//            if(new_graph->nodes[i]->grad_level > 0) {
//                Node node = new_graph->nodes[i];
//                for(std::sregex_iterator s = std::sregex_iterator(node.unwrap()->name.begin(), node.unwrap()->name.end(), ids);
//                    s != std::sregex_iterator();
//                    ++s )
//                {
//                    std::smatch m = *s;
//                    positions.push_back(m.position());
//                    lengths.push_back(m.length());
//                }
//                int diff = 0;
//                for(size_t s=0;s<positions.size();s++){
//                    std::string new_id = " " + std::to_string(mapping[node.unwrap()->id].unwrap()->id);
//                    node.unwrap()->name.replace(positions[i]+diff, lengths[i], new_id);
//                    diff += new_id.length()  - lengths[i];
//                }
//                positions.clear();
//                lengths.clear();
//            }
//        }
            logger()->trace("Copy completed");
            return mapping;
        }

        std::vector<bool> GraphInternal::get_descendants_mask(NodeVec marked) const {
            logger()->trace("Generating descendants mask");
            auto n = nodes.size();
            std::vector<bool> descendants_mask(n, false);

            // Mark the nodes provided
            for (int i = 0; i < marked.size(); i++) {
                descendants_mask[marked[i]->id] = true;
            }

            // At each iteration if the node has been marked, mark its direct children
            for (int i = 0; i < n; i++) {
                if (descendants_mask[i]) {
                    auto children = nodes[i]->children;
                    for (int j = 0; j < children.size(); j++) {
                        descendants_mask[children[j]->id] = true;
                    }
                }
            }
            return descendants_mask;
        };

        std::vector<bool> GraphInternal::get_ancestors_mask(NodeVec marked) const {
            logger()->trace("Generating ancestors mask");
            auto n = nodes.size();
            std::vector<bool> ancestors_mask(n, false);

            // Mark the nodes provided
            for (int i = 0; i < marked.size(); i++) {
                ancestors_mask[marked[i]->id] = true;
            }

            // At each iteration if the node has been marked, mark its direct ancestors
            for (size_t i = n - 1; i < n; i--) {
                if (ancestors_mask[i]) {
                    NodeVec ancestors = nodes[i]->op->get_ancestors();
                    for (int j = 0; j < ancestors.size(); j++) {
                        ancestors_mask[ancestors[j]->id] = true;
                    }
                }
            }
            return ancestors_mask;
        };

        /**
         * TODO have to do this function properly
         */
        Node GraphInternal::find_same_node(Operator op) {
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
//        NodeVec candidates = op->get_parents()
//        for(int i=0; i<op->get_parents()[0].unwrap().chilidren)
//        // For the moment only one such for the experiment
//        if(op->name == "Exp"){
//            Node parent = op->get_parents()[0];
//            for(int i=0;i<parent.unwrap()->children.size(); i++){
//                Node child = parent.unwrap()->children[i];
//                if(child.unwrap()->op->name == "Exp"){
//                    return child.alias();
//                }
//            }
//            for(int i=0;i<parent.unwrap()->children.size(); i++){
//                Node child = parent.unwrap()->children[i];
//                if(child.unwrap()->op->name == "Neg"){
//                    for(int j=0;j<child.unwrap()->children.size();j++){
//                        Node neg_child = child.unwrap()->children[j];
//                        if(neg_child.unwrap()->op->name == "Exp"){
//                            return neg_child.div();
//                        }
//                    }
//                }
//            }
//            if(parent.unwrap()->op->name == "Neg"){
//                Node gparent = parent.unwrap()->op->get_parents()[0];
//                for(int i=0;i<gparent.unwrap()->children.size(); i++){
//                    Node child = gparent.unwrap()->children[i];
//                    if(child.unwrap()->op->name == "Exp"){
//                        return child.div();
//                    }
//                }
//            }
//        }
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

        std::vector<Node> GraphInternal::gradient(Node objective, std::vector<Node> params) {
            logger()->trace("Getting gradients of {}", objective->id);
            if (objective.dims() != 0) {
                auto err = std::make_shared<UnsupportedGradient>(objective);
                err->log(logger());
                throw err;
            }

            // Extract the flow tree between params and objective
            std::vector<bool> descendants_mask = get_descendants_mask(params);
            std::vector<bool> ancestors_mask = get_ancestors_mask(NodeVec{objective});
            std::vector<Node> flow_tree;
            // Add all required nodes to the flow_tree and the rest to be constants
            for (size_t i = 0; i < nodes.size(); i++) {
                if (ancestors_mask[i] and descendants_mask[i]) {
                    flow_tree.push_back(nodes[i]);
                } else {
                    temporary_constants.push_back(nodes[i]);
                }
            }
            // The gradient mode is one higher than that of the objective
            grad_level = objective->grad_level + ((unsigned short)(1));

            // Stores the current group in order to recreate it
            Group old_current = current_group;
            Group old_base = current_base_group;

            current_base_group = get_or_create_base_group("Grads " + std::to_string(grad_level));
            current_group = current_base_group;
            // Set the current group to the corresponding gradients group

            // This vector will contain at each index the gradient message to the corresponding node
            std::vector<Node> grad_messages(nodes.size(), Node());
            // Send the first message as 1 to the objective
//            logger()->info("nodes: {}, msgs: {}, oid: {}", nodes.size(), grad_messages.size(), objective->id);
            grad_messages[objective->id] = constant(1);
            // Send all gradient messages
            for (size_t i = flow_tree.size(); i > 0; i--) {
                if (not grad_messages[flow_tree[i - 1]->id].ptr.expired()) {
                    flow_tree[i - 1]->op->generate_gradients(grad_messages);
                }
            }

            // Reset the gradient mode
            grad_level = 0;

            // Remove all nodes from the temporary constants
            temporary_constants.clear();

            // Restore the current group
            current_group = old_current;
            current_base_group = old_base;

            // Extract the gradients for each parameter
            std::vector<Node> grads;
            for (int i = 0; i < params.size(); i++) {
                grads.push_back(grad_messages[params[i]->id]);
            }

            return grads;
        };

//        // Copies the graph and optimizes it, populating the execution data
//        Graph optimize(NodeVec &targets, Updates &updates, NodeVec &inputs,
//                                      NodeVec &new_targets, Updates &new_updates, NodeVec &new_inputs) {
//            logger()->debug("Running optimization of graph {}", name);
//            // Copy only the relevant part of the graph
//            Graph copy = create_graph();
//            add_temporary_updates(updates);
//            NodeVec marked(targets.size() + this->updates.size() + this->temporary_updates.size());
//            for (size_t i = 0; i < targets.size(); i++) {
//                marked[i] = targets[i];
//            }
//            for (size_t i = 0; i < this->updates.size(); i++) {
//                marked[i + targets.size()] = this->updates[i].second;
//            }
//            for (size_t i = 0; i < this->temporary_updates.size(); i++) {
//                marked[i + targets.size()] = this->temporary_updates[i].second;
//            }
//
//            NodeVec mapping = this->copy(copy.get(), get_ancestors_mask(marked));
//            clear_temporary_updates();
//            // Optimize
//            for (size_t i = 0; i < copy->nodes.size(); i++) {
//                Node node = copy->nodes[i];
////                if (node->op->name == "Input") {
////                    node->execution.inlined = true;
////                }
////                if (node->op->name == "Shared") {
////                    node->execution.inlined = true;
////                }
////                if (node.is_scalar() and node.is_constant()) {
////                    node->execution.inlined = true;
////                }
////                if (node->op->name == "Broadcast") {
////                    node->execution.inlined = true;
////                }
////                if (node->op->name == "Transpose") {
////                    node->execution.inlined = true;
////                }
////                if (node->op->name == "Neg") {
////                    node->execution.inlined = true;
////                }
////                if (node->children.size() <= 1) {
////                    node->execution.inlined = true;
////                }
//            }
//            // Set the new_targets and new_updates
//            for (int i = 0; i < targets.size(); i++) {
//                new_targets.push_back(mapping[targets[i]->id]);
//            }
//            for (int i = 0; i < updates.size(); i++) {
//                Node node1 = mapping[updates[i].first->id];
//                Node node2 = mapping[updates[i].second->id];
//                new_updates.push_back(std::pair<Node, Node>(node1, node2));
//            }
//            for (int i = 0; i < inputs.size(); i++) {
//                new_inputs.push_back(mapping[inputs[i]->id]);
//            }
//            return copy;
//        };

//        Node GraphInternal::shared_var(af::array value, std::string name) {
//            SharedPtr shared = std::make_shared<SharedVariable>(shared_vars.size(), value);
//            shared_vars.push_back(shared);
//            std::shared_ptr<Operator> op = std::make_shared<SharedInput>(this, shared);
//            Node node = derived_node(op);
//            node->name = name;
//            return node;
//        };
//        dType dtype;
//
//
//        if(value.type() == af::dtype::b8){
//            dtype = BOOLEAN;
//        } else if(value.type() == af::dtype::f32
//                  or value.type() == af::dtype::f64){
//            dtype = FLOAT;
//        } else {
//            dtype = UNSIGNED_INT;
//        }
//        af::dim4 dims = value.dims();
//
//        Node node = derived_node(op);
//
//        std::shared_ptr<NodeData> result = std::make_shared<NodeData>(
//                shared_from_this().get(),
//                default_device,
//                nodes.size(),
//                name,
//                ad_node_type::INPUT,
//                dtype,
//                Shape {dims[0], dims[1], dims[2], dims[3]},
//                std::make_shared<Input>(shared_from_this().get()),
//                0,
//                current_group
//        );
//        result->shared = std::make_shared<SharedVariable>(shared_vars.size(), value);
//        shared_vars.push_back(result->shared);
//        nodes.push_back(result);
//        result->op->owner = result;
//        return result;
//        };

        Node GraphInternal::derived_node(Operator op, std::string name) {
            Node same_node = find_same_node(op);
            if (same_node.ptr.expired()) {
//            std::cout << "Creating new derived node for operator " << op->name << std::endl;
//                if(op->name == "Tanh"){
//                    std::cout << op->get_shape() << " " << op->get_parents()[0]->op->name <<
//                    op->get_parents()[0]->shape << std::endl;
//                }
                auto result = std::make_shared<NodeData>(
                        shared_from_this().get(),
                        nodes.size(),
                        name,
                        default_device,
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

//    Node GraphInternal::constant_node(af::array value){
//        dType dtype;
//        if(value.type() == af::dtype::b8){
//            dtype = BOOLEAN;
//        } else if(value.type() == af::dtype::f32
//                  or value.type() == af::dtype::f64){
//            dtype = FLOAT;
//        } else {
//            dtype = INTEGER;
//        }
//        af::dim4 dims = value.dims();
//        std::shared_ptr<NodeData> result = std::make_shared<NodeData>(
//                shared_from_this().get(),
//                default_device,
//                nodes.size(),
//                "Constant Node",
//                ad_node_type::CONSTANT,
//                dtype,
//                Shape {dims[0], dims[1], dims[2], dims[3]},
//                std::make_shared<Input>(shared_from_this().get()),
//                0,
//                current_group
//        );
//        result->value = value;
//        nodes.push_back(result);
//        result->op->owner = result;
//        return result;
//    };

        SymInt GraphInternal::get_new_symbolic_integer() const {
            return SymInt::registry->new_variable();
        }

        Group GraphInternal::get_or_create_base_group(std::string name){
            for(auto i=0; i < base_groups.size(); ++i){
                if(base_groups[i]->full_name == name){
                    return base_groups[i];
                }
            }
            base_groups.push_back(std::make_shared<NodeGroup>(name, std::weak_ptr<NodeGroup>()));
            return base_groups.back();
        }

        void GraphInternal::set_base_group(std::string name){
            current_base_group = get_or_create_base_group(name);
        }

        void GraphInternal::reset_base_group(){
            current_base_group = base_groups[0];
        }

        Group GraphInternal::get_or_create_group_from_base(std::string full_name, Group base_group){
            Group group = base_group;
            std::stringstream name_stream(full_name);
            std::string item;
            while (std::getline(name_stream, item, utils::props()->group_delimiter)) {
                size_t index = group.lock()->children.size();
                for (size_t i = 0; i < index; i++) {
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

        Group GraphInternal::get_or_create_group_from_base(std::string full_name, std::string base_group){
            return get_or_create_group_from_base(full_name, get_or_create_base_group(base_group));
        }
        Group GraphInternal::get_or_create_group_from_base(std::string full_name){
            return get_or_create_group_from_base(full_name, current_base_group);
        }

        Group GraphInternal::get_or_create_group_from_parent(std::string name, Group parent){
            auto p = parent.lock();
            for(auto i=0; i<p->children.size(); ++i){
                if(p->children[i].lock()->name == name){
                    return p->children[i];
                }
            }
            groups.push_back(std::make_shared<NodeGroup>(name, parent));
            p->children.push_back(groups.back());
            return groups.back();
        }

        Group GraphInternal::get_or_create_group_from_parent(std::string name, std::string parent){
            return get_or_create_group_from_parent(name, get_or_create_group_from_base(parent));
        }

        Group GraphInternal::get_or_create_group(std::string name){
            return get_or_create_group_from_parent(name, current_group);
        }

        void GraphInternal::set_group_from_base(std::string full_name, std::string base_group){
            current_group = get_or_create_group_from_base(full_name, base_group);
        }

        void GraphInternal::set_group_from_base(std::string full_name){
            current_group = get_or_create_group_from_base(full_name, current_base_group);
        }

        void GraphInternal::set_group_from_parent(std::string name, Group parent){
            current_group = get_or_create_group_from_parent(name, parent);
        }

        void GraphInternal::set_group_from_parent(std::string name, std::string parent){
            current_group = get_or_create_group_from_parent(name, parent);
        }

        void GraphInternal::set_group(std::string name){
            current_group = get_or_create_group_from_parent(name, current_group);
        }
    }
}

