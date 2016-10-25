//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_LOGICAL_H
#define METADIFF_CORE_OPERATORS_LOGICAL_H

namespace md {
    namespace core {
        namespace op {

            /** Logical not - !p */
            class LogicalNot : public LogicalUnaryElementwise {
            public:
                LogicalNot(GraphInPtr graph, Node parent) :
                        AbstractOperator("LogicalNot", graph), UnaryOperator(parent) {
                    if (parent->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent}, name,
                                                                      "Calling logical not on node of type "
                                                                      + to_string(parent->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->parent = graph->cast(parent, b8);
                    }
                }

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
                        AbstractOperator("LogicalAnd", graph), BinaryOperator(parent1, parent2) {
                    if (parent1->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent1, parent2}, name,
                                                                      "Calling logical operator"
                                                                              " on node of type "
                                                                      + to_string(parent1->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->parent1 = graph->cast(parent1, b8);
                    }
                    if (parent2->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent1, parent2}, name,
                                                                      "Calling logical operator"
                                                                              " on node of type "
                                                                      + to_string(parent2->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->parent2 = graph->cast(parent2, b8);
                    }
                };

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
                        AbstractOperator("LogicalOr", graph), BinaryOperator(parent1, parent2) {
                    if (parent1->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent1, parent2}, name,
                                                                      "Calling logical operator"
                                                                              " on node of type "
                                                                      + to_string(parent1->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->parent1 = graph->cast(parent1, b8);
                    }
                    if (parent2->data_type != b8) {
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent1, parent2}, name,
                                                                      "Calling logical operator"
                                                                              " on node of type "
                                                                      + to_string(parent2->data_type));
                        operate_policy(graph->props.policies.cast, logger(), err);
                        this->parent2 = graph->cast(parent2, b8);
                    }
                };

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
                        AbstractOperator("GreaterThan", graph), BinaryOperator(parent1, parent2) {};

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
                        AbstractOperator("LessThan", graph), BinaryOperator(parent1, parent2) {};

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
                        AbstractOperator("GreaterThanOrEqual", graph), BinaryOperator(parent1, parent2) {};

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
                        AbstractOperator("LessThanOrEqual", graph), BinaryOperator(parent1, parent2) {};

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
                        AbstractOperator("Equals", graph), BinaryOperator(parent1, parent2) {};

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
                        AbstractOperator("NotEquals", graph), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<NotEquals>(graph, ancestors[0], ancestors[1]);
                }
            };

            /**  Checks if the two nodes are equal, up to a tolerance measure */
            class ApproximatelyEquals : public LogicalBinaryElementwise {
            public:
                double tol;

                ApproximatelyEquals(GraphInPtr graph,
                                    Node parent1,
                                    Node parent2,
                                    double tol) :
                        AbstractOperator("ApproximatelyEquals", graph), BinaryOperator(parent1, parent2),
                        tol(tol) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<ApproximatelyEquals>(graph, ancestors[0], ancestors[1], tol);
                }
            };

            /** Verifies if any of the elements is NaN */
            class IsNaN : public LogicalUnaryElementwise {
            public:
                IsNaN(GraphInPtr const graph, Node parent) :
                        AbstractOperator("IsNaN", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IsNaN>(graph, ancestors[0]);
                }
            };

            /** Verifies if any of the elements is Inf */
            class IsInf : public LogicalUnaryElementwise {
            public:
                IsInf(GraphInPtr const graph, Node parent) :
                        AbstractOperator("IsInf", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IsInf>(graph, ancestors[0]);
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_LOGICAL_H
