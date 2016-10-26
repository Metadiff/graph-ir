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
                        AbstractOperator("Softplus", graph), UnaryOperator(parent),
                        threshold(threshold){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Softplus>(graph, ancestors[0], threshold);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->mul(my_derivative, graph->sigmoid(parent));
                }

            };

            /** Logarithm of sum exp(x_i) */
            class LogSumExp : public FloatReductionOperator {
            public:
                double threshold;
                LogSumExp(GraphInPtr graph, Node parent, Axes axes, double threshold = 10) :
                        AbstractOperator(name, graph), UnaryOperator(parent), ReductionOperator(axes),
                        threshold(threshold){};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<LogSumExp>(graph, ancestors[0], axes, threshold);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->mul(my_derivative, graph->softmax(parent));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    auto softmax = graph->softmax(parent_derivatives[index]);
                    return graph->sum(graph->mul(parent_derivatives[index], softmax), axes);
                }
            };


            /** Logistic function */
            class Sigmoid : public FloatUnaryElementwiseOperator {
            public:
                Sigmoid(GraphInPtr graph, Node parent) :
                        AbstractOperator("Sigmoid", graph), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sigmoid>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->mul(my_derivative, owner, graph->neg(graph->constant(1), owner));
                }
            };

            /** Softmax function */
            class Softmax : public FloatUnaryElementwiseOperator {
            public:
                short axis;
                Softmax(GraphInPtr graph, Node parent) :
                        AbstractOperator("Softmax", graph), UnaryOperator(parent) {
                    if(parent.dims() < 1){
                        // TODO raise an error
                    }
                    axis = parent.dims() - 1;
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Softmax>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    auto vtg = graph->sum(graph->mul(my_derivative, owner), axis);
                    return graph->neg(graph->mul(owner, my_derivative), graph->mul(owner, vtg));
                }
            };

            /**
             * The Binary cross entropy of p and q = sigmoid(x)
             */
            class BinaryCrossEntropyLogits: public BinaryFloatElementwiseOperator {
            public:
                Node softplus_x, softplus_minus_x;
                BinaryCrossEntropyLogits(GraphInPtr graph, Node p, Node x):
                        AbstractOperator("BinaryCrossEntropyLogits", graph), BinaryOperator(p, x){
                    softplus_x = graph->softplus(x);
                    softplus_minus_x = graph->softplus(graph->neg(x));
                }

                Operator copy_to(GraphInPtr graph, std::vector <Node> ancestors) const {
                    return std::make_shared<BinaryCrossEntropyLogits>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff_parent(Node my_derivative, short index){
                    // Parents - p, x
                    // Node computes f = - p * log(q) - (1-p) * log(1-q)
                    // q = sigmoid(x) => log(q) = - softplus(-x), log(1-q) = - softplus(x)
                    // f = p * softplus(-x) + (1-p) * softplus(x)
                    // df/dp = softplus(-x) - softplus(x)
                    // df/dx = p * sigmoid(-x) * (-1) + (1-p) * sigmoid(x) =
                    // = - p (sigmoid(-x) + sigmoid(x)) + sigmoid(x) =
                    // = sigmoid(x) - p
                    if (index == 0) {
                        return graph->mul(my_derivative, graph->neg(softplus_minus_x, softplus_x));
                    } else {
                        return graph->mul(my_derivative, graph->neg(graph->sigmoid(parent2), parent1));
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
                        AbstractOperator("CategoricalCrossEntropyLogits", graph), BinaryOperator(p, x){
                    log_z = graph->log_sum_exp(x);
                }

                Operator copy_to(GraphInPtr graph, std::vector <Node> ancestors) const {
                    return std::make_shared<CategoricalCrossEntropyLogits>(graph, ancestors[0], ancestors[1]);
                }

                Node backward_diff_parent(Node my_derivative, short index) {
                    // Parents - p, x
                    // Node computes f = - sum[p_i log e^x_i/Z] = - sum[p_i(x_i - log(Z))
                    // df/dp_i = log(Z) - x_i
                    // df/dx_i = p_i - softmax(x_i)
                    if (index == 0) {
                        return graph->mul(my_derivative, graph->neg(log_z, parent2));
                    } else {
                        return graph->mul(my_derivative, graph->neg(parent1, graph->softmax(parent2)));
                    }
                }
            };
        }
    }
};


#endif //METADIFF_CORE_OPERATORS_OPTIMIZED_H
