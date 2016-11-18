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

        /** @brief Takes the lower triangular part of a matrix
         * if k is an integer takes the lower traingular bit excluding the k major diagonals
         * if k < 1 multiplies the diagonal with k
         * k can not be less then 0
         *
         * @param node
         * @param k
         * @return
         */
        Node lower_tri(Node node, double k = 0);

        /** @brief Takes the upper triangular part of a matrix
         * if k > 1 it must be an integer takes the upper traingular bit excluding the k major diagonals
         * if k < 1 multiplies the diagonal with k
         * k can not be less then 0
         * @param node
         * @param k
         * @return
         */
        Node upper_tri(Node node, double k = 0);

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

        /** @brief Flips the elements of the tensor along every of the specified axes
         *
         * @param node
         * @param axes
         * @return
         */
        Node flip(Node node, Axes axes);

        /** @brief Flips the elements of the tensor along the specified axis
         *
         * @param node
         * @param axis
         * @return
         */
        Node flip(Node node, int axis);
    }
}
#endif //GRAPH_IR_API_SHAPE_H
