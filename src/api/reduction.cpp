//
// Created by alex on 19/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node sum(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            // The sum(sum(x,a),b) = sum(x, a U b)
            if (base->op->name == "Sum") {
                auto cast_op = std::dynamic_pointer_cast<op::Sum>(base->op);
                for(auto i=0; i < cast_op->axes.size(); ++i){
                    axes.push_back(cast_op->axes[i]);
                }
                return sum(base->op->get_parents()[0], axes);
            }
            // Standard
            Operator op = std::make_shared<op::Sum>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node sum(Node node){
            return sum(node, auto_infer_axes(node->shape));
        }

        Node sum(Node node, int axis){
            return sum(node, Axes{axis});
        }

        Node sum(Node node, int axis0, int axis1){
            return sum(node, Axes{axis0, axis1});
        }

        Node sum(Node node, int axis0, int axis1, int axis2){
            return sum(node, Axes{axis0, axis1, axis2});
        }

        Node mean(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            // The mean(mean(x,a),b) = mean(x, a U b)
            if (base->op->name == "Mean") {
                auto cast_op = std::dynamic_pointer_cast<op::Mean>(base->op);
                for(auto i=0; i < cast_op->axes.size(); ++i){
                    axes.push_back(cast_op->axes[i]);
                }
                return mean(base->op->get_parents()[0], axes);
            }
            // Standard
            Operator op = std::make_shared<op::Mean>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node mean(Node node){
            return mean(node, auto_infer_axes(node->shape));
        }

        Node mean(Node node, int axis){
            return mean(node, Axes{axis});
        }

        Node mean(Node node, int axis0, int axis1){
            return mean(node, Axes{axis0, axis1});
        }

        Node mean(Node node, int axis0, int axis1, int axis2){
            return mean(node, Axes{axis0, axis1});
        }

        Node var(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            // Standard
            Operator op = std::make_shared<op::Variance>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node var(Node node){
            return mean(node, auto_infer_axes(node->shape));
        }

        Node var(Node node, int axis){
            return mean(node, Axes{axis});
        }

        Node var(Node node, int axis0, int axis1){
            return mean(node, Axes{axis0, axis1});
        }

        Node var(Node node, int axis0, int axis1, int axis2){
            return mean(node, Axes{axis0, axis1});
        }

        Node prod(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            if (base->op->name == "Product") {
                // The prod(prod(x,a),b) = prod(x, a U b)
                auto cast_op = std::dynamic_pointer_cast<op::Product>(base->op);
                for(auto i=0; i < cast_op->axes.size(); ++i){
                    axes.push_back(cast_op->axes[i]);
                }
                return prod(base->op->get_parents()[0], axes);
            }
            // Standard
            Operator op = std::make_shared<op::Product>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node prod(Node node){
            return prod(node, auto_infer_axes(node->shape));
        }

        Node prod(Node node, int axis){
            return prod(node, Axes{axis});
        }

        Node prod(Node node, int axis0, int axis1){
            return prod(node, Axes{axis0, axis1});
        }

        Node prod(Node node, int axis0, int axis1, int axis2){
            return prod(node, Axes{axis0, axis1, axis2});
        }

        Node all_true(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            if(node->data_type != b8){
                // Node should be b8
                node = cast_or_throw(node, b8, "AllTrue");
            } else if (base->op->name == "AllTrue") {
                // The all_true(all_true(x,a),b) = all_true(x, a U b)
                auto cast_op = std::dynamic_pointer_cast<op::AllTrue>(base->op);
                for(auto i=0; i < cast_op->axes.size(); ++i){
                    axes.push_back(cast_op->axes[i]);
                }
                return all_true(base->op->get_parents()[0], axes);
            }
            // Standard
            Operator op = std::make_shared<op::AllTrue>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node all_true(Node node){
            return all_true(node, auto_infer_axes(node->shape));
        }

        Node all_true(Node node, int axis){
            return all_true(node, Axes{axis});
        }

        Node all_true(Node node, int axis0, int axis1){
            return all_true(node, Axes{axis0, axis1});
        }

        Node all_true(Node node, int axis0, int axis1, int axis2){
            return all_true(node, Axes{axis0, axis1, axis2});
        }

        Node any_true(Node node, Axes axes){
            // If no axes do nothing
            if(axes.size() == 0){
                return alias(node);
            }
            // Validate axes
            axes = validate_axes(node->shape, axes);
            // Verify correctness
            Graph g = node->g();
            auto base = get_base_node(node);
            if(node->data_type != b8){
                // Node should be b8
                node = cast_or_throw(node, b8, "AnyTrue");
            } else if (base->op->name == "AnyTrue") {
                // The all_true(all_true(x,a),b) = all_true(x, a U b)
                auto cast_op = std::dynamic_pointer_cast<op::AnyTrue>(base->op);
                for(auto i=0; i < cast_op->axes.size(); ++i){
                    axes.push_back(cast_op->axes[i]);
                }
                return any_true(base->op->get_parents()[0], axes);
            }
            // Standard
            Operator op = std::make_shared<op::AnyTrue>(g.get(), node, axes);
            return g->derived_node(op);
        }

        Node any_true(Node node){
            return all_true(node, auto_infer_axes(node->shape));
        }

        Node any_true(Node node, int axis){
            return any_true(node, Axes{axis});
        }

        Node any_true(Node node, int axis0, int axis1){
            return any_true(node, Axes{axis0, axis1});
        }

        Node any_true(Node node, int axis0, int axis1, int axis2){
            return any_true(node, Axes{axis0, axis1, axis2});
        }
    }
}

