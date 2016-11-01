//
// Created by alex on 31/10/16.
//

#ifndef METADIFF_API_LINALG_H
#define METADIFF_API_LINALG_H

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
        Node matrix_inverse(Node node);

        /** @brief Takes the matrix inverse of the first matrix and mutyplies with the second
         *
         * @param node1
         * @param node2
         * @param transpose
         * @return
         */
        Node matrix_inverse_mul(Node node1, Node node2, bool transpose = false);

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
    }

    namespace core {
        // Matrix inverse
        inline Node operator~(Node node) {
            return api::matrix_inverse(node);
        }

//        // Matrix multiplication
//        inline Node operator&(Node node1, Node node2){
//            return api::dot(node1, node2);
//        }
//
//        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
//        inline Node operator&(Node node1, T node2) {
//            return api::dot(node1, wrap(node2, node1->g()));
//        };
//
//        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
//        inline Node operator&(T node1, Node node2) {
//            return api::dot(wrap(node1, node2->g()), node2);
//        };
    }
}

#endif //METADIFF_API_LINALG_H
