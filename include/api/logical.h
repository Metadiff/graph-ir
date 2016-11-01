//
// Created by alex on 28/10/16.
//

#ifndef METADIFF_API_LOGICAL_H
#define METADIFF_API_LOGICAL_H

namespace md{
    namespace api{
        /** @brief Applies logical NOT operator elementwise 
         * 
         * @param node - Expected to be of type b8, otherwise it is casted or an error is raised.
         * @return A tensor of type b8
         */
        Node logical_not(Node node);

        /** @brief Applies logical AND operator elementwise
         * 
         * @param node1 - Expected to be of type b8, otherwise it is casted or an error is raised.
         * @param node2 - Expected to be of type b8, otherwise it is casted or an error is raised.
         * @return A tensor of type b8
         */
        Node logical_and(Node node1, Node node2);

        /** @brief Applies logical OR operator elementwise
         * 
         * @param node1 - Expected to be of type b8, otherwise it is casted or an error is raised.
         * @param node2 - Expected to be of type b8, otherwise it is casted or an error is raised.
         * @return A tensor of type b8
         */
        Node logical_or(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is greater than node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node greater_than(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is less than node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node less_than(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is greater than or equal node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node greater_than_or_equal(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is less than or equal node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node less_than_or_equal(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is equal to node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node equals(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is not equal to node2
         * 
         * @param node1 
         * @param node2 
         * @return A tensor of type b8
         */
        Node not_equals(Node node1, Node node2);

        /** @brief Compares elementwise whether node1 is approximately equal to node2 up to the specified tolerance
         * 
         * @param node1 
         * @param node2 
         * @param tolerance 
         * @return A tensor of type b8
         */
        Node approx_equals(Node node1, Node node2, double tolerance = 1e-9);

        /** @brief Checks elementwise wheter each element of the variable is Not a Number (NaN) 
         * 
         * @param node 
         * @return A tensor of type b8
         */
        Node isNan(Node node);

        /** @brief Checks elementwise wheter each element of the variable is Infinity (Inf)
         * 
         * @param node 
         * @return A tensor of type b8
         */
        Node isInf(Node node);
    }

    namespace core {
        // not A
        inline Node operator!(Node node) {
            return api::logical_not(node);
        }

        // A and B
        inline Node operator&&(Node node1, Node node2) {
            return api::logical_and(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator&&(Node node1, T node2) {
            return api::logical_and(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator&&(T node1, Node node2) {
            return api::logical_and(wrap(node1, node2->g()), node2);
        };

        // A or B
        inline Node operator||(Node node1, Node node2) {
            return api::logical_or(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator||(Node node1, T node2) {
            return api::logical_or(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator||(T node1, Node node2) {
            return api::logical_or(wrap(node1, node2->g()), node2);
        };

        // A > B
        inline Node operator>(Node node1, Node node2) {
            return api::greater_than(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator>(Node node1, T node2) {
            return api::greater_than(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator>(T node1, Node node2) {
            return api::greater_than(wrap(node1, node2->g()), node2);
        };

        // A < B
        inline Node operator<(Node node1, Node node2) {
            return api::less_than(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator<(Node node1, T node2) {
            return api::less_than(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator<(T node1, Node node2) {
            return api::less_than(wrap(node1, node2->g()), node2);
        };

        // A >= B
        inline Node operator>=(Node node1, Node node2) {
            return api::greater_than_or_equal(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator>=(Node node1, T node2) {
            return api::greater_than_or_equal(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator>=(T node1, Node node2) {
            return api::greater_than_or_equal(wrap(node1, node2->g()), node2);
        };

        // A <= B
        inline Node operator<=(Node node1, Node node2) {
            return api::less_than_or_equal(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator<=(Node node1, T node2) {
            return api::less_than_or_equal(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator<=(T node1, Node node2) {
            return api::less_than_or_equal(wrap(node1, node2->g()), node2);
        };

        // A == B
        inline Node operator==(Node node1, Node node2) {
            return api::equals(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator==(Node node1, T node2) {
            return api::equals(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator==(T node1, Node node2) {
            return api::equals(wrap(node1, node2->g()), node2);
        };

        // A != B
        inline Node operator!=(Node node1, Node node2) {
            return api::not_equals(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator!=(Node node1, T node2) {
            return api::not_equals(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator!=(T node1, Node node2) {
            return api::not_equals(wrap(node1, node2->g()), node2);
        };
    }
}
#endif //METADIFF_API_LOGICAL_H
