//
// Created by alex on 11/10/16.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_REDUCTION_H
#define METADIFF_GRAPH_IR_OPERATORS_REDUCTION_H
namespace md{
    namespace gir{
        namespace op{
            /** Summation along an axes */
            class Sum : public MorphReductionOperator {
            public:
                Sum(GraphInPtr graph,
                    Node parent,
                    Axes axes) :
                        AbstractOperator(graph, "Sum"), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Sum>(graph, ancestors[0], axes);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return api::broadcast(my_derivative, parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return sum(parent_derivatives[index], axes);
                }
            };

            /** Product along axes */
            class Product : public MorphReductionOperator {
            public:
                Product(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator(graph, "Product"), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Product>(graph, ancestors[0], axes);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return div(my_derivative, parent);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    auto factor = div(parent_derivatives[index], parent);
                    return sum(mul(result, factor), axes);
                }
            };

            /** Mean along axes */
            class Mean : public FloatReductionOperator {
            public:
                Mean(GraphInPtr graph,
                     Node parent,
                     Axes axes) :
                        AbstractOperator(graph, "Mean"), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mean>(graph, ancestors[0], axes);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    return api::broadcast(div(my_derivative, graph->sym_int_node(N)), parent->shape);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return mean(parent_derivatives[index], axes);
                }
            };

            /** Variance along axes */
            class Variance : public FloatReductionOperator {
            public:
                Variance(GraphInPtr graph,
                         Node parent,
                         Axes axes) :
                        AbstractOperator(graph, "Variance"), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mean>(graph, ancestors[0], axes);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    auto two = graph->constant(2);
                    auto centered = neg(parent, mean(parent, axes));
                    return div(mul(my_derivative, two, centered), graph->sym_int_node(N));
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    SymInt N = 0;
                    for(auto i=0; i < axes.size(); ++i){
                        N = N * axes[i];
                    }
                    auto two = graph->constant(2);
                    auto centered = neg(parent, mean(parent, axes));
                    auto centered_d = neg(parent_derivatives[index], mean(parent_derivatives[index], axes));
                    return mean(mul(two, centered, centered_d), axes);
                }
            };

            /** Reducation with operator AND (&&) along an axes */
            class AllTrue : public LogicalReductionOperator {
            public:
                AllTrue(GraphInPtr graph,
                        Node parent,
                        Axes axes) :
                        AbstractOperator(graph, "AllTrue"), UnaryOperator(parent), ReductionOperator(axes) {};

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
                        AbstractOperator(graph, "AnyTrue"), UnaryOperator(parent), ReductionOperator(axes) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<AnyTrue>(graph, ancestors[0], axes);
                }
            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_REDUCTION_H
