//
// Created by alex on 04/05/16.
//

#ifndef METADIFF_ABSTRACT_H
#define METADIFF_ABSTRACT_H
namespace md {
    namespace core {
        namespace op {
            /** Abstract class for operators */
            class AbstractOperator {
            public:
                std::shared_ptr<spdlog::logger> logger() const {
                    return md::utils::logger("operator::" + this->name);
                }
            public:
                /** Unique name of the concrete Operator class */
                std::string const name;
                /** Pointer to the owning GraphInternal */
                GraphInPtr graph;
                /** Pointer to the owning Node */
                Node owner;

                AbstractOperator(std::string name,
                                 GraphInPtr graph) :
                        name(name),
                        graph(graph) { };

                /** Copies the operator to a new graph, by using the ancestors
                 * provided from the new graph. See Node::copy_to(GraphInPtr graph, NodeVec ancestors)
                 * */
                virtual Operator copy_to(GraphInPtr graph, NodeVec ancestors) const = 0;

                /** Calculates what should be the resulting NodeData#dtype */
                virtual dataType get_data_type() const = 0;

                /** Calculates what should be the resulting NodeData#shape */
                virtual Shape get_shape() const = 0;

                /** Calculates what should be the resulting NodeData#node_type */
                virtual nodeType get_node_type() const = 0;

                /** Calculates what should be the resulting NodeData#grad_level */
                virtual unsigned short get_grad_level() const = 0;

                /** Returns the parents NodeVec of this operator */
                virtual NodeVec get_parents() const = 0;

                /** Returns the arguments NodeVec of this operator */
                virtual NodeVec get_arguments() const = 0;

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
                virtual void generate_gradients(NodeVec &messages);

                /**
                 * TODO this and the symbolic_equals are things which aren't yet well done
                 * Compares only if this operator is equal to the other, not the other way around.
                 * Note that although equality is symmetric, because of mathematical idenitities
                 * and the fact that the code is with each operator separately the true equality
                 * operator is `op1.equals(op2) or op2.equals(op1)`
                 */
                virtual bool equals(Operator const op) const = 0;

                /**
                 * Returns the union of the parents and arguments of this Operator
                 * See: get_parents(), get_ancestors()
                 */
                NodeVec get_ancestors() const;
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

                NodeVec get_arguments() const {
                    return NodeVec {};
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const UnaryOperator>(op);
                        return symbolic_equals(parent, cast_op->parent);
                    }
                    return false;
                }
            };

            /** Abstract class for binary operators. */
            class BinaryOperator : public AbstractOperator {
            public:
                Node parent1;
                Node parent2;
                Shape shape;

                BinaryOperator(std::string const name,
                               GraphInPtr graph,
                               Node parent1,
                               Node parent2) :
                        AbstractOperator(name, graph),
                        parent1(parent1),
                        parent2(parent2) {}

                NodeVec get_parents() const {
                    return {parent1, parent2};
                };

                dataType get_data_type() const {
                    return graph->promotion_table[parent1->data_type][parent2->data_type];
                };

                nodeType get_node_type() const {
                    if (parent1->node_type == INPUT
                        or parent1->node_type == INPUT_DERIVED
                        or parent2->node_type == INPUT
                        or parent2->node_type == INPUT_DERIVED) {
                        return INPUT_DERIVED;
                    } else {
                        return CONSTANT_DERIVED;
                    }
                };

                Shape get_shape() const {
                    return shape;
                }

                unsigned short get_grad_level() const {
                    return parent1->grad_level > parent2->grad_level ?
                           parent1->grad_level :
                           parent2->grad_level;
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

            /** Abstract class for any operators that take 2 or more arguments */
            class NaryOperator : public AbstractOperator {
            public:
                NodeVec parents;
                Shape shape;

                NaryOperator(std::string const name,
                             GraphInPtr graph,
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

                NodeVec get_parents() const {
                    return parents;
                };

                dataType get_data_type() const {
                    dataType data_type = b8;
                    for (size_t i = 0; i < parents.size(); i++) {
                        data_type = graph->promotion_table[data_type][parents[i]->data_type];
                    }
                    return data_type;
                };

                nodeType get_node_type() const {
                    for (int i = 0; i < parents.size(); i++) {
                        if (parents[i]->node_type == INPUT
                            or parents[i]->node_type == INPUT_DERIVED) {
                            return INPUT_DERIVED;
                        }
                    }
                    return CONSTANT_DERIVED;
                };

                Shape get_shape() const {
                    return shape;
                }

                unsigned short get_grad_level() const {
                    size_t max_grad_level = 0;
                    for (int i = 0; i < parents.size(); i++) {
                        if (parents[i]->grad_level > max_grad_level) {
                            max_grad_level = parents[i]->grad_level;
                        }
                    }
                    return max_grad_level;
                };

                NodeVec get_arguments() const {
                    return NodeVec {};
                }
            };

            /** Abstract class for operators which are constant expressions */
            class ConstantOperator : public AbstractOperator {
            public:
                Shape shape;
                dataType data_type;

                ConstantOperator(std::string const name,
                                 GraphInPtr graph,
                                 dataType data_type) :
                        AbstractOperator(name, graph),
                        data_type(data_type) {};

                ConstantOperator(std::string const name,
                                 GraphInPtr graph,
                                 Shape shape,
                                 dataType data_type) :
                        AbstractOperator(name, graph),
                        shape(shape),
                        data_type(data_type) {};

                NodeVec get_parents() const {
                    return {};
                };

                dataType get_data_type() const {
                    return data_type;
                };

                nodeType get_node_type() const {
                    for (int i = 0; i < 4; i++) {
                        if (not shape[i].is_constant()) {
                            return CONSTANT_DERIVED;
                        }
                    }
                    return CONSTANT;
                };

                Shape get_shape() const {
                    return shape;
                }

                unsigned short get_grad_level() const {
                    return 0;
                };

                NodeVec get_arguments() const {
                    return NodeVec {};
                }

                Node get_parent_grad(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }

                bool equals(Operator const op) const {
                    if (name == op->name) {
                        auto cast_op = std::static_pointer_cast<const ConstantOperator>(op);
                        return shape == cast_op->shape and data_type == cast_op->data_type;
                    }
                    return false;
                }
            };

            /** Abstract class for binary operators which are applied elementwise */
            class ElementwiseBinary : public BinaryOperator {
            public:
                ElementwiseBinary(std::string const name,
                                  GraphInPtr graph,
                                  Node parent1,
                                  Node parent2) :
                        BinaryOperator(name, graph, parent1, parent2) {
                    NodeVec parents = get_parents();
                    shape = verify_elementwise_shapes(NodeVec{parents});
                    if (parent1->shape != shape and not parent1.is_scalar()) {
                        operate_policy(graph->broadcast_err_policy, logger(),
                                       std::make_shared<ImplicitBroadcast>(NodeVec{parent1, parent2}, name));
                        this->parent1 = parent1.broadcast(shape);
                    }
                    if (parent2->shape != shape and not parent2.is_scalar()) {
                        operate_policy(graph->broadcast_err_policy, logger(),
                                       std::make_shared<ImplicitBroadcast>(NodeVec{parent1, parent2}, name));
                        this->parent2 = parent2.broadcast(shape);
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
                    shape = verify_elementwise_shapes(parents);
                    for (int i = 0; i < parents.size(); i++) {
                        if (parents[i]->shape != shape and not parents[i].is_scalar()) {
                            operate_policy(graph->broadcast_err_policy, logger(),
                                           std::make_shared<ImplicitBroadcast>(NodeVec{parents}, name));
                            this->parents.push_back(parents[i].broadcast(shape));
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
        }
    }
}

#endif //METADIFF_ABSTRACT_H
