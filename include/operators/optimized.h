//
// Created by alex on 19/12/15.
//

#ifndef METADIFF_OPERATORS_OPTIMIZED_H
#define METADIFF_OPERATORS_OPTIMIZED_H

namespace metadiff {
    namespace op {
        using namespace core;
        using namespace exceptions;

        /** Binary cross-entropy between p an sigmoid(x) */
        class BinaryCrossEntropyLogit : public ElementwiseBinary {
        public:
            Node softplus_x, softplus_mx;

            BinaryCrossEntropyLogit(GraphInPtr graph, Node p, Node x) :
                    ElementwiseBinary("BinCrossEntropyLogit", graph, p, x) {
                softplus_x = x.softplus();
                softplus_mx = x.neg().softplus();
            }

            BinaryCrossEntropyLogit(GraphInPtr graph, Node p, Node x,
                                    Node softplus_x, Node softplus_mx) :
                    ElementwiseBinary("BinCrossEntropyLogit", graph, p, x),
                    softplus_x(softplus_x),
                    softplus_mx(softplus_mx) {};

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
                return std::make_shared<BinaryCrossEntropyLogit>(graph,
                                                                 ancestors[0], ancestors[1],
                                                                 ancestors[2], ancestors[3]);
            }

            dType get_dtype() const{
                return graph->max_float;
            }

            NodeVec get_arguments() const {
                return {softplus_x, softplus_mx};
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                // Parents - p, x
                // Arguments - sf(x), sf(-x)
                // Node computes f = - p * log(q) - (1-p) * log(1-q)
                // log(q) = -sf(-x), log(1-q) = -sf(x)
                // Node computes f = p * sf(-x) + (1 - p)*sf(x) = p*(sf(-x)-sf(x)) + sf(x)
                // dE/dp = dE * (sf(-x)-sf(x))
                // dE/dx = dE * (q-p) = dE * (sigmoid(x) - p)
                if (index == 0) {
                    return my_grad * (softplus_mx - softplus_x);
                } else {
//                    std::cout << my_grad->shape << std::endl
//                    << my_grad->op->name << std::endl
//                    << my_grad->op->get_shape() << std::endl
//                    << my_grad->op->get_parents()[0]->op->name << std::endl
//                    << my_grad->op->get_parents()[0]->shape << std::endl;
//                    auto a = - parent1;
//                    std::cout << a->shape << std::endl;
//                    auto b = parent2.sigmoid();
//                    std::cout << b->shape << std::endl;
//                    auto c = a + b;
//                    std::cout << c->shape << std::endl;
                    return my_grad * (parent2.sigmoid() - parent1);
                }
            }
        };
    }
    namespace core{
        Node Node::binary_cross_entropy_logit(Node node) {
            return apply<op::BinaryCrossEntropyLogit>(this, node);
        }

        Node Node::relu() {
            // TODO Check if this is optimal?
            return unwrap()->graph->constant_value(0.5) * (this + abs());
        }
    }
};


#endif //METADIFF_OPERATORS_OPTIMIZED_H
