//
// Created by alex on 25/10/16.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_RANDOM_H
#define METADIFF_GRAPH_IR_OPERATORS_RANDOM_H

namespace md{
    namespace gir{
        namespace op {
            /** Node filled with uniformly distributed random numbers */
            class RandomUniform : public ConstantOperator {
            public:
                Shape shape;

                RandomUniform(GraphInPtr graph, Shape shape) :
                        AbstractOperator(graph, "RandomUniform"), ConstantOperator(graph->props.max_float),
                        shape(shape) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<RandomUniform>(graph, shape);
                }

                Shape get_shape() const {
                    return shape;
                }

            };

            /** Node filled with normally distributed random numbers */
            class RandomNormal : public ConstantOperator {
            public:
                Shape shape;

                RandomNormal(GraphInPtr graph, Shape shape) :
                        AbstractOperator(graph, "RandomNormal"), ConstantOperator(graph->props.max_float),
                        shape(shape) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<RandomNormal>(graph, shape);
                }

                Shape get_shape() const {
                    return shape;
                }

            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_OPERATORS_RANDOM_H
