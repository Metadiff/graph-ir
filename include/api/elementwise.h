//
// Created by alex on 30/10/16.
//

#ifndef METADIFF_API_ELEMENTWISE_H
#define METADIFF_API_ELEMENTWISE_H

namespace md{
    namespace api{
        /** @brief Elementwise square
         *
         * @param node
         * @return
         */
        Node square(Node node);

        /** @brief Elementwise square root
         *
         * @param node
         * @return
         */
        Node sqrt(Node node);

        /** @brief Elementwise exponential
         *
         * @param node
         * @return
         */
        Node exp(Node node);

        /** @brief Elementwise natural logarithm
         *
         * @param node
         * @return
         */
        Node log(Node node);

        /** @brief Elementwise logarithm in base 10
         *
         * @param node
         * @return
         */
        Node log10(Node node);

        /** @brief Elementwise absoulte value
         *
         * @param node
         * @return
         */
        Node abs(Node node);

        /** @brief Elementwise calculates the function log(x+1)
         *
         * @param node
         * @return
         */
        Node log1p(Node node);

        /** @brief Elementwise trigonometirc sine
         *
         * @param node
         * @return
         */
        Node sin(Node node);

        /** @brief Elementwise trigonometirc cosine
         *
         * @param node
         * @return
         */
        Node cos(Node node);

        /** @brief Elementwise trigonometirc tangent
         *
         * @param node
         * @return
         */
        Node tan(Node node);

        /** @brief Elementwise trigonometirc cotangent
         *
         * @param node
         * @return
         */
        Node cot(Node node);

        /** @brief Elementwise hyperbolic sine
         *
         * @param node
         * @return
         */
        Node sinh(Node node);

        /** @brief Elementwise hyperbolic cosine
         *
         * @param node
         * @return
         */
        Node cosh(Node node);

        /** @brief Elementwise hyperbolic tangent
         *
         * @param node
         * @return
         */
        Node tanh(Node node);

        /** @brief Elementwise hyperbolic cotangent
         *
         * @param node
         * @return
         */
        Node coth(Node node);

        /** @brief Takes the elements of node1 to the power as the elements of node2
         *
         * @param node
         * @return
         */
        Node pow(Node node1, Node node2);
    }
}
#endif //METADIFF_API_ELEMENTWISE_H
