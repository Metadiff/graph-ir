//
// Created by alex on 28/10/16.
//

#ifndef METADIFF_API_ARITHMETIC_H
#define METADIFF_API_ARITHMETIC_H

namespace md{
    namespace api{
        /** @brief Adds elementwise all of the variables together
         *
         * @param nodes
         * @return
         */
        Node add(NodeVec nodes);

        /** @brief see add(NodeVec)
         *
         * @param node1
         * @param node2
         * @return
         */
        Node add(Node node1, Node node2);

        /** @brief see add(NodeVec)
         *
         * @param node1
         * @param node2
         * @param node3
         * @return
         */
        Node add(Node node1, Node node2, Node node3);

        /** @brief see add(NodeVec)
         *
         * @param node1
         * @param node2
         * @param node3
         * @param node4
         * @return
         */
        Node add(Node node1, Node node2, Node node3, Node node4);

        /** @brief Negates elementwise the variable
         *
         * @param node
         * @return
         */
        Node neg(Node node);

        /** @brief Takes elementwise the difference between node1 and node2
         *
         * @param node1
         * @param node2
         * @return
         */
        Node neg(Node node1, Node node2);

        /** @brief Multiplies elementwise all of the variables together
         *
         * @param nodes
         * @return
         */
        Node mul(NodeVec nodes);

        /** @brief see mul(NodeVec)
         *
         * @param node1
         * @param node2
         * @return
         */
        Node mul(Node node1, Node node2);

        /** @brief see mul(NodeVec)
         *
         * @param node1
         * @param node2
         * @param node3
         * @return
         */
        Node mul(Node node1, Node node2, Node node3);

        /** @brief see mul(NodeVec)
         *
         * @param node1
         * @param node2
         * @param node3
         * @param node4
         * @return
         */
        Node mul(Node node1, Node node2, Node node3, Node node4);

        /** @brief Takes elementwise the inverse of the variable
         *
         * @param node
         * @return
         */
        Node div(Node node);

        /** @brief Divides elementwise node1 by node2
         *
         * @param node1
         * @param node2
         * @return
         */
        Node div(Node node1, Node node2);

        /** @brief Makes an integer division elementwise of node1 by node2
         *
         * For this operator both nodes are expected to be integer types
         *
         * @param node1
         * @param node2
         * @return
         */
        Node int_div(Node node1, Node node2);

        /** @brief Takes the integer modulus elementwise of node1 by node2
         *
         * For this operator both nodes are expected to be integer types
         *
         * @param node1
         * @param node2
         * @return
         */
        Node int_mod(Node node1, Node node2);
    }

    namespace core {
        // Add
        inline Node operator+(Node node) {
                return node;
        }

        inline Node operator+(Node node1, Node node2) {
                return api::add(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator+(Node node1, T node2) {
                return api::add(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator+(T node1, Node node2) {
                return api::add(wrap(node1, node2->g()), node2);
        };

        // Compound Add
        inline Node& operator+=(Node & node1, Node const & node2){
            node1.ptr = api::add(node1, node2).ptr;
            return node1;
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator+=(Node & node1, T const & node2) {
            node1.ptr = api::add(node1, wrap(node2, node1->g())).ptr;
            return node1;
        };

        // Neg
        inline Node operator-(Node node) {
            return api::neg(node);
        }

        inline Node operator-(Node node1, Node node2) {
            return api::neg(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator-(Node node1, T node2) {
            return api::neg(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator-(T node1, Node node2) {
            return api::neg(wrap(node1, node2->g()), node2);
        };

        // Compound Neg
        inline Node& operator-=(Node & node1, Node const & node2){
            node1.ptr = api::neg(node1, node2).ptr;
            return node1;
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator-=(Node & node1, T const & node2) {
            node1.ptr = api::neg(node1, wrap(node2, node1->g())).ptr;
            return node1;
        };

        // Mul
        inline Node operator*(Node node1, Node node2) {
            return api::mul(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator*(Node node1, T node2) {
            return api::mul(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator*(T node1, Node node2) {
            return api::mul(wrap(node1, node2->g()), node2);
        };

        // Compound Mul
        inline Node& operator*=(Node & node1, Node const & node2){
            node1.ptr = api::mul(node1, node2).ptr;
            return node1;
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator*=(Node & node1, T const & node2) {
            node1.ptr = api::mul(node1, wrap(node2, node1->g())).ptr;
            return node1;
        };

        // Division
        inline Node operator/(Node node1, Node node2) {
            return api::div(node1, node2);
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator/(Node node1, T node2) {
            return api::div(node1, wrap(node2, node1->g()));
        };

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator/(T node1, Node node2) {
            return api::div(wrap(node1, node2->g()), node2);
        };

        // Compound Div
        inline Node& operator/=(Node & node1, Node const & node2){
            node1.ptr = api::div(node1, node2).ptr;
            return node1;
        }

        template<typename T, typename = std::enable_if<!std::is_same<T, Node>::value>>
        inline Node operator/=(Node & node1, T const & node2) {
            node1.ptr = api::div(node1, wrap(node2, node1->g())).ptr;
            return node1;
        };

    }
}
#endif //METADIFF_API_ARITHMETIC_H
