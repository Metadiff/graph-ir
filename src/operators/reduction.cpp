//
// Created by alex on 19/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{
        Node GraphInternal::sum(Node node, Axes axes){
            return op::apply_reduction<op::Sum>(this, node, axes);
        }

        Node GraphInternal::sum(Node node, int axis){
            auto a = auto_infer_axes(node);
            if(axis == auto_infer){
                return op::apply_reduction<op::Sum>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Sum>(this, node, axis);
        }

        Node GraphInternal::mean(Node node, Axes axes){
            return op::apply_reduction<op::Mean>(this, node, axes);
        }

        Node GraphInternal::mean(Node node, int axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::Mean>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Mean>(this, node, axis);
        }

        Node GraphInternal::prod(Node node, Axes axes){
            return op::apply_reduction<op::Product>(this, node, axes);
        }

        Node GraphInternal::prod(Node node, int axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::Product>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Product>(this, node, axis);
        }

        Node GraphInternal::all_true(Node node, Axes axes){
            return op::apply_reduction<op::AllTrue>(this, node, axes);
        }

        Node GraphInternal::all_true(Node node, int axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::AllTrue>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::AllTrue>(this, node, axis);
        }

        Node GraphInternal::any_true(Node node, Axes axes){
            return op::apply_reduction<op::AnyTrue>(this, node, axes);
        }

        Node GraphInternal::any_true(Node node, int axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::AnyTrue>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::AnyTrue>(this, node, axis);
        }
    }
}

