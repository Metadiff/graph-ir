//
// Created by alex on 19/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        Node GraphInternal::sum(Node node, Axes axes){
            return op::apply_reduction<op::Sum>(this, node, axes);
        }

        Node GraphInternal::sum(Node node, short axis){
            auto a = auto_infer_axes(node);
            if(axis == auto_infer){
                return op::apply_reduction<op::Sum>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Sum>(this, node, axis);
        }

        Node GraphInternal::mean(Node node, Axes axes){
            return op::apply_reduction<op::Mean>(this, node, axes);
        }

        Node GraphInternal::mean(Node node, short axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::Mean>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Mean>(this, node, axis);
        }

        Node GraphInternal::prod(Node node, Axes axes){
            return op::apply_reduction<op::Product>(this, node, axes);
        }

        Node GraphInternal::prod(Node node, short axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::Product>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::Product>(this, node, axis);
        }

        /** Reduction with operator AND */
        Node GraphInternal::all_true(Node node, Axes axes){
            return op::apply_reduction<op::AllTrue>(this, node, axes);
        }

        /** Reduction with operator AND */
        Node GraphInternal::all_true(Node node, short axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::AllTrue>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::AllTrue>(this, node, axis);
        }

        /** Reduction with operator OR */
        Node GraphInternal::any_true(Node node, Axes axes){
            return op::apply_reduction<op::AnyTrue>(this, node, axes);
        }

        /** Reduction with operator OR */
        Node GraphInternal::any_true(Node node, short axis){
            if(axis == auto_infer){
                return op::apply_reduction<op::AnyTrue>(this, node, auto_infer_axes(node));
            }
            return op::apply_reduction<op::AnyTrue>(this, node, axis);
        }
    }
}

