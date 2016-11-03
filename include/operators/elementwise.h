//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_ELEMENTWISE_H
#define METADIFF_GRAPH_IR_OPERATORS_ELEMENTWISE_H
namespace md{
    using namespace api;
    namespace gir {
        namespace op {

            /** Absolute value */
            class Abs : public MorphElementwiseOperator {
            public:
                Abs(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Abs"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Abs>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    auto zero = graph->constant(0);
                    auto factor = cast(greater_than(parent, zero), result->data_type);
                        factor = neg(mul(graph->constant(2), factor), graph->constant(1));
                    return mul(my_derivative, factor);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return backward_diff_parent(parent_derivatives[index], index);
                }
            };

            /** Squre */
            class Square : public FloatUnaryElementwiseOperator {
            public:
                Square(GraphInPtr graph, Node parent) :
                AbstractOperator(graph, "Square"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Square>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    Node two = graph->constant(2);
                    return mul(my_derivative, two, parent);
                }
            };

            /** Square root */
            class Sqrt : public FloatUnaryElementwiseOperator {
            public:
                Sqrt(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Sqrt"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sqrt>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    Node two = graph->constant(2);
                    return div(my_derivative, mul(two, result));
                }
            };

            /** Exponential */
            class Exp : public FloatUnaryElementwiseOperator {
            public:
                Exp(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Exp"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Exp>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, result);
                }
            };

            /** Logarithm */
            class Log : public FloatUnaryElementwiseOperator {
            public:
                Log(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Log"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return div(my_derivative, parent);
                }
            };

            /** Logarithm in base 10 */
            class Log10 : public FloatUnaryElementwiseOperator {
            public:
                Log10(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Log10"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return div(my_derivative, mul(parent, graph->LN_10()));
                }
            };

            /** Logarithm of x + 1 */
            class Log1p : public FloatUnaryElementwiseOperator {
            public:
                Log1p(GraphInPtr graph,
                      Node parent) :
                        AbstractOperator(graph, "Log1p"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Log1p>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, sigmoid(parent));
                }
            };

            /** Trigonometric sine */
            class Sin : public FloatUnaryElementwiseOperator {
            public:
                Sin(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Sin"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sin>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return my_derivative * cos(parent);
                }
            };

            /** Trigonometric cosine */
            class Cos : public FloatUnaryElementwiseOperator {
            public:
                Cos(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Cos"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cos>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, neg(sin(parent)));
                }
            };

            /** Trigonometric tangent */
            class Tan : public FloatUnaryElementwiseOperator {
            public:
                Tan(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Tan"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tan>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return div(my_derivative, square(cos(parent)));
                }
            };

            /** Trigonometric cotangent */
            class Cot : public FloatUnaryElementwiseOperator {
            public:
                Cot(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Cot"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cot>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return neg(div(my_derivative, square(sin(parent))));
                }
            };

            /** Hyperbolic sine */
            class Sinh : public FloatUnaryElementwiseOperator {
            public:
                Sinh(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Sinh"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sinh>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, cosh(parent));
                }
            };

            /** Hyperbolic cosine */
            class Cosh : public FloatUnaryElementwiseOperator {
            public:
                Cosh(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Cosh"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Cosh>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    return mul(my_derivative, sinh(parent));
                }
            };

            /** Hyperbolic tangent */
            class Tanh : public FloatUnaryElementwiseOperator {
            public:
                Tanh(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Tanh"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Tanh>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    Node derivative = neg(graph->constant(1), square(result));
                    return mul(my_derivative, derivative);
                }
            };

            /** Hyperbolic cotangent */
            class Coth : public FloatUnaryElementwiseOperator {
            public:
                Coth(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Coth"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Coth>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    auto one = graph->constant(1);
                    Node derivative = neg(one, square(result));
                    return mul(my_derivative, derivative);
                }
            };

            /** Takes the first input to the power of the second elementwise */
            class Pow : public BinaryFloatElementwiseOperator {
            public:
                Pow(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator(graph, "Pow"), BinaryOperator(parent1, parent2) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Pow>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    Node product = mul(my_derivative, result);
                    if (index == 0) {
                        Node factor = div(parent2, parent1);
                        return mul(product, factor);
                    } else {
                        return mul(product, log(parent1));
                    }
                }
            };
        }
    }
}

#endif //METADIFF_GRAPH_IR_ELEMENTWISE_FUNC_H
