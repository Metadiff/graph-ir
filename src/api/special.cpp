//
// Created by alex on 18/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{

        Node cast(Node node, DataType data_type) {
            Graph g = node.g();
            // If same data_type do nothing
            if(node->data_type == data_type){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::Cast>(g.get(), node, data_type);
            return g->derived_node(op);
        }

        Node alias(Node node) {
            Graph g = node.g();
            // Standard
            Operator op = std::make_shared<op::Alias>(g.get(), node);
            return g->derived_node(op);
        }

        Node broadcast(Node node, Shape shape) {
            Graph g = node.g();
            // If same shape do nothing
            if(node->shape == shape){
                return alias(node);
            }
            // Verify broadcasting is possible
            for(auto i=0; i<4; ++i){
                if(node->shape[i] != shape[i] and node->shape[i] != 1){
                    op_logger("Broadcast")->error("Can not broadcast shape {} to shape {}",
                                                  to_string(node->shape), to_string(shape));
                    throw InvalidOperatorArgument(NodeVec{node}, "Broadcast",
                                                  "Can not broadcast shape " + to_string(node->shape) +
                                                  " to shape " + to_string(shape));
                }
            }
            // Standard
            Operator op = std::make_shared<op::Broadcast>(g.get(), node, shape);
            return g->derived_node(op);
        }

        Node make_constant(Node node){
            Graph g = node.g();
            // If already a constant do nothing
            if(not node->is_differentiable){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::MakeConstant>(g.get(), node);
            return g->derived_node(op);
        }

        void update(Node shared, Node update){
            Graph g = shared.g();
            if(g != update.g()){
                op_logger("Update")->error("Shared and update are not part of the same graph.");
                throw InvalidOperatorArgument(NodeVec{shared, update}, "Update",
                                              "Shared and update are not part of the same graph.");
            }
            // Check that shared is a SharedInput
            if(shared->op->name != "SharedInput"){
                op_logger("Update")->error("The variable provided for updating is not a SharedInput.");
                throw InvalidOperatorArgument(NodeVec{shared, update}, "Update",
                                              "The variable provided for updating is not a SharedInput.");
            }
            // Check that Shared does not already have an update
            if(g->updates.find(shared->id) != g->updates.end()){
                op_logger("Update")->error("Trying to update a shared variable which already has an update rule.");
                throw InvalidOperatorArgument(NodeVec{shared, update}, "Update",
                                              "Trying to update a shared variable which already has an update rule.");
            }
            // Broadcast if needed
            update = implicit_broadcast(update, shared->shape, "Update");
            // Add to the updates
            g->updates[shared->id] = update;
        }

        Node select(Node condition, Node if_true, Node if_false){
            Graph g = condition.g();
            std::string const op_name = "Select";
            // Check the nodes are from the same graph
            if(g != if_true.g() or g != if_false.g()){
                op_logger("Select")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{condition, if_true, if_false},
                                              "Select", "The input variables are not from the same graph.");
            }
            // If the two nodes are symbolically equal we don't need this
            if(gir::symbolic_equals(if_true, if_false)){
                return alias(if_true);
            }
            // Verify that the condition is boolean
            if (condition->data_type != b8) {
                condition = implicit_cast(condition, b8, "Select");
            }
            // Verify the shapes of the inputs
            NodeVec corrected = {condition, if_true, if_false};
            verify_shapes_and_broadcast(corrected, op_name);
            // Standard
            Operator op = std::make_shared<op::Select>(g.get(), corrected[0], corrected[1], corrected[2]);
            return g->derived_node(op);
        }
    }
}
