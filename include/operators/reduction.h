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

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->broadcast(my_derivative, parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->sum(parent_derivatives[index], axes);
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

                Node backward_diff_parent(Node my_derivative, short index){
                    return graph->div(my_derivative, parent);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    auto factor = graph->div(parent_derivatives[index], parent);
                    return graph->sum(graph->mul(owner, factor), axes);
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

                Node backward_diff_parent(Node my_derivative, short index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    return graph->broadcast(graph->div(my_derivative, graph->wrap(N)), parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    return graph->mean(parent_derivatives[index], axes);
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

                Node backward_diff_parent(Node my_derivative, short index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    auto two = graph->constant(2);
                    auto centered = graph->neg(parent, graph->mean(parent, axes));
                    return graph->div(graph->mul(my_derivative, two, centered), graph->wrap(N));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, short index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    auto two = graph->constant(2);
                    auto centered = graph->neg(parent, graph->mean(parent, axes));
                    auto centered_d = graph->neg(parent_derivatives[index], graph->mean(parent_derivatives[index], axes));
                    return graph->mean(graph->mul(two, centered, centered_d), axes);
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
