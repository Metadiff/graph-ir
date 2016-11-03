//
// Created by alex on 27/10/16.
//

#ifndef GRAPH_IR_API_SHAPE_H
#define GRAPH_IR_API_SHAPE_H

namespace md{
    namespace api{
        /** @brief Either extracts the diagonal of the input matrix,
         * or generates a matrix whose diagonal is equal to the input vector.
         *
         * @param node A matrix or a vector
         * @param g
         * @return
         */
        Node diag(Node node);

        /** @brief Reshapes the tensor to a specified shape
         *
         * @param node
         * @param shape
         * @param g
         * @return
         */
        Node reshape(Node node, Shape shape);

        /** @brief Reshapes the tensor to a vector
         *
         * @param node
         * @param shape
         * @param g
         * @return
         */
        Node flatten(Node node);

        /** @brief Reorders the dimensions of the tensor as specified
         *
         * @param node
         * @param axes
         * @param g
         * @return
         */
        Node reorder(Node node, Axes order);

        /** @brief Takes the transpose of a tesor. For 3D and 4D tensor switches the last two dimensions.
         *
         * @param node
         * @param g
         * @return
         */
        Node transpose(Node node);
    }
}
#endif //GRAPH_IR_API_SHAPE_H
