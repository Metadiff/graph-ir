//
// Created by alex on 18/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node cast(Node node, DataType data_type) {
            Graph g = node->g();
            // If same data_type do nothing
            if(node->data_type == data_type){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::Cast>(g.get(), node, data_type);
            return g->derived_node(op);
        }

        Node alias(Node node) {
            Graph g = node->g();
            // Standard
            Operator op = std::make_shared<op::Alias>(g.get(), node);
            return g->derived_node(op);
        }

        Node broadcast(Node node, Shape shape) {
            Graph g = node->g();
            // If same shape do nothing
            if(node->shape == shape){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::Broadcast>(g.get(), node, shape);
            return g->derived_node(op);
        }

        Node make_constant(Node node){
            Graph g = node->g();
            // If already a constant do nothing
            if(not node->is_differentiable){
                return alias(node);
            }
            // Standard
            Operator op = std::make_shared<op::MakeConstant>(g.get(), node);
            return g->derived_node(op);
        }

        Node select(Node condition, Node if_true, Node if_false){
            Graph g = condition->g();
            std::string const op_name = "Select";
            // Check the nodes are from the same graph
            if(g != if_true->g() or g != if_false->g()){
                // TODO make an exception
                throw 1;
            }
            // If the two nodes are symbolically equal we don't need this
            if(core::symbolic_equals(if_true, if_false)){
                return alias(if_true);
            }
            // Verify that the condition is boolean
            if (condition->data_type != b8) {
                auto err = std::make_shared<InvalidArguments>
                        (NodeVec{condition, if_true, if_false},
                         op_name, "Calling Select with condition of type "
                         + to_string(condition->data_type));
                operate_policy(g->props.policies.cast, g->op_logger(op_name), err);
                condition = cast(condition, b8);
            }
            // Verify the shapes of the inputs
            NodeVec corrected = verify_shapes_and_broadcast({condition, if_true, if_false}, op_name);
            // Standard
            Operator op = std::make_shared<op::Select>(g.get(), corrected[0], corrected[1], corrected[2]);
            return g->derived_node(op);
        }
    }
}
