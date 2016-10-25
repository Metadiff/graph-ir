//
// Created by alex on 06/10/16.
//

#ifndef METADIFF_CORE_HELPERS_H
#define METADIFF_CORE_HELPERS_H

namespace md{
    namespace core{
        /** Calculates the total number of elements */
        SymInt number_of_elements(Shape const shape);

        /** Skips any alias operators to get the base operator */
        Operator get_base_op(Operator const op);

        /** Skips any alias operators to get the base node */
        Node get_base_node(Node node);

        /** Helper function to validate axes argument */
        bool validate_axes(Axes axes);

        /** Helper function to verify shapes of elementwise operators */
        Shape verify_elementwise_shapes(NodeVec nodes, std::shared_ptr<spdlog::logger> logger);

        /** TODO Tries to evaluate that two nodes are symbolically equals */
        bool symbolic_equals(Node const & node1, Node const & node2);

        /** Returns all axes which are not 1 */
        Axes auto_infer_axes(Node node);
    }
}
#endif //METADIFF_CORE_HELPERS_H
