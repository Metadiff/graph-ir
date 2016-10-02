//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_OPERATORS_LOGICAL_H
#define METADIFF_OPERATORS_LOGICAL_H

namespace metadiff {
    namespace op {
        using namespace core;
        using namespace exceptions;
        
        /** Logical not - !p */
        class Not : public LogicalUnary {
        public:
            Not(GraphInPtr graph, Node parent) :
                    LogicalUnary("Not", graph, parent) {
                if (parent->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent = parent.cast(b8);
                }
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Not>(graph, ancestors[0]);
            }
        };

        /** Logical and - p1 && p2 */
        class And : public LogicalBinary {
        public:
            And(GraphInPtr graph,
                Node parent1,
                Node parent2) :
                    LogicalBinary("And", graph, parent1, parent2) {
                if (parent1->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent1, parent2}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent1 = parent1.cast(b8);
                }
                if (parent2->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent1, parent2}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent2 = parent2.cast(b8);
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<And>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Logical and - p1 && p2 */
        class Or : public LogicalBinary {
        public:
            Or(GraphInPtr graph,
                Node parent1,
                Node parent2) :
                    LogicalBinary("Or", graph, parent1, parent2) {
                if (parent1->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent1, parent2}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent1 = parent1.cast(b8);
                }
                if (parent2->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent1, parent2}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent2 = parent2.cast(b8);
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Or>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 > p2 */
        class GreaterThan : public LogicalBinary {
        public:
            GreaterThan(GraphInPtr graph,
                        Node parent1,
                        Node parent2) :
                    LogicalBinary("Gt", graph, parent1, parent2) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<GreaterThan>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 >= p2 */
        class GreaterThanOrEqual : public LogicalBinary {
        public:
            GreaterThanOrEqual(GraphInPtr graph,
                               Node parent1,
                               Node parent2) :
                    LogicalBinary("Ge", graph, parent1, parent2) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<GreaterThanOrEqual>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 < p2 */
        class LessThan : public LogicalBinary {
        public:
            LessThan(GraphInPtr graph,
                     Node parent1,
                     Node parent2) :
                    LogicalBinary("Lt", graph, parent1, parent2) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<LessThan>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 <= p2 */
        class LessThanOrEqual : public LogicalBinary {
        public:
            LessThanOrEqual(GraphInPtr graph,
                            Node parent1,
                            Node parent2) :
                    LogicalBinary("Le", graph, parent1, parent2) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<LessThanOrEqual>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 == p2 */
        class Equals : public LogicalBinary {
        public:
            Equals(GraphInPtr graph,
                   Node parent1,
                   Node parent2) :
                    LogicalBinary("Eq", graph, parent1, parent2) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Equals>(graph, ancestors[0], ancestors[1]);
            }
        };

        /** Elementwise comparison for p1 != p2 */
        class NotEquals : public LogicalBinary {
        public:
            NotEquals(GraphInPtr graph,
                   Node parent1,
                   Node parent2) :
                    LogicalBinary("NotEq", graph, parent1, parent2) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<NotEquals>(graph, ancestors[0], ancestors[1]);
            }
        };

        /**  Checks if the two nodes are equal, up to a tolerance measure */
        class ApproximatelyEquals : public LogicalBinary {
        public:
            double tol;
            ApproximatelyEquals(GraphInPtr graph,
                                Node parent1,
                                Node parent2,
                                double tol) :
                    LogicalBinary("ApproxEq", graph, parent1, parent2),
                    tol(tol) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<ApproximatelyEquals>(graph, ancestors[0], ancestors[1], tol);
            }
        };

        /** Verifies if any of the elements is NaN */
        class IsNaN : public LogicalUnary {
        public:
            IsNaN(GraphInPtr graph,
                  Node parent) :
                    LogicalUnary("IsNaN", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<IsNaN>(graph, ancestors[0]);
            }
        };

        /** Verifies if any of the elements is Inf */
        class IsInf : public LogicalUnary {
        public:
            IsInf(GraphInPtr graph,
                  Node parent) :
                    LogicalUnary("IsInf", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<IsInf>(graph, ancestors[0]);
            }
        };

        /** Verifies all of the variables in the tensor are true
         * TODO maybe add an axis parameter?
         */
        class All : public LogicalUnary {
        public:
            All(GraphInPtr graph, Node parent) :
                    LogicalUnary("All", graph, parent) {
                if (parent->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent = parent.cast(b8);
                }
            }

            Shape get_shape() const {
                return scalar_shape;
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<All>(graph, ancestors[0]);
            }
        };

        /** Verifies if any of the variables in the tensor are true
         * TODO maybe add an axis parameter?
         */
        class Any : public LogicalUnary {
        public:
            Any(GraphInPtr graph, Node parent) :
                    LogicalUnary("Any", graph, parent) {
                if (parent->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{parent}, name,
                                                          "Logical operators should be called only on b8 inputs"));
                    this->parent = parent.cast(b8);
                }
            }

            Shape get_shape() const {
                return scalar_shape;
            }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Any>(graph, ancestors[0]);
            }
        };


        /**
         * Elementwise selects one of the two parents based on the condition
         * Both the parents and the condition node must be of the same size.
         */
        class Select : public ElementwiseBinary {
        public:
            Node condition;
            Select(GraphInPtr graph,
                   Node condition,
                   Node trueParent,
                   Node falseParent) :
                    ElementwiseBinary("Select", graph, trueParent, falseParent),
                    condition(condition) {
                if (condition->dtype != b8) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{condition, trueParent, falseParent}, name,
                                                          "The condition in Select should be of type b8"));
                    this->condition = condition.cast(b8);
                }
                if (parent1->dtype != parent2->dtype) {
                    operate_policy(graph->cast_err_policy,
                                         logger(),
                                         InvalidArguments(NodeVec{condition, trueParent, falseParent}, name,
                                                          "The parents in Select should be of the same data type"));
                    dType promoted = graph->promote_type(trueParent->dtype, falseParent->dtype);
                    if(promoted != trueParent->dtype){
                        this->parent1 = trueParent.cast(promoted);
                    } else {
                        this->parent2 = falseParent.cast(promoted);
                    }
                }
                shape = verify_elementwise_shapes(name, NodeVec{condition, trueParent, falseParent}, logger());
                if (condition.is_scalar()) {
                    this->condition = this->condition.broadcast(shape);
                }
                if (trueParent.is_scalar()) {
                    this->parent1 = this->parent1.broadcast(shape);
                }
                if (falseParent.is_scalar()) {
                    this->parent1 = this->parent2.broadcast(shape);
                }
            };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Select>(graph, ancestors[2], ancestors[0], ancestors[1]);
            }

            NodeVec get_arguments() const {
                return NodeVec {condition};
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node zero = graph->constant_value(0.0);
                zero->grad_level = my_grad.unwrap()->grad_level;
                if (index == 0) {
                    return condition.select(my_grad, zero);
                } else {
                    return condition.select(zero, my_grad);
                }
            };
        };

        //        /**
//         * Checks every element if its equal to 0
//         */
//        class ZeroElements : public LogicalUnary {
//        public:
//            ZeroElements(GraphInPtr graph,
//                         Node parent) :
//                    LogicalUnary("ZeroElem", graph, parent) { };
//
//            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
//                return std::make_shared<ZeroElements>(graph, ancestors[0]);
//            }
//        };
//
//        Node Node::zero_elem() {
//            return apply<ZeroElements>(this);
//        }
//
//        Node zero_elem(Node node) {
//            return apply<ZeroElements>(node);
//        }
//
//        Node Node::non_zero_elem() {
//            return !(this->zero_elem());
//        }
//
//        Node non_zero_elem(Node node) {
//            return !(node.zero_elem());
//        }
    }
    
    namespace core{
        Node Node::logical_not(){
            return apply<op::Not>(this);
        }

        Node operator!(Node node) {
            return apply<op::Not>(node);
        }

        Node Node::logical_and(Node node) {
            return apply<op::And>(this, node);
        }

        Node operator&&(Node node1, Node node2) {
            return apply<op::And>(node1, node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator&&(L node1, Node node2) {
            return node2->graph->wrap(node1).logical_and(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator&&(Node node1, R node2) {
            return node1.logical_and(node1->graph->wrap(node2));
        };

        Node Node::logical_or(Node node) {
            return apply<op::Or>(this, node);
        }

        Node operator||(Node node1, Node node2) {
            return apply<op::Or>(node1, node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator||(L node1, Node node2) {
            return node2->graph->wrap(node1).logical_or(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator||(Node node1, R node2) {
            return node1.logical_or(node1->graph->wrap(node2));
        };

        Node Node::gt(Node node) {
            return apply<op::GreaterThan>(this, node);
        }

        Node operator>(Node node1, Node node2) {
            return node1.gt(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator>(L node1, Node node2) {
            return node2->graph->wrap(node1).gt(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator>(Node node1, R node2) {
            return node1.gt(node1->graph->wrap(node2));
        };

        Node Node::ge(Node node) {
            return apply<op::GreaterThanOrEqual>(this, node);
        }

        Node operator>=(Node node1, Node node2) {
            return node1.ge(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator>=(L node1, Node node2) {
            return node2->graph->wrap(node1).ge(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator>=(Node node1, R node2) {
            return node1.ge(node1->graph->wrap(node2));
        };

        Node Node::lt(Node node) {
            return apply<op::LessThan>(this, node);
        }

        Node operator<(Node node1, Node node2) {
            return node1.lt(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator<(L node1, Node node2) {
            return node2->graph->wrap(node1).lt(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator<(Node node1, R node2) {
            return node1.lt(node1->graph->wrap(node2));
        };

        Node Node::le(Node node) {
            return apply<op::LessThanOrEqual>(this, node);
        }

        Node operator<=(Node node1, Node node2) {
            return node1.le(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator<=(L node1, Node node2) {
            return node2->graph->wrap(node1).le(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator<=(Node node1, R node2) {
            return node1.le(node1->graph->wrap(node2));
        };

        Node Node::eq(Node node) {
            return apply<op::Equals>(this, node);
        }

        Node operator==(Node node1, Node node2) {
            return node1.eq(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator==(L node1, Node node2) {
            return node2->graph->wrap(node1).eq(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator==(Node node1, R node2) {
            return node1.eq(node1->graph->wrap(node2));
        };

        Node Node::neq(Node node) {
            return apply<op::NotEquals>(this, node);
        }

        Node operator!=(Node node1, Node node2) {
            return node1.neq(node2);
        }

        template <typename L, typename = std::enable_if<not std::is_same<L, Node>::value>>
        Node operator!=(L node1, Node node2) {
            return node2->graph->wrap(node1).neq(node2);
        };

        template <typename R, typename = std::enable_if<not std::is_same<R, Node>::value>>
        Node operator!=(Node node1, R node2) {
            return node1.neq(node1->graph->wrap(node2));
        };

        Node Node::approx_eq(Node node, double tol) {
            GraphInPtr graph = unwrap()->graph;
            return graph->derived_node(std::make_shared<op::ApproximatelyEquals>(graph, this, node, tol));
        }

        Node Node::approx_neq(Node node, double tol) {
            return this->approx_eq(node, tol).logical_not();
        }

        Node Node::is_nan() {
            return apply<op::IsNaN>(this);
        }

        Node Node::is_inf() {
            return apply<op::IsInf>(this);
        }

        Node Node::all(){
            return apply<op::All>(this);
        }

        Node Node::any() {
            return apply<op::Any>(this);
        }

        Node Node::select(Node result_true, Node result_false) {
            return unwrap()->graph->derived_node(
                    std::make_shared<op::Select>(unwrap()->graph, this, result_true, result_false));
        }
    }
}
#endif //METADIFF_OPERATORS_LOGICAL_H
