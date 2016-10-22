//
// Created by alex on 21/10/16.
//

#ifndef METADIFF_CORE_OPERATORS_DEBUG_H
#define METADIFF_CORE_OPERATORS_DEBUG_H

namespace md{
    namespace core{
        namespace op{

            /** A Monitor attached to parent1, reporting parent2 */
            class Monitor: public AbstractOperator{
            public:
                Node anchor;
                Node reported;
                monitorRole role;
                std::string message;
                Monitor(GraphInPtr graph, Node anchor, Node reported,
                        monitorRole role, std::string message):
                        AbstractOperator("Monitor", graph),
                        anchor(anchor), reported(reported), role(role), message(message) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Monitor>(graph, ancestors[0], ancestors[1], role, message);
                }

                dataType get_data_type() const {
                    return anchor->data_type;
                }

                Shape get_shape() const{
                    return anchor->shape;
                }

                nodeType get_node_type() const{
                    return anchor->node_type;
                }

                unsigned short get_grad_level() const{
                    return anchor->grad_level;
                }

                NodeVec get_parents() const{
                    return {anchor};
                }

                NodeVec get_arguments() const{
                    return {reported};
                }

                Node get_parent_grad(Node my_grad, short index){
                    return my_grad;
                }

                bool equals(Operator const op) const {
                    return false;
                }
            };

            /**
             * Guards against very large values
             */
            class Guard: public UnaryOperator{
            public:
                Guard(GraphInPtr graph, Node parent):
                        UnaryOperator("Guard", graph, parent) {};


                Node get_parent_grad(Node my_grad, short index){
                    return my_grad;
                }

                bool equals(Operator const op) const {
                    return false;
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_DEBUG_H
