//
// Created by alex on 31/10/16.
//

#ifndef GRAPH_IR_API_DEBUG_H
#define GRAPH_IR_API_DEBUG_H

namespace md{
    namespace api{
        /** @brief Prints out the values of the monitored node exactly after the calculation of the anchor.
         * If the anchor node is empty than use the monitored node as an anchor.
         * Note that this prevents optimization of the monitored node.
         *
         * @param anchor
         * @param monitored
         * @return
         */
        Node print(Node monitored, std::string msg, Node anchor = Node());

        /** @brief Retrieves the monitored value together with the outputs at the end of the function execution
         * If the anchor node is empty than use the monitored node as an anchor.
         * Note that this prevents optimization of the monitored node.
         *
         * @param anchor
         * @param monitored
         * @return
         */
        Node retrieve(Node monitored, std::string msg, Node anchor = Node());

        /** @brief Logs to a file the values of the monitored node exactly after the calculation of the anchor.
         * If the anchor node is empty than use the monitored node as an anchor.
         * Note that this prevents optimization of the monitored node.
         *
         * @param anchor
         * @param monitored
         * @return
         */
        Node log_to_file(Node monitored, std::string msg, Node anchor = Node());

        /** @brief Guards that the values of the monitored node do not go outside the interval low, high or go NaN or Inf.
         * The check is executed exacly after the calculation of the anchor.
         * If the anchor node is empty than use the monitored node as an anchor.
         * Note that this prevents optimization of the monitored node.
         *
         * @param anchor
         * @param monitored
         * @return
         */
        Node guard(Node monitored, std::string msg, double low, double high, Node anchor = Node());
    }
}
#endif //GRAPH_IR_API_DEBUG_H
