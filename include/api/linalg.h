//
// Created by alex on 31/10/16.
//

#ifndef GRAPH_IR_API_LINALG_H
#define GRAPH_IR_API_LINALG_H

namespace md{
    namespace api{
        /** @brief Multiplies the matricies in the same order as provided and transposing which required
         *
         * @param nodes
         * @param transpositions
         * @return
         */
        Node matrix_mul(NodeVec nodes, std::vector<bool> transpositions = {});

        /** @brief Multiplies two matrices, with option for transposing any of them
         *
         * @param left
         * @param right
         * @param transpose_left
         * @param transpose_right
         * @return
         */
        Node dot(Node left, Node right, bool transpose_left = false, bool transpose_right = false);

        /** Takes the matrix inverse of the variable
         *
         * @param node
         * @return
         */
        Node matrix_inverse(Node node, bool transpose = false);

        /** @brief Takes the matrix inverse of the first matrix and mutyplies with the second
         *
         * @param node1
         * @param node2
         * @param transpose
         * @return
         */
        Node matrix_inverse_mul(Node node1, Node node2, bool transpose_inv = false, bool transpose_mul = false);

        /** @brief Takes the determinant of the matrix
         *
         * @param node
         * @return
         */
        Node determinant(Node node);

        /** @brief Takes the lograithm of the determinant of the matrix
         *
         * @param node
         * @return
         */
        Node log_det(Node node);

        /** @brief Takes the trace of the matrix
         *
         * @param node
         * @return
         */
        Node trace(Node node);

        /** @brief Takes the Kronecker product between the two
         *
         * @param node1
         * @param node2
         * @return
         */
        Node kron(Node node1, Node node2);

        /** @brief Calculates forward diff for Cholesky decomposition using blas routines
         * See https://arxiv.org/pdf/1602.07527v1.pdf
         *
         * @param cholesky
         * @param parent_derivative
         * @param lower
         * @return
         */
        Node cholesky_forward_diff_blas(Node cholesky, Node parent_derivative, bool lower = true);

        /** @brief Calculates backward diff for Cholesky decomposition using blas routines
         * See https://arxiv.org/pdf/1602.07527v1.pdf
         *
         * @param cholesky
         * @param my_derivative
         * @param lower
         * @return
         */
        Node cholesky_backward_diff_blas(Node cholesky, Node my_derivative, bool lower = true);
    }

    namespace gir {
        // Matrix inverse
        inline Node operator~(Node node) {
            return api::matrix_inverse(node);
        }
    }
}

#endif //GRAPH_IR_API_LINALG_H
