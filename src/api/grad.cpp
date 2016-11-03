//
// Created by alex on 27/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{
        NodeVec gradient(Node const f, NodeVec const & w) {
            // If no parameters return empty vector as well
            if(w.size() == 0){
                return NodeVec();
            }
            Graph g = f.g();
            // Verify that the objective is a scalar
            if (f.order() != 0) {
                op_logger("Grad")->error("Requested gradient with respect to a non-scalar function.");
                throw InvalidOperatorArgument(NodeVec{f}, "Grad", "Requested gradient with respect to a non-scalar function.");
            }
            NodeVec u = {g->constant(1)};
            u[0]->grad_level = f->grad_level + ((unsigned int)(1));
            return g->backward_diff(NodeVec{f}, u, w);
        };
    }
}