//
// Created by alex on 31/10/16.
//

#ifndef GRAPH_IR_OPTIMIZED_H
#define GRAPH_IR_OPTIMIZED_H

namespace md{
    namespace api{
        /** @brief Elementwise calculates log(1 + exp(x))
         *
         * @param node
         * @param threshold
         * @return
         */
        Node softplus(Node node, double threshold = 50);

        /** @brief Elementwise calculautes the logistic function - 1 / (1 + exp(-x))
         *
         * @param node
         * @return
         */
        Node sigmoid(Node node);

        /** @brief Calculates log(sum(exp(x), axes)) in a more stable way
         *
         * @param node
         * @param axes
         * @return
         */
        Node log_sum_exp(Node node, Axes axes);

        /** @brief Takes the log of the sum of exponentials of all of the elements of the variable
         *
         * @param node
         * @return
         */
        Node log_sum_exp(Node node);

        /** @brief See log_sum_exp(node, Axes)
         * @param node
         * @param axis
         * @return
         */
        Node log_sum_exp(Node node, int axis);

        /** @brief See log_sum_exp(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node log_sum_exp(Node node, int axis0, int axis1);

        /** @brief See log_sum_exp(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node log_sum_exp(Node node, int axis0, int axis1, int axis2);

        /** @brief Takes the softmax of the node, normalizing along the axes provieded
         *
         * @param node
         * @return
         */
        Node softmax(Node node, Axes axes);

        /** @brief Takes the softmax, normalizing along the last non-unit dimension
         *
         * @param node
         * @return
         */
        Node softmax(Node node);

        /** @brief See softmax(node, Axes)
         * If the axis=-1 then normalizes all of the elements
         * @param node
         * @param axis
         * @return
         */
        Node softmax(Node node, int axis);

        /** @brief See softmax(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node softmax(Node node, int axis0, int axis1);

        /** @brief See softmax(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node softmax(Node node, int axis0, int axis1, int axis2);
    }
}
#endif //GRAPH_IR_OPTIMIZED_H
