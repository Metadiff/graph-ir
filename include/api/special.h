//
// Created by alex on 27/10/16.
//

#ifndef METADIFF_API_SPECIAL_H
#define METADIFF_API_SPECIAL_H


namespace md{
    namespace api{
        /** @brief Casts the input tensor to a different DataType
         *
         * @param node
         * @param data_type
         * @return
         */
        Node cast(Node node, DataType data_type);

        /** @brief Boradcasts the input tensor to a specified Shape
         *
         * @param node
         * @param shape
         * @throw exception if the new Shape is incosistant with the current one
         * @return
         */
        Node broadcast(Node node, Shape shape);

        /** @brief Makes a new tensor which is an alias of the input
         *
         * @param node
         * @return
         */
        Node alias(Node node);

        /** @brief Makes a new tensor which is a view of the input, but is non-diffentiable
         *
         * @param node
         * @return
         */
        Node make_constant(Node node);

        /** @brief Makes a selection elementwise between two tensors based on the condition
         *
         * Formally R = select(C, A, B), then
         * R[i,j,k,l] = A[i, j, k, l] if C[i, j, k, l] = 1 else B[i, j, k, l]
         * The condition argument is expected to be a boolean, if not it will be casted to such.
         *
         * @param condition
         * @param if_true
         * @param if_false
         * @return
         */
        Node select(Node condition, Node if_true, Node if_false);

        template <typename T, typename F, typename = std::enable_if<
                !conjunction<std::is_same<T, Node>, std::is_same<F, Node>>::value>>
        Node select(Node condition, T if_true, F if_false){
            Graph g = condition->g();
            return select(condition, wrap(if_true, g), wrap(if_false, g));
        };

        template <typename C, typename F, typename = std::enable_if<
                !conjunction<std::is_same<C, Node>, std::is_same<F, Node>>::value>>
        Node select(C condition, Node if_true, F if_false){
            Graph g = if_true->g();
            return select(wrap(condition, g), if_true, wrap(if_false, g));
        };

        template <typename C, typename T, typename = std::enable_if<
                !conjunction<std::is_same<C, Node>, std::is_same<T, Node>>::value>>
        Node select(C condition, T if_true, Node if_false){
            Graph g = if_false->g();
            return select(wrap(condition, g), wrap(if_true, g), if_false);
        };
    }
}
#endif //METADIFF_API_SPECIAL_H
