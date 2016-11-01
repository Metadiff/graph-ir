//
// Created by alex on 27/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        bool check_independent(Node anchor, Node monitored){
            return anchor->id >= monitored->id;
        }

        Axes validate_axes(Shape shape, Axes axes){
            std::sort(axes.begin(), axes.end());
            for(auto i=0; i < axes.size(); ++i){
                if(shape[axes[i]] == 1){
                    // Reduction on unit axis
                    // TODO throw proper exception
                    utils::op_logger("Reduction")->error("Reduction on unit axis {} - {}", axes[i], to_string(shape));
                    throw 1;
                } else if(i > 0 and axes[i] == axes[i-1]){
                    // Duplicate axis
                    // TODO throw proper exception
                    utils::op_logger("Reduction")->error("Duplicate axis {}", axes[i]);
                    throw 1;
                } else if(axes[i] < 0 or axes[i] > 4){
                    // Invalid axis
                    // TODO throw proper exception
                    utils::op_logger("Reduction")->error("Invalid axis {}", axes[i]);
                    throw 1;
                }
            }
            return axes;
        }

        Axes auto_infer_axes(Shape shape){
            Axes axes;
            for(auto i=0; i < 4; ++i){
                if(shape[i] != 1){
                    axes.push_back(i);
                }
            }
            return axes;
        }
    }

}