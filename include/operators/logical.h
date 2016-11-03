//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_LOGICAL_H
#define METADIFF_GRAPH_IR_OPERATORS_LOGICAL_H

namespace md {
    namespace gir {
        namespace op {

            /** Logical not - !p */
            class LogicalNot : public LogicalUnaryElementwise {
            public:
                LogicalNot(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "LogicalNot"), UnaryOperator(parent) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogicalNot>(graph, ancestors[0]);
                }
            };

            /** Logical and - p1 && p2 */
            class LogicalAnd : public LogicalBinaryElementwise {
            public:
                LogicalAnd(GraphInPtr graph,
                           Node parent1,
                           Node parent2) :
                        AbstractOperator(graph, "LogicalAnd"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogicalAnd>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Logical or - p1 || p2 */
            class LogicalOr : public LogicalBinaryElementwise {
            public:
                LogicalOr(GraphInPtr graph,
                          Node parent1,
                          Node parent2) :
                        AbstractOperator(graph, "LogicalOr"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogicalOr>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 > p2 */
            class GreaterThan : public LogicalBinaryElementwise {
            public:
                GreaterThan(GraphInPtr graph,
                            Node parent1,
                            Node parent2) :
                        AbstractOperator(graph, "GreaterThan"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<GreaterThan>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 < p2 */
            class LessThan : public LogicalBinaryElementwise {
            public:
                LessThan(GraphInPtr graph,
                         Node parent1,
                         Node parent2) :
                        AbstractOperator(graph, "LessThan"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LessThan>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 >= p2 */
            class GreaterThanOrEqual : public LogicalBinaryElementwise {
            public:
                GreaterThanOrEqual(GraphInPtr graph,
                                   Node parent1,
                                   Node parent2) :
                        AbstractOperator(graph, "GreaterThanOrEqual"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<GreaterThanOrEqual>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 <= p2 */
            class LessThanOrEqual : public LogicalBinaryElementwise {
            public:
                LessThanOrEqual(GraphInPtr graph,
                                Node parent1,
                                Node parent2) :
                        AbstractOperator(graph, "LessThanOrEqual"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LessThanOrEqual>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 == p2 */
            class Equals : public LogicalBinaryElementwise {
            public:
                Equals(GraphInPtr graph,
                       Node parent1,
                       Node parent2) :
                        AbstractOperator(graph, "Equals"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Equals>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Elementwise comparison for p1 != p2 */
            class NotEquals : public LogicalBinaryElementwise {
            public:
                NotEquals(GraphInPtr graph,
                          Node parent1,
                          Node parent2) :
                        AbstractOperator(graph, "NotEquals"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<NotEquals>(graph, ancestors[0], ancestors[1]);
                }
            };

            /**  Returns if the two nodes are equal, up to a tolerance measure */
            class ApproximatelyEquals : public LogicalBinaryElementwise {
            public:
                double tolerance;

                ApproximatelyEquals(GraphInPtr graph,
                                    Node parent1,
                                    Node parent2,
                                    double tolerance) :
                        AbstractOperator(graph, "ApproximatelyEquals"), BinaryOperator(parent1, parent2),
                        tolerance(tolerance) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<ApproximatelyEquals>(graph, ancestors[0], ancestors[1], tolerance);
                }
            };

            /** Returns whether the elements of the input are NaN */
            class IsNaN : public LogicalUnaryElementwise {
            public:
                IsNaN(GraphInPtr const graph, Node parent) :
                        AbstractOperator(graph, "IsNaN"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IsNaN>(graph, ancestors[0]);
                }
            };

            /** Returns whether the elements of the input are Inf */
            class IsInf : public LogicalUnaryElementwise {
            public:
                IsInf(GraphInPtr const graph, Node parent) :
                        AbstractOperator(graph, "IsInf"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IsInf>(graph, ancestors[0]);
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_LOGICAL_H
