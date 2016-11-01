//
// Created by alex on 25/10/16.
//

#ifndef METADIFF_CORE_OPERATORS_RANDOM_H
#define METADIFF_CORE_OPERATORS_RANDOM_H

namespace md{
    namespace core{
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
#endif //METADIFF_CORE_OPERATORS_RANDOM_H
