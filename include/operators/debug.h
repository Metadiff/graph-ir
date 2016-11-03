//
// Created by alex on 21/10/16.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_DEBUG_H
#define METADIFF_GRAPH_IR_OPERATORS_DEBUG_H

namespace md{
    namespace gir{
        namespace op{
            /** The operator will print the monitored node at runtime, exactly after the execution */
            class Print: public MonitorOperator{
            public:
                Print(GraphInPtr const graph, Node anchor, Node monitored, std::string msg):
                        AbstractOperator(graph, "Print"), UnaryOperator(anchor), MonitorOperator(monitored, msg) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Print>(graph, ancestors[0], ancestors[1], msg);
                }
            };

            /** The operator will return the monitored node at runtime */
            class Retrieve: public MonitorOperator{
            public:
                Retrieve(GraphInPtr const graph, Node anchor, Node monitored, std::string msg):
                        AbstractOperator(graph, "Retrieve"), UnaryOperator(anchor), MonitorOperator(monitored, msg) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Retrieve>(graph, ancestors[0], ancestors[1], msg);
                }
            };

            /** The operator will print the monitored node at runtime, exactly after the execution */
            class LogToFile: public MonitorOperator{
            public:
                LogToFile(GraphInPtr const graph, Node anchor, Node monitored, std::string msg):
                        AbstractOperator(graph, "LogToFile"), UnaryOperator(anchor), MonitorOperator(monitored, msg) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogToFile>(graph, ancestors[0], ancestors[1], msg);
                }
            };

            /** The operator will guard that the monitored value that it does not go out of bounds including nans and infs */
            class Guard: public MonitorOperator{
            public:
                double low;
                double high;
                Guard(GraphInPtr const graph, Node anchor, Node monitored, std::string msg, double low, double high):
                        AbstractOperator(graph, "Guard"), UnaryOperator(anchor), MonitorOperator(monitored, msg),
                        low(low), high(high){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Guard>(graph, ancestors[0], ancestors[1], msg, low, high);
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_DEBUG_H
