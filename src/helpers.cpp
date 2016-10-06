//
// Created by alex on 02/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        SymInt elements_number(Shape const shape){
            return shape[0] * shape[1] * shape[2] * shape[3];
        }

        Operator get_base_op(Operator const op) {
            Operator base_op = op;
            while (base_op->name == "Alias") {
                base_op = base_op->get_parents()[0]->op;
            }
            return base_op;
        }

        bool validate_axes(Axes axes) {
            if (axes.size() > 4) {
                return false;
            }
            bool checks[4]{false, false, false, false};
            for (int i = 0; i < axes.size(); i++) {
                if (axes[i] > 3) {
                    return false;
                }
                if (checks[axes[i]]) {
                    return false;
                }
                checks[axes[i]] = true;
            }
            return true;
        }

        Shape verify_elementwise_shapes(NodeVec nodes) {
            Shape max_shape = nodes[0]->shape;
            for (int i = 1; i < nodes.size(); i++) {
                Shape node_shape = nodes[i]->shape;
                bool max = false;
                for (int j = 0; j < 4; j++) {
                    if (node_shape[j] != 1 and max_shape[j] == 1) {
                        max = true;
                        break;
                    }
                }
                if (max) {
                    for (int j = 0; j < 4; j++) {
                        if (node_shape[j] == 1 and max_shape[j] != 1) {
                            auto err = std::make_shared<IncompatibleShapes>(nodes, nodes[0]->op->name);
                            err->log(nodes[0]->op->logger());
                            throw err;
                        } else if (node_shape[j] != 1 and max_shape[j] != 1 and node_shape[j] != max_shape[j]) {
                            auto err = std::make_shared<IncompatibleShapes>(nodes, nodes[0]->op->name);
                            err->log(nodes[0]->op->logger());
                            throw err;
                        }
                    }
                    max_shape = node_shape;
                }
            }
            return max_shape;
        }

        bool symbolic_equals(Node const & node1, Node const & node2) {
            if (node1->id == node2->id) {
                return true;
            }
            if (node1->node_type != node2->node_type) {
                return false;
            }
            nodeType node_type = node1->node_type;
            if (node_type == INPUT) {
                return false;
            } else {
                auto base_op1 = get_base_op(node1->op);
                auto base_op2 = get_base_op(node2->op);
                return base_op1->equals(base_op2) or base_op2->equals(base_op1);
            }
        };
    }
}
