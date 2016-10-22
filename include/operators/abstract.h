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
            public:
                std::shared_ptr<spdlog::logger> logger() const {
                    return md::utils::logger("operator::" + name);
                }
            public:
                /** Unique name of the concrete Operator class */
                std::string const name;
                /** Pointer to the owning GraphInternal */
                GraphInPtr const graph;
                /** Pointer to the owning Node */
                Node owner;

                AbstractOperator(std::string const name,
                                 GraphInPtr const graph) :
                        name(name),
                        graph(graph) { };

                /** Copies the operator to a new graph, by using the ancestors
                 * provided from the new graph. See Node::copy_to(GraphInPtr graph, NodeVec ancestors)
                 * */
                virtual Operator copy_to(GraphInPtr graph, NodeVec ancestors) const = 0;

                /** Calculates what should be the resulting NodeData#node_type */
                virtual nodeType get_node_type() const = 0;

                /** Calculates what should be the resulting NodeData#dtype */
                virtual dataType get_data_type() const = 0;

                /** Calculates what should be the resulting NodeData#shape */
                virtual Shape get_shape() const = 0;

                /** Returns the parents NodeVec of this operator */
                virtual NodeVec get_parents() const  = 0;

                /** Returns the arguments NodeVec of this operator */
                virtual NodeVec get_arguments() const  = 0;

                /**
                 * Returns the union of the parents and arguments of this Operator
                 * See: get_parents(), get_ancestors()
                 */
                NodeVec get_ancestors() const{
                    auto ancestors = get_parents();
                    auto arguments = get_arguments();
                    std::copy (ancestors.begin(), ancestors.end(), std::back_inserter(arguments));
                    return ancestors;
                }

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
                virtual bool equals(Operator const op) const = 0;
            };

            /** Abstract Operator with no parents */
            class OrphanOperator: public AbstractOperator {
            public:
                dataType data_type;
                OrphanOperator(std::string const name,
                               GraphInPtr const graph,
                               dataType const data_type):
                        AbstractOperator(name, graph),
                        data_type(data_type) {};

                dataType get_data_type() const {
                    return data_type;
                }

                NodeVec get_parents() const {
                    return NodeVec {};
                }

                NodeVec get_arguments() const {
                    return NodeVec {};
                }

                unsigned short get_grad_level() const {
                    return 0;
                }

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }

                bool equals(Operator const op) const {
                    return false;
                }
            };

            /** Abstract Opeartor for Inputs */
            class InputOperator: public OrphanOperator {
            public:

                InputOperator(std::string const name,
                              GraphInPtr const graph,
                              dataType const data_type):
                        OrphanOperator(name, graph, data_type) {};

                nodeType get_node_type() const {
                    return INPUT;
                };
            };

            /** Abstract class for operators which are constant expressions */
            class ConstantOperator : public OrphanOperator {
            public:
                ConstantOperator(std::string const name,
                                 GraphInPtr const graph,
                                 dataType const data_type) :
                        OrphanOperator(name, graph, data_type){};

                nodeType get_node_type() const {
                    Shape shape = get_shape();
                    for (int i = 0; i < 4; i++) {
                        if (not shape[i].is_constant()) {
                            return INPUT_DERIVED_NON_DIFF;
                        }
                    }
                    return CONSTANT;
                };
            };

            /** Abstract class for any operators that can be applied to more than two nodes */
            class NaryOperator : public AbstractOperator {
            public:
                NodeVec parents;
                Shape shape;

                NaryOperator(std::string const name,
                             GraphInPtr const graph,
                             NodeVec parents) :
                        AbstractOperator(name, graph),
                        parents(parents) {
                    if (parents.size() < 2) {
                        auto err = std::make_shared<InvalidArguments>
                                (parents, name, "All NaryOperators require at least 2 parents");
                        err->log(logger());
                        throw err;
                    }
                };

                nodeType get_node_type() const {
                    bool input_derived = false;
                    bool input_non_diff = false;
                    for (int i = 0; i < parents.size(); i++) {
                        if (parents[i]->node_type == INPUT
                            or parents[i]->node_type == INPUT_DERIVED) {
                            input_derived = true;
                        }
                        if(parents[i]->node_type == INPUT_DERIVED_NON_DIFF){
                            input_non_diff = true;
                        }
                    }
                    return input_derived ? INPUT_DERIVED : (input_non_diff ? INPUT_DERIVED_NON_DIFF: CONSTANT_DERIVED);
                };

                dataType get_data_type() const {
                    dataType max = b8;
                    for (size_t i = 0; i < parents.size(); i++) {
                        if(parents[i]->data_type > max){
                            max = parents[i]->data_type;
                        }
                    }
                    if(max < i8){
                        return static_cast<dataType>(1 + (graph->max_int - 1) % 4);
                    } else if(max < f8){
                        return graph->max_int;
                    } else {
                        return graph->max_float;
                    }
                };

                Shape get_shape() const {
                    return shape;
                }

                NodeVec get_parents() const {
                    return parents;
                };

                NodeVec get_arguments() const {
                    return NodeVec {};
                }
            };

            /** Abstract class for binary operators. */
            class BinaryOperator : public AbstractOperator {
            public:
                Node parent1;
                Node parent2;
                Shape shape;

                BinaryOperator(std::string const name,
                               GraphInPtr const graph,
                               Node parent1,
                               Node parent2) :
                        AbstractOperator(name, graph),
                        parent1(parent1),
                        parent2(parent2) {};

                nodeType get_node_type() const {
                    if (parent1->node_type == INPUT
                        or parent1->node_type == INPUT_DERIVED
                        or parent2->node_type == INPUT
                        or parent2->node_type == INPUT_DERIVED) {
                        return INPUT_DERIVED;
                    } else if(parent1->node_type == INPUT_DERIVED_NON_DIFF
                              or parent2->node_type == INPUT_DERIVED_NON_DIFF){
                        return INPUT_DERIVED_NON_DIFF;
                    } else {
                        return CONSTANT_DERIVED;
                    }
                };

                Shape get_shape() const {
                    return shape;
                }

                NodeVec get_parents() const {
                    return {parent1, parent2};
                };

                NodeVec get_arguments() const {
                    return NodeVec {};
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const BinaryOperator>(op);
                        return symbolic_equals(parent1, cast_op->parent1) and
                               symbolic_equals(parent2, cast_op->parent2);
                    }
                    return false;
                }

            };

            /** Abstract class for unary operators */
            class UnaryOperator : public AbstractOperator {
            public:
                Node parent;

                UnaryOperator(std::string const name,
                              GraphInPtr graph,
                              Node parent) :
                        AbstractOperator(name, graph),
                        parent(parent) {};

                NodeVec get_parents() const {
                    return {parent};
                };

                NodeVec get_arguments() const {
                    return NodeVec {};
                }

                dataType get_data_type() const {
                    return parent->data_type;
                };

                nodeType get_node_type() const {
                    if (parent->node_type == INPUT
                        or parent->node_type == INPUT_DERIVED) {
                        return INPUT_DERIVED;
                    } else {
                        return CONSTANT_DERIVED;
                    }
                };

                Shape get_shape() const {
                    return parent->shape;
                }

                unsigned short get_grad_level() const {
                    return parent->grad_level;
                };

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const UnaryOperator>(op);
                        return symbolic_equals(parent, cast_op->parent);
                    }
                    return false;
                }
            };

            class FloatUnaryOperator: public UnaryOperator{
            public:
                FloatUnaryOperator(std::string const name,
                                   GraphInPtr graph,
                                   Node parent) :
                        UnaryOperator(name, graph, parent) {}

                dataType get_data_type() const {
                    return graph->max_float;
                };

            };

            /** Abstract class for binary operators which are applied elementwise */
            class ElementwiseBinary : public BinaryOperator {
            public:
                ElementwiseBinary(std::string const name,
                                  GraphInPtr graph,
                                  Node parent1,
                                  Node parent2) :
                        BinaryOperator(name, graph, parent1, parent2) {
                    dataType promoted = graph->promotion_table[parent1->data_type][parent2->data_type];
                    dataType limited = graph->limit_type(promoted);
                    if(limited < promoted) {
                        auto err = std::make_shared<TypePromotion>(NodeVec{parent1, parent2},
                                                                   name, promoted, limited);
                        operate_policy(graph->promotion_err_policy, logger(), err);
                    }

                    NodeVec parents = get_parents();
                    shape = verify_elementwise_shapes(NodeVec{parents}, logger());
                    if (parent1->shape != shape and parent1.dims() > 0) {
                        operate_policy(graph->broadcast_err_policy, logger(),
                                       std::make_shared<ImplicitBroadcast>(NodeVec{parent1, parent2}, name));
                        this->parent1 = graph->broadcast(parent1, shape);
                    }
                    if (parent2->shape != shape and parent2.dims() > 0) {
                        operate_policy(graph->broadcast_err_policy, logger(),
                                       std::make_shared<ImplicitBroadcast>(NodeVec{parent1, parent2}, name));
                        this->parent2 = graph->broadcast(parent2, shape);
                    }
                }
            };

            /** Abstract class for nary operators which are applied elementwise */
            class ElementwiseNary : public NaryOperator {
            public:
                ElementwiseNary(std::string const name,
                                GraphInPtr graph,
                                NodeVec parents) :
                        NaryOperator(name, graph, parents) {
                    this->parents.clear();
                    shape = verify_elementwise_shapes(parents, logger());
                    for (int i = 0; i < parents.size(); i++) {
                        if (parents[i]->shape != shape and parents[i].dims() > 0) {
                            operate_policy(graph->broadcast_err_policy, logger(),
                                           std::make_shared<ImplicitBroadcast>(NodeVec{parents}, name));
                            this->parents.push_back(graph->broadcast(parents[i], shape));
                        } else {
                            this->parents.push_back(parents[i]);
                        }
                    }
                };
            };

            /** Abstract class for unary logical operators */
            class LogicalUnary : public UnaryOperator {
            public:
                LogicalUnary(std::string const name,
                             GraphInPtr graph,
                             Node parent) :
                        UnaryOperator(name, graph, parent) {};

                dataType get_data_type() const {
                    return b8;
                };

                nodeType get_node_type() const {
                    return CONSTANT_DERIVED;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Abstract class for binary logical operators */
            class LogicalBinary : public ElementwiseBinary {
            public:
                LogicalBinary(std::string const name,
                              GraphInPtr graph,
                              Node parent1,
                              Node parent2) :
                        ElementwiseBinary(name, graph, parent1, parent2) {};

                dataType get_data_type() const {
                    return b8;
                };

                nodeType get_node_type() const {
                    return CONSTANT_DERIVED;
                };

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            /** Abstract class for binary logical operators */
            class Reduction : public UnaryOperator {
            public:
                Axes axes;
                Reduction(std::string const name,
                          GraphInPtr graph,
                          Node parent,
                          Axes axes) :
                        UnaryOperator(name, graph, parent) {
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

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const Reduction>(op);
                        bool axes_true = axes.size() == cast_op->axes.size();
                        for(auto i=0; i<axes.size(); ++i){
                            axes_true = axes_true and axes[i] == cast_op->axes[i];
                        }
                        if(axes_true){
                            return symbolic_equals(parent, cast_op->parent);
                        }
                    }
                    return false;
                }
            };
        }
    }
}

#endif //METADIFF_CORE_OP_ABSTRACT_H
