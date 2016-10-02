//
// Created by alex on 30/09/16.
//

namespace md{
    namespace core{
        namespace op{

            bool validate_axes(Axes axes){
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

            Shape verify_elementwise_shapes(NodeVec nodes, ) {
                if(nodes.size() == 0){
                    auto err = OtherError(nodes);

                }
                Shape max_shape = nodes[0]->shape;
                for (int i = 1; i < node_ptrs.size(); i++) {
                    Shape node_shape = node_ptrs[i]->shape;
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
                                auto err = IncompatibleShapes(node_ptrs, name);
                                logger->error() << err.msg;
                                throw err;
                            } else if (node_shape[j] != 1 and max_shape[j] != 1 and node_shape[j] != max_shape[j]) {
                                auto err = IncompatibleShapes(node_ptrs, name);
                                logger->error() << err.msg;
                                throw err;
                            }
                        }
                        max_shape = node_shape;
                    }
                }
                return max_shape;
            }
        }
    }
}