//
// Created by alex on 31/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node softplus(Node node, double threshold){
            Graph g = node->g();
            // Standard
            Operator op = std::make_shared<op::Softplus>(g.get(), node, threshold);
            return g->derived_node(op);
        }

        Node sigmoid(Node node) {
            Graph g = node->g();
            // Standard
            Operator op = std::make_shared<op::Sigmoid>(g.get(), node);
            return g->derived_node(op);
        }

        Node log_sum_exp(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            if (base->op->name == "Log") {
                // If the parent is a log return remove it
                auto cast_op = std::dynamic_pointer_cast<op::Log>(base->op);
                return log(sum(cast_op->parent, axes));
            }
            // Standard
            Operator op = std::make_shared<op::LogSumExp>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node log_sum_exp(Node node){
            return log_sum_exp(node, auto_infer_axes(node->shape));
        }

        Node log_sum_exp(Node node, int axis){
            return log_sum_exp(node, Axes{axis});
        }

        Node log_sum_exp(Node node, int axis0, int axis1){
            return log_sum_exp(node, Axes{axis0, axis1});
        }

        Node log_sum_exp(Node node, int axis0, int axis1, int axis2){
            return log_sum_exp(node, Axes{axis0, axis1, axis2});
        }

        Node softmax(Node node, Axes axes){
            if(node.dims() == 0){
                // TODO throw proper error
                utils::op_logger("Softmax")->error("Can not work on scalars");
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            // Standard
            Operator op = std::make_shared<op::Softmax>(g.get(), node, axes);
            return g->derived_node(op);

        }

        Node softmax(Node node){
            int axis = 3;
            for(auto i=2; i>=0; --i){
                if(node->shape[i] == 1){
                    axis = i;
                } else {
                    break;
                }
            }
            return softmax(node, Axes{axis});
        }

        Node softmax(Node node, int axis){
            if(axis == -1){
                return softmax(node, auto_infer_axes(node->shape));
            } else {
                return softmax(node, Axes{axis});
            }
        }

        Node softmax(Node node, int axis0, int axis1){
            return softmax(node, Axes{axis0, axis1});
        }

        Node softmax(Node node, int axis0, int axis1, int axis2){
            return softmax(node, Axes{axis0, axis1, axis2});
        }
    }
}
