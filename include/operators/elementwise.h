//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_ELEMENTWISE_H
#define METADIFF_CORE_OPERATORS_ELEMENTWISE_H
namespace md{
    namespace core {
        namespace op {

            /** Absolute value */
            class Abs : public MorphElementwiseOperator {
            public:
                Abs(GraphInPtr graph, Node parent) :
                        AbstractOperator("Abs", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Abs>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    Node zero = graph->constant(0);
                    zero->grad_level = my_grad->grad_level;
                    return graph->mul(my_grad, graph->greater_than(parent, zero));
                }
            };

            /** Squre */
            class Square : public FloatUnaryElementwiseOperator {
            public:
                Square(GraphInPtr graph, Node parent) :
                AbstractOperator("Square", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Square>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    Node two = graph->constant(2);
                    two->grad_level = my_grad->grad_level;
                    return graph->mul(my_grad, two, parent);
                }
            };

            /** Square root */
            class Sqrt : public FloatUnaryElementwiseOperator {
            public:
                Sqrt(GraphInPtr graph, Node parent) :
                        AbstractOperator("Sqrt", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sqrt>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    Node two = graph->constant(2);
                    two->grad_level = my_grad->grad_level;
                    return graph->div(my_grad, graph->mul(two, owner));
                }
            };

            /** Exponential */
            class Exp : public FloatUnaryElementwiseOperator {
            public:
                Exp(GraphInPtr graph, Node parent) :
                        AbstractOperator("Exp", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Exp>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, owner);
                }
            };

            /** Logarithm */
            class Log : public FloatUnaryElementwiseOperator {
            public:
                Log(GraphInPtr graph, Node parent) :
                        AbstractOperator("Log", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->div(my_grad, parent);
                }
            };

            /** Logarithm in base 10 */
            class Log10 : public FloatUnaryElementwiseOperator {
            public:
                Log10(GraphInPtr graph, Node parent) :
                        AbstractOperator("Log10", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->div(my_grad, graph->mul(parent, graph->LN_10()));
                }
            };

            /** Logarithm of x + 1 */
            class Log1p : public FloatUnaryElementwiseOperator {
            public:
                Log1p(GraphInPtr graph,
                      Node parent) :
                        AbstractOperator("Log1p", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log1p>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->sigmoid(parent));
                }
            };

            /** Trigonometric sine */
            class Sin : public FloatUnaryElementwiseOperator {
            public:
                Sin(GraphInPtr graph, Node parent) :
                        AbstractOperator("Sin", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sin>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->cos(parent));
                }
            };

            /** Trigonometric cosine */
            class Cos : public FloatUnaryElementwiseOperator {
            public:
                Cos(GraphInPtr graph, Node parent) :
                        AbstractOperator("Cos", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cos>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->neg(graph->sin(parent)));
                }
            };

            /** Trigonometric tangent */
            class Tan : public FloatUnaryElementwiseOperator {
            public:
                Tan(GraphInPtr graph, Node parent) :
                        AbstractOperator("Tan", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tan>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->div(my_grad, graph->square(graph->cos(parent)));
                }
            };

            /** Trigonometric cotangent */
            class Cot : public FloatUnaryElementwiseOperator {
            public:
                Cot(GraphInPtr graph, Node parent) :
                        AbstractOperator("Cot", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cot>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->neg(graph->div(my_grad, graph->square(graph->sin(parent))));
                }
            };

            /** Hyperbolic sine */
            class Sinh : public FloatUnaryElementwiseOperator {
            public:
                Sinh(GraphInPtr graph, Node parent) :
                        AbstractOperator("Sinh", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sinh>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->cosh(parent));
                }
            };

            /** Hyperbolic cosine */
            class Cosh : public FloatUnaryElementwiseOperator {
            public:
                Cosh(GraphInPtr graph, Node parent) :
                        AbstractOperator("Cosh", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cosh>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->mul(my_grad, graph->sinh(parent));
                }
            };

            /** Hyperbolic tangent */
            class Tanh : public FloatUnaryElementwiseOperator {
            public:
                Tanh(GraphInPtr graph, Node parent) :
                        AbstractOperator("Tanh", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tanh>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    Node derivative = graph->neg(graph->constant(1), graph->square(owner));
                    return graph->mul(my_grad, derivative);
                }
            };

            /** Hyperbolic cotangent */
            class Coth : public FloatUnaryElementwiseOperator {
            public:
                Coth(GraphInPtr graph, Node parent) :
                        AbstractOperator("Coth", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Coth>(graph, ancestors[0]);
                }

                Node backward_diff(Node my_grad, short index) {
                    auto one = graph->constant(1);
                    Node derivative = graph->neg(one, graph->square(owner));
                    return graph->mul(my_grad, derivative);
                }
            };

            /** Takes the first input to the power of the second elementwise */
            class Pow : public BinaryFloatElementwiseOperator {
            public:
                Pow(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator("Pow", graph), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Pow>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff(Node my_grad, short index) {
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
    }
}

#endif //METADIFF_ELEMENTWISE_FUNC_H
