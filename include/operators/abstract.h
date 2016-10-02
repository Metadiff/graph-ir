//
// Created by alex on 04/05/16.
//

#ifndef METADIFF_ABSTRACT_H
#define METADIFF_ABSTRACT_H
namespace md {
    namespace core {
        namespace op {
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

                dataType get_dtype() const {
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
                        auto err = InvalidArguments(parents, name, "All NaryOperators require at least 2 parents");
                        logger()->error(err.msg);
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
                    auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                    logger()->error(err.msg);
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
                                       ImplicitBroadcast(NodeVec{parent1, parent2}, name));
                        this->parent1 = parent1.broadcast(shape);
                    }
                    if (parent2->shape != shape and not parent2.is_scalar()) {
                        operate_policy(graph->broadcast_err_policy, logger(),
                                       ImplicitBroadcast(NodeVec{parent1, parent2}, name));
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
                                           ImplicitBroadcast(parents, name));
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
                    auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                    logger()->error(err.msg);
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
                    auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                    logger()->error(err.msg);
                    throw err;
                }
            };
        }
    }
}

#endif //METADIFF_ABSTRACT_H
