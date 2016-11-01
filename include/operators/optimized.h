//
// Created by alex on 19/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_OPTIMIZED_H
#define METADIFF_CORE_OPERATORS_OPTIMIZED_H

namespace md {
    namespace core {
        namespace op {

            /** Logarithm of exp(x) + 1 */
            class Softplus : public FloatUnaryElementwiseOperator {
            public:
                double threshold;
                Softplus(GraphInPtr graph, Node parent, double threshold = 50) :
                        AbstractOperator(graph, "Softplus"), UnaryOperator(parent),
                        threshold(threshold){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Softplus>(graph, ancestors[0], threshold);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return mul(my_derivative, sigmoid(parent));
                }

            };

            /** Logarithm of sum exp(x_i) */
            class LogSumExp : public FloatReductionOperator {
            public:
                double threshold;
                LogSumExp(GraphInPtr graph, Node parent, Axes axes, double threshold = 10) :
                        AbstractOperator(graph, "LogSumExp"), UnaryOperator(parent), ReductionOperator(axes),
                        threshold(threshold){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogSumExp>(graph, ancestors[0], axes, threshold);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return mul(my_derivative, softmax(parent));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto s = softmax(parent_derivatives[index]);
                    return sum(mul(parent_derivatives[index], s), axes);
                }
            };


            /** Logistic function */
            class Sigmoid : public FloatUnaryElementwiseOperator {
            public:
                Sigmoid(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Sigmoid"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sigmoid>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return mul(my_derivative, owner, neg(graph->constant(1), owner));
                }
            };

            /** Softmax function */
            class Softmax : public FloatUnaryElementwiseOperator {
            public:
                Axes axes;
                Softmax(GraphInPtr graph, Node parent, Axes axes) :
                        AbstractOperator(graph, "Softmax"), UnaryOperator(parent),
                        axes(axes) {
//                    if(parent.dims() < 1){
//                        // TODO raise an error
//                    }
//                    axis = parent.dims() - 1;
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Softmax>(graph, ancestors[0], axes);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    auto vtg = sum(mul(my_derivative, owner), axes);
                    return neg(mul(owner, my_derivative), mul(owner, vtg));
                }
            };

            /**
             * The Binary cross entropy of p and q = sigmoid(x)
             */
            class BinaryCrossEntropyLogits: public BinaryFloatElementwiseOperator {
            public:
                Node softplus_x, softplus_minus_x;
                BinaryCrossEntropyLogits(GraphInPtr graph, Node p, Node x):
                        AbstractOperator(graph, "BinaryCrossEntropyLogits"), BinaryOperator(p, x){
                    softplus_x = softplus(x);
                    softplus_minus_x = softplus(neg(x));
                }

                Operator copy_to(GraphInPtr graph, std::vector <Node> ancestors) const {
                    return std::make_shared<BinaryCrossEntropyLogits>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    // Parents - p, x
                    // Node computes f = - p * log(q) - (1-p) * log(1-q)
                    // q = sigmoid(x) => log(q) = - softplus(-x), log(1-q) = - softplus(x)
                    // f = p * softplus(-x) + (1-p) * softplus(x)
                    // df/dp = softplus(-x) - softplus(x)
                    // df/dx = p * sigmoid(-x) * (-1) + (1-p) * sigmoid(x) =
                    // = - p (sigmoid(-x) + sigmoid(x)) + sigmoid(x) =
                    // = sigmoid(x) - p
                    if (index == 0) {
                        return mul(my_derivative, neg(softplus_minus_x, softplus_x));
                    } else {
                        return mul(my_derivative, neg(sigmoid(parent2), parent1));
                    }
                }
            };

            /**
             * Categorical crossnetropy p and q = softmax(x)
             * TODO Should check if the p is one less dimention then it is class encodings (indexes)
             */
            class CategoricalCrossEntropyLogits: public BinaryFloatElementwiseOperator {
            public:
                Node log_z;
                CategoricalCrossEntropyLogits(GraphInPtr graph, Node p, Node x):
                        AbstractOperator(graph, "CategoricalCrossEntropyLogits"), BinaryOperator(p, x){
                    log_z = log_sum_exp(x);
                }

                Operator copy_to(GraphInPtr graph, std::vector <Node> ancestors) const {
                    return std::make_shared<CategoricalCrossEntropyLogits>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff_parent(Node my_derivative, int index) {
                    // Parents - p, x
                    // Node computes f = - sum[p_i log e^x_i/Z] = - sum[p_i(x_i - log(Z))
                    // df/dp_i = log(Z) - x_i
                    // df/dx_i = p_i - softmax(x_i)
                    if (index == 0) {
                        return mul(my_derivative, neg(log_z, parent2));
                    } else {
                        return mul(my_derivative, neg(parent1, softmax(parent2)));
                    }
                }
            };
        }
    }
};


#endif //METADIFF_CORE_OPERATORS_OPTIMIZED_H
