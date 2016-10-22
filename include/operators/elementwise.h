//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_ELEMENTWISE_H
#define METADIFF_CORE_OPERATORS_ELEMENTWISE_H
namespace md{
    namespace core {
        namespace op {

            /** Explicit operator for square */
            class Square : public UnaryOperator {
            public:
                Square(GraphInPtr graph, Node parent) :
                        UnaryOperator("Square", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Square>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    Node two = graph->constant(2);
                    two->grad_level = my_grad->grad_level;
                    return graph->mul(my_grad, two, parent);
                }
            };

            /** Square root */
            class Sqrt : public FloatUnaryOperator {
            public:
                Sqrt(GraphInPtr graph, Node parent) :
                FloatUnaryOperator("Sqrt", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sqrt>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    Node two = graph->constant(2);
                    two->grad_level = my_grad->grad_level;
                    return graph->div(my_grad, graph->mul(two, owner));
                }
            };

            /** Exponential */
            class Exp : public FloatUnaryOperator {
            public:
                Exp(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Exp", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Exp>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, owner);
                }
            };

            /** Logarithm */
            class Log : public FloatUnaryOperator {
            public:
                Log(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Log", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->div(my_grad, parent);
                }
            };

            /** Logarithm in base 10 */
            class Log10 : public FloatUnaryOperator {
            public:
                Log10(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Log10", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->div(my_grad, graph->mul(parent, graph->LN_10()));
                }
            };

            /** Absolute value */
            class Abs : public UnaryOperator {
            public:
                Abs(GraphInPtr graph, Node parent) :
                        UnaryOperator("Abs", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Abs>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    Node zero = graph->constant(0);
                    zero->grad_level = my_grad->grad_level;
                    return graph->mul(my_grad, graph->greater_than(parent, zero));
                }
            };

            /** Logarithm of x + 1 */
            class Log1p : public FloatUnaryOperator {
            public:
                Log1p(GraphInPtr graph,
                      Node parent) :
                        FloatUnaryOperator("Log1p", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log1p>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->sigmoid(parent));
                }
            };

            /** Trigonometric sine function */
            class Sin : public FloatUnaryOperator {
            public:
                Sin(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Sin", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sin>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->cos(parent));
                }
            };

            /** Trigonometric cosine function */
            class Cos : public FloatUnaryOperator {
            public:
                Cos(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Cos", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cos>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->neg(graph->sin(parent)));
                }
            };

            /** Trigonometric tangent function */
            class Tan : public FloatUnaryOperator {
            public:
                Tan(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Tan", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tan>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->div(my_grad, graph->square(graph->cos(parent)));
                }
            };

            /** Trigonometric cotangent function */
            class Cot : public FloatUnaryOperator {
            public:
                Cot(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Cot", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cot>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->neg(graph->div(my_grad, graph->square(graph->sin(parent))));
                }
            };

            /** Hyperbolic sine function */
            class Sinh : public FloatUnaryOperator {
            public:
                Sinh(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Sinh", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sinh>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->cosh(parent));
                }
            };

            /** Hyperbolic cosine function */
            class Cosh : public FloatUnaryOperator {
            public:
                Cosh(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Cosh", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cosh>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->sinh(parent));
                }
            };

            /** Hyperbolic tangent function */
            class Tanh : public FloatUnaryOperator {
            public:
                Tanh(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Tanh", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tanh>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    Node derivative = graph->neg(graph->constant(1), graph->square(owner));
                    return graph->mul(my_grad, derivative);
                }
            };

            /** Hyperbolic cotangent function */
            class Coth : public FloatUnaryOperator {
            public:
                Coth(GraphInPtr graph, Node parent) :
                        FloatUnaryOperator("Coth", graph, parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Coth>(graph, ancestors[0]);
                }

                Node get_parent_grad(Node my_grad, short index) {
                    auto one = graph->constant(1);
                    Node derivative = graph->neg(one, graph->square(owner));
                    return graph->mul(my_grad, derivative);
                }
            };

            /** Takes the first input to the power of the second elementwise */
            class Pow : public ElementwiseBinary {
            public:
                Pow(GraphInPtr graph, Node parent1, Node parent2) :
                        ElementwiseBinary("Pow", graph, parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Pow>(graph, ancestors[0], ancestors[1]);
                }

                dataType get_data_type() const {
                    return graph->max_float;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    Node product = graph->mul(my_grad, owner);
                    if (index == 0) {
                        Node factor = graph->div(parent2, parent1);
                        return graph->mul(product, factor);
                    } else {
                        return graph->mul(product, graph->log(parent1));
                    }
                }
            };
        }

//        namespace core {
//            Node Node::square() {
//                return apply<op::Square>(this);
//            }
//
//            Node Node::exp() {
//                return apply<op::Exp>(this);
//            }
//
//            Node Node::sigmoid() {
//                GraphInPtr graph = unwrap()->graph;
//                return graph->constant_value(1.0) / (graph->constant_value(1.0) + this->neg().exp());
//            }
//
//            Node Node::log() {
//                return apply<op::Log>(this);
//            }
//
//            Node Node::log10() {
//                return apply<op::Log10>(this);
//            }
//
//            Node Node::abs() {
//                return apply<op::Abs>(this);
//            }
//
//            Node Node::log1p() {
//                return apply<op::Log1p>(this);
//            }
//
//            Node Node::softplus(int threshold) {
//                if (threshold <= 0) {
//                    return exp().log1p();
//                } else {
//                    Node condition = this->ge(unwrap()->graph->constant_value(threshold));
//                    return condition.select(this, this->exp().log1p());
//                }
//            }
//
//            Node Node::sin() {
//                return apply<op::Sin>(this);
//            }
//
//            Node Node::cos() {
//                return apply<op::Cos>(this);
//            }
//
//            Node Node::tan() {
//                return apply<op::Tan>(this);
//            }
//
//            Node Node::cot() {
//                return apply<op::Cot>(this);
//            }
//
//            Node Node::sinh() {
//                return apply<op::Sinh>(this);
//            }
//
//            Node Node::cosh() {
//                return apply<op::Cosh>(this);
//            }
//
//            Node Node::tanh() {
//                return apply<op::Tanh>(this);
//            }
//
//            Node Node::coth() {
//                return apply<op::Coth>(this);
//            }
//
//            Node Node::pow(Node power) {
//                std::shared_ptr<NodeInternal> ptr = unwrap();
//                return ptr->graph->derived_node(std::make_shared<op::Pow>(ptr->graph, this, power));
//            }
//        }
    }
}

#endif //METADIFF_ELEMENTWISE_FUNC_H
