//
// Created by alex on 29/10/16.
//

#ifndef METADIFF_API_REDUCTION_H
#define METADIFF_API_REDUCTION_H

namespace md{
    namespace api{
        /** @brief Sums the variable along all of the axes specified.
         *
         * @param node
         * @param axes
         * @return
         */
        Node sum(Node node, Axes axes);
        
        /** @brief Sums all of the elements of the variable
         * 
         * @param node 
         * @return 
         */
        Node sum(Node node);

        /** @brief See sum(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node sum(Node node, int axis);

        /** @brief See sum(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node sum(Node node, int axis0, int axis1);

        /** @brief See sum(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node sum(Node node, int axis0, int axis1, int axis2);

        /** @brief Calculates the mean of the variable along all of the axes specified.
         *
         * @param node
         * @param axes
         * @return
         */
        Node mean(Node node, Axes axes);

        /** @brief Calculates the mean of all of the elements of the variable
         *
         * @param node
         * @return
         */
        Node mean(Node node);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node mean(Node node, int axis);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node mean(Node node, int axis0, int axis1);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node mean(Node node, int axis0, int axis1, int axis2);

        /** @brief Calculates the variance of the variable along all of the axes specified.
         *
         * @param node
         * @param axes
         * @return
         */
        Node var(Node node, Axes axes);

        /** @brief Calculates the variance of all of the elements of the variable
         *
         * @param node
         * @return
         */
        Node var(Node node);

        /** @brief See var(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node var(Node node, int axis);

        /** @brief See var(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node var(Node node, int axis0, int axis1);

        /** @brief See var(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node var(Node node, int axis0, int axis1, int axis2);

        /** @brief Takes the product of the variable along all of the axes specified.
         *
         * @param node
         * @param axes
         * @return
         */
        Node prod(Node node, Axes axes);

        /** @brief Takes the product of all of the elements of the variable
         *
         * @param node
         * @return
         */
        Node prod(Node node);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node prod(Node node, int axis);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node prod(Node node, int axis0, int axis1);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node prod(Node node, int axis0, int axis1, int axis2);

        /** @brief Returns whether any of the elements of the variable along the axes specified are true
         *
         * @param node
         * @param axes
         * @return
         */
        Node all_true(Node node, Axes axes);

        /** @brief Returns whether any of the elements of the variable are true
         *
         * @param node
         * @return
         */
        Node all_true(Node node);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node all_true(Node node, int axis);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node all_true(Node node, int axis0, int axis1);

        /** @brief See mean(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node all_true(Node node, int axis0, int axis1, int axis2);

        /** @brief Returns whether all of the elements of the variable along the axes specified are true
         *
         * @param node
         * @param axes
         * @return
         */
        Node any_true(Node node, Axes axes);

        /** @brief Returns whether all of the elements of the variable are true
         *
         * @param node
         * @return
         */
        Node any_true(Node node);

        /** @brief See any_true(node, Axes)
         *
         * @param node
         * @param axis
         * @return
         */
        Node any_true(Node node, int axis);

        /** @brief See any_true(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @return
         */
        Node any_true(Node node, int axis0, int axis1);

        /** @brief See any_true(node, Axes)
         *
         * @param node
         * @param axis0
         * @param axis1
         * @param axis2
         * @return
         */
        Node any_true(Node node, int axis0, int axis1, int axis2);
    }
}
#endif //METADIFF_API_REDUCTION_H
