//
// Created by alex on 04/05/16.
//

#ifndef METADIFF_CORE_OPERATORS_ABSTRACT_H
#define METADIFF_CORE_OPERATORS_ABSTRACT_H
namespace md {
    namespace core {
        namespace op {
            /** Abstract class for operators */
            class AbstractOperator {
            protected:
                AbstractOperator(): name("."), graph(nullptr) {};
            public:
                std::shared_ptr<spdlog::logger> logger() const {
                    return md::utils::logger("operator::" + name);
                }
                /** Unique name of the concrete Operator class */
                std::string const name;
                /** Pointer to the owning GraphInternal */
                GraphInPtr const graph;
                /** Pointer to the owning Node */
                Node owner;

                AbstractOperator(std::string const name,
                                 GraphInPtr const graph) :
                        name(name),  graph(graph) {};

                /** Copies the operator to a new graph, by using the ancestors
                 * provided from the new graph. See Node::copy_to(GraphInPtr graph, NodeVec ancestors)
                 * */
                virtual Operator copy_to(GraphInPtr graph, NodeVec ancestors) const = 0;

                /** Calculates what should be the resulting NodeData#shape */
                virtual Shape get_shape() const = 0;

                /** Returns the parents NodeVec of this operator */
                virtual NodeVec get_parents() const  = 0;

                virtual bool is_input_dependent() const;

                virtual bool is_differentiable() const;

                /** Calculates what should be the resulting NodeData#dtype */
                virtual dataType get_data_type() const = 0;

                /** Returns the arguments NodeVec of this operator */
                virtual NodeVec get_arguments() const;

                /** Returns the union of the parents and arguments */
                NodeVec get_ancestors() const;

                /** Calculates what should be the resulting NodeData#grad_level */
                unsigned short get_grad_level() const;

                /**
                 * A function which should compute and return the gradient with respect
                 * to the parent and the specified index, given the gradient of the owner node
                 */
                virtual Node get_parent_grad(Node my_grad, short index) = 0;

                /**
                 * Sends a gradient message from this Operator to the parent with id target.
                 * If the target has no gradient messages, then just inserts the new message,
                 * otherwise it adds it to the already existing message
                 * (e.g. accumulates the gradients)
                 *
                 * See: generate_gradients(NodeVec &messages)
                 */
                void send_grad_message(size_t target,
                                       Node msg,
                                       NodeVec &messages) const;

                /** Generates gradient messages for all parents of this Operator.
                 * See: send_grad_message(size_t target, Node msg, NodeVec &messages)
                 */
                void generate_gradients(NodeVec &messages);

                /**
                 * TODO this and the symbolic_equals are things which aren't yet well done
                 * Compares only if this operator is equal to the other, not the other way around.
                 * Note that although equality is symmetric, because of mathematical idenitities
                 * and the fact that the code is with each operator separately the true equality
                 * operator is `op1.equals(op2) or op2.equals(op1)`
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

            /** Operators which do not have any parents */
            class OrphanOperator: public virtual AbstractOperator {
            public:

                NodeVec get_parents() const {
                    return NodeVec {};
                }

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Operators which are inputs */
            class InputOperator: public virtual OrphanOperator {
            public:
                bool is_input_dependent() const{
                    return true;
                }

                bool is_differentiable() const{
                    return true;
                }
            };

            /** Operators which represent a constant expression */
            class ConstantOperator : public virtual OrphanOperator, public virtual NonDifferentiableOperator {
            public:
                dataType data_type;

                ConstantOperator(dataType data_type):
                        data_type(data_type) {};

                dataType get_data_type() const {
                    return data_type;
                }
            };

            class LogicalOperator: public virtual NonDifferentiableOperator{

                dataType get_data_type() const {
                    return b8;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Operators which always return integer values */
            class IntegerOperator: public virtual AbstractOperator{
            public:
                dataType get_data_type() const{
                    return graph->max_int;
                }
            };

            /** Operators which always return float values */
            class FloatOperator: public virtual AbstractOperator{
            public:
                dataType get_data_type() const{
                    return graph->max_float;
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

            /** Abstract class for any operators that can be applied to more than two nodes */
            class AssociativeOperator : public virtual AbstractOperator {
            protected:
                AssociativeOperator() {};
            public:
                NodeVec parents;
                AssociativeOperator(NodeVec parents) :
                        parents(parents) {
                    if (parents.size() < 2) {
                        auto err = std::make_shared<InvalidArguments>
                                (parents, name, "All NaryOperators require at least 2 parents");
                        err->log(logger());
                        throw err;
                    }
                };

                NodeVec get_parents() const {
                    return parents;
                };

                dataType get_data_type() const {
                    return parents[0]->data_type;
                }
            };

            /** Operators which preserve the data_type of their parent */
            class MorphOperator: public virtual UnaryOperator {
            protected:
                MorphOperator(): UnaryOperator() {};
            public:
                dataType get_data_type() const{
                    return parent->data_type;
                }
            };

            /** Abstract class for binary logical operators */
            class ReductionOperator : public virtual UnaryOperator {
            protected:
                ReductionOperator(): UnaryOperator() {};
            public:
                Axes axes;
                ReductionOperator(Axes axes) : axes(axes){
                    if(not validate_axes(axes)){
                        std::string axes_str;
                        for (auto i = 0; i < axes.size(); ++i) {
                            axes_str += std::to_string(axes[i]);
                            if (i < axes.size() - 1) {
                                axes_str += ", ";
                            }
                        }
                        if (axes.size() == 0) {
                            axes_str = "NULL";
                        }
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent}, name, "Invalid axes: " + axes_str);
                        err->log(logger());
                        throw err;
                    }
                    // TODO should we forbid summing over axes where shape[i] = 1?
                    for(auto i=0; i<axes.size(); ++i){
                        if(parent->shape[axes[i]] == 1){
                            axes.erase(std::remove(axes.begin(), axes.end(), i), axes.end());
                        }
                    }
                    this->axes = axes;
                };

                Shape get_shape() const {
                    Shape p_shape = parent->shape;
                    for (int i = 0; i < axes.size(); i++) {
                        p_shape[axes[i]] = 1;
                    }
                    return p_shape;
                }
            };


            class UnaryElementwiseOperator: public virtual UnaryOperator,
                                            public virtual ElementwiseOperator {};

            class LogicalUnaryElementwise: public virtual LogicalOperator,
                                           public virtual UnaryElementwiseOperator {};


            class FloatUnaryElementwiseOperator: public virtual FloatOperator,
                                                 public virtual UnaryElementwiseOperator {};

            class FloatUnaryOperator: public virtual UnaryOperator,
                                      public virtual FloatOperator {};

            class BinaryElementwiseOperator: public virtual BinaryOperator,
                                             public virtual ElementwiseOperator {};

            class LogicalBinaryElementwise: public virtual LogicalOperator,
                                            public virtual BinaryElementwiseOperator {};

            class BinaryIntegerElementwiseOperator: public virtual IntegerOperator,
                                                 public virtual BinaryElementwiseOperator{};

            class BinaryFloatElementwiseOperator: public virtual FloatOperator,
                                               public virtual BinaryElementwiseOperator{};

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

#endif //METADIFF_CORE_OP_ABSTRACT_H
