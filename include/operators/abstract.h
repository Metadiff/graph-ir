//
// Created by alex on 04/05/16.
//

#ifndef METADIFF_GRAPH_IR_OPERATORS_ABSTRACT_H
#define METADIFF_GRAPH_IR_OPERATORS_ABSTRACT_H

namespace md {
    namespace gir {
        namespace op {
            using namespace api;

            /** The abstract class for all operatros */
            class AbstractOperator {
            protected:
                /** Pointer to the owning GraphInternal */
                GraphInPtr const graph;

                /** This should never be called directly, it exists ONLY for virtual inheritance purposes */
                AbstractOperator():  graph(nullptr), name("") {};
            public:
                /** Unique name of the concrete Operator class */
                std::string const name;
                /** Pointer to the output Node of the operator */
                Node result;

                /** Note that the output Node should always be set after the construction of the Operator */
                AbstractOperator(GraphInPtr const graph, std::string const name) :
                        graph(graph), name(name){};

                /** @brief Copies the Operator to the new graph with the provided ancestors
                 *
                 * @param graph
                 * @param ancestors
                 * @return
                 */
                virtual Operator copy_to(GraphInPtr graph, NodeVec ancestors) const = 0;

                /** @brief  Calculates the DataType for the output Node based on its ancestors
                 *
                 * @return
                 */
                virtual DataType get_data_type() const = 0;

                /** @brief Calculates the Shape for the output Node based on its ancestors
                 *
                 * @return
                 */
                virtual Shape get_shape() const = 0;

                /** @brief Calculates if the output Node is input dependent based on its ancestors
                 * The default implementation returns true if any of the ancestors is input dependent.
                 *
                 * @return
                 */
                virtual bool is_input_dependent() const;

                /** @brief Calculates if the output Node is differentiable based on its ancestors
                 * The default implementation returns true if any of the parents are differentiable
                 *
                 * @return
                 */
                virtual bool is_differentiable() const;

                /** @brief Calculates the grad level of the output Node
                 * The default implementation returns the maximum of the grad levels of its ancestors
                 *
                 * @return
                 */
                unsigned int get_grad_level() const;

                /** @brief  Returns all of the parents of this operator in order
                 * Parents are defined as all ancestors which affect the node in a differentiable way
                 *
                 * @return
                 */
                virtual NodeVec get_parents() const  = 0;

                /** @brief  Returns all of the arguments of this operator in order
                 * Parents are defined as all ancestors which affect the node in a non-differentiable way
                 *
                 * @return
                 */
                virtual NodeVec get_arguments() const;

                /** @brief Returns the union of the parents and arguments
                 *
                 * @return
                 */
                virtual NodeVec get_ancestors() const;

                /** @brief Generates and sends the backward differentiation messages to all parents using the messages incoming to the output Node
                 *
                 * @param messages
                 * @param flow_tree
                 */
                void backward_diff(std::vector<NodeVec> &messages, std::vector<bool>& flow_tree);

                /** @brief Returns the backward diferentiation message to the parent at the index specified
                 *
                 * @param my_derivative
                 * @param index
                 * @return
                 */
                virtual Node backward_diff_parent(Node my_derivative, int index) = 0;

                /** @brief Combines the backward differentiation messages in coming to this Node
                 * The default implementation sums all of the messages
                 *
                 * @param derivatives
                 * @return
                 */
                virtual Node backward_diff_combine(NodeVec & derivatives) const;

                /** @brief Combines all of the parent_derivatives apropirately and computes the derivative of the output Node
                 *
                 * @param messages
                 * @param flow_tree
                 */
                void forward_diff(NodeVec & parent_derivatives);

                /** @brief Returns the forward diferentiation message from the parent at the index specified
                 *
                 * @param my_derivative
                 * @param index
                 * @return
                 */
                virtual Node forward_diff_parent(NodeVec & parent_derivatives, int index) = 0;

                /** @brief Combines the forward differentiation messages in coming to this Node
                 * The default implementation sums all of the messages
                 *
                 * @param derivatives
                 * @return
                 */
                virtual Node forward_diff_combine(NodeVec & parent_derivatives) const;

                /** @brief Returns whether the Operator op is symbolically equivalent to this one.
                 * TODO This together with find_same_node() and symbolic_equals() still need to be implemented well
                 *
                 * @param op
                 * @return
                 */
                virtual bool equals(Operator const op) const{
                    return false;
                }
            };


            class NonDifferentiableOperator: public virtual AbstractOperator {
                bool is_differentiable() const{
                    return false;
                }
            };

            class OrphanOperator: public virtual AbstractOperator {
            public:

                NodeVec get_parents() const {
                    return NodeVec {};
                }

                Node backward_diff_parent(Node my_grad, int index) {
                    op_logger(name)->error("Calling backward_diff unexpectedly.");
                    throw InternalGraphError(name, "Calling backward_diff unexpectedly.");
                }

                Node forward_diff_parent(NodeVec & parent_derivative, int index){
                    return Node();
                }
            };

            class InputOperator: public virtual OrphanOperator {
            public:
                bool is_input_dependent() const{
                    return true;
                }

                bool is_differentiable() const{
                    return true;
                }
            };

            class ConstantOperator : public virtual OrphanOperator,
                                     public virtual NonDifferentiableOperator {
            public:
                DataType data_type;

                ConstantOperator(DataType data_type):
                        data_type(data_type) {};

                DataType get_data_type() const {
                    return data_type;
                }
            };

            class LogicalOperator: public virtual NonDifferentiableOperator{
            public:
                DataType get_data_type() const {
                    return b8;
                };

                Node backward_diff_parent(Node my_derivative, int index){
                    op_logger(name)->error("Calling backward_diff unexpectedly.");
                    throw InternalGraphError(name, "Calling backward_diff unexpectedly.");
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return Node();
                }
            };

            class IntegerOperator: public virtual AbstractOperator{
            public:
                DataType get_data_type() const{
                    return graph->props.max_int;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    op_logger(name)->error("Calling backward_diff unexpectedly.");
                    throw InternalGraphError(name, "Calling backward_diff unexpectedly.");
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return Node();
                }
            };

            class FloatOperator: public virtual AbstractOperator{
            public:
                DataType get_data_type() const{
                    return graph->props.max_float;
                }
            };

            class ElementwiseOperator: public virtual AbstractOperator{
            public:
                Shape get_shape() const {
                    return get_parents()[0]->shape;
                }
            };

            class UnaryOperator: public virtual AbstractOperator{
            protected:
                UnaryOperator(): parent(Node()) {};
            public:
                Node parent;
                UnaryOperator(Node parent) :
                        parent(parent) {};

                NodeVec get_parents() const {
                    return {parent};
                };
            };

            class BinaryOperator: public virtual AbstractOperator{
            protected:
                BinaryOperator(): parent1(Node()), parent2(Node()) {};
            public:
                Node parent1;
                Node parent2;
                BinaryOperator(Node parent1, Node parent2) :
                        parent1(parent1), parent2(parent2) {};

                NodeVec get_parents() const {
                    return {parent1, parent2};
                };
            };

            class AssociativeOperator : public virtual AbstractOperator {
            protected:
                AssociativeOperator() {};
            public:
                NodeVec parents;
                AssociativeOperator(NodeVec parents) :
                        parents(parents) {};

                NodeVec get_parents() const {
                    return parents;
                };

                DataType get_data_type() const {
                    return parents[0]->data_type;
                }
            };

            class MorphOperator: public virtual UnaryOperator {
            protected:
                MorphOperator(): UnaryOperator() {};
            public:
                DataType get_data_type() const{
                    return parent->data_type;
                }
            };

            class ReductionOperator : public virtual UnaryOperator {
            protected:
                ReductionOperator(): UnaryOperator() {};
            public:
                Axes axes;
                ReductionOperator(Axes axes) : axes(axes){};

                Shape get_shape() const {
                    Shape p_shape = parent->shape;
                    for (int i = 0; i < axes.size(); i++) {
                        p_shape[axes[i]] = 1;
                    }
                    return p_shape;
                }
            };

            class MonitorOperator: public virtual ElementwiseOperator, public virtual MorphOperator{
            public:
                Node monitored;
                std::string msg;
                MonitorOperator(Node monitored, std::string msg):
                        monitored(monitored), msg(msg) {};

                NodeVec get_arguments() const {
                    return NodeVec{monitored};
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return parent_derivatives[index];
                }
            };

            class UnaryElementwiseOperator: public virtual UnaryOperator,
                                            public virtual ElementwiseOperator {};

            class LogicalUnaryElementwise: public virtual LogicalOperator,
                                           public virtual UnaryElementwiseOperator {};


            class FloatUnaryElementwiseOperator: public virtual FloatOperator,
                                                 public virtual UnaryElementwiseOperator {

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return backward_diff_parent(parent_derivatives[index], index);
                }
            };

            class FloatUnaryOperator: public virtual UnaryOperator,
                                      public virtual FloatOperator {};

            class BinaryElementwiseOperator: public virtual BinaryOperator,
                                             public virtual ElementwiseOperator {};

            class LogicalBinaryElementwise: public virtual LogicalOperator,
                                            public virtual BinaryElementwiseOperator {};

            class BinaryIntegerElementwiseOperator: public virtual IntegerOperator,
                                                    public virtual BinaryElementwiseOperator{};

            class BinaryFloatElementwiseOperator: public virtual FloatOperator,
                                                  public virtual BinaryElementwiseOperator{
                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return backward_diff_parent(parent_derivatives[index], index);
                }
            };

            class AssociativeElementwiseOperator: public virtual AssociativeOperator,
                                                  public virtual ElementwiseOperator {};

            class MorphElementwiseOperator: public virtual MorphOperator,
                                            public virtual ElementwiseOperator {};

            class LogicalReductionOperator: public virtual LogicalOperator,
                                            public virtual ReductionOperator {};

            class MorphReductionOperator: public virtual MorphOperator,
                                          public virtual ReductionOperator {};


            class FloatReductionOperator: public virtual FloatOperator,
                                          public virtual ReductionOperator {};
        }
    }
}

#endif //METADIFF_GRAPH_IR_OP_ABSTRACT_H
