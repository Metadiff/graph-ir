//
// Created by alex on 11/10/16.
//

#ifndef METADIFF_CORE_OPERATORS_REDUCTIONS_H
#define METADIFF_CORE_OPERATORS_REDUCTIONS_H
namespace md{
    namespace core{
        namespace op{
            /** Summation along an axes */
            class Sum : public MorphReductionOperator {
            public:
                Sum(GraphInPtr graph,
                    Node parent,
                    Axes axes) :
                AbstractOperator("Sum", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sum>(graph, ancestors[0], axes);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->broadcast(my_grad, parent->shape);
                }
            };

            /** Product along axes */
            class Product : public MorphReductionOperator {
            public:
                Product(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator("Product", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Product>(graph, ancestors[0], axes);
                }

                Node backward_diff(Node my_grad, short index) {
                    return graph->div(my_grad, parent);
                }
            };

            /** Mean along axes */
            class Mean : public FloatReductionOperator {
            public:
                Mean(GraphInPtr graph,
                     Node parent,
                     Axes axes) :
                        AbstractOperator("Mean", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mean>(graph, ancestors[0], axes);
                }

                Node backward_diff(Node my_grad, short index) {
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    return graph->broadcast(graph->div(my_grad, graph->wrap(N)), parent->shape);
                }
            };

            /** Variance along axes */
            class Variance : public FloatReductionOperator {
            public:
                Variance(GraphInPtr graph,
                     Node parent,
                     Axes axes) :
                        AbstractOperator("Variance", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mean>(graph, ancestors[0], axes);
                }

                Node backward_diff(Node my_grad, short index) {
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    return graph->broadcast(graph->div(my_grad, graph->wrap(N)), parent->shape);
                }
            };

            /** Reducation with operator AND (&&) along an axes */
            class AllTrue : public LogicalReductionOperator {
            public:
                AllTrue(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator("AllTrue", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<AllTrue>(graph, ancestors[0], axes);
                }

                dataType get_data_type() const {
                    return b8;
                };

                Node backward_diff(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Reducation with operator OR (||) along an axes */
            class AnyTrue : public LogicalReductionOperator {
            public:
                AnyTrue(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator("AnyTrue", graph), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<AnyTrue>(graph, ancestors[0], axes);
                }

                dataType get_data_type() const {
                    return b8;
                };

                Node backward_diff(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            template <typename T>
            Node apply_reduction(GraphInPtr g,Node node, Axes axes){
                // If scalar do nothing
                if(node.dims() == 0 or axes.size() == 0){
                    return g->alias(node);
                }
                auto op = std::make_shared<T>(g, node, axes);
                return g->derived_node(op);
            }

            template <typename T>
            Node apply_reduction(GraphInPtr g, Node node, short axis){
                if(axis != auto_infer){
                    return g->sum(node, {axis});
                }
                return apply_reduction<T>(g, node, auto_infer_axes(node));
            }
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_REDUCTIONS_H
