//
// Created by alex on 27/10/16.
//

#ifndef GRAPH_IR_API_GENERAL_H
#define GRAPH_IR_API_GENERAL_H
#include "type_traits"
namespace md{
    namespace api{
        /** @brief Calculates the gradient of f with respect to the variables w
         *
         * @param f
         * @param w
         * @return
         */
        NodeVec gradient(Node const f, NodeVec const & w);
    }
}

#endif //GRAPH_IR_API_GENERAL_H
