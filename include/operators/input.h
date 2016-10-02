//
// Created by alex on 03/05/16.
//

#ifndef METADIFF_SPECIAL_H
#define METADIFF_SPECIAL_H

namespace metadiff {
    namespace op {
        using namespace core;
        using namespace exceptions;

        /** Operator for input variables */
        class Input : public Operator {
        public:
            dType dtype;

            Input(GraphInPtr graph, dType dtype) :
                    Operator("Input", graph),
                    dtype(dtype) { }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
                return std::make_shared<Input>(graph, dtype);
            }

            dType get_dtype() const {
                return dtype;
            }

            Shape get_shape() const {
                return Shape{SymInt::zero, SymInt::zero, SymInt::zero, SymInt::zero};
            }

            nodeType get_node_type() const {
                return INPUT;
            };

            unsigned short get_grad_level() const {
                return 0;
            }

            NodeVec get_parents() const {
                return NodeVec {};
            }

            NodeVec get_arguments() const {
                return NodeVec {};
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                logger()->error() << err.msg;
                throw err;
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                return false;
            }
        };

        /** Operator for shared input variables */
        class SharedInput : public Operator {
        public:
            SharedPtr var;

            SharedInput(GraphInPtr graph, SharedPtr var) :
                    Operator("Shared", graph),
                    var(var) { }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<SharedInput>(graph, var);
            }

            dType get_dtype() const {
                return var->get_dtype();
            }

            Shape get_shape() const {
                return Shape{var->shape[0], var->shape[1], var->shape[2], var->shape[3]};
            }

            nodeType get_node_type() const {
                return INPUT;
            };

            unsigned short get_grad_level() const {
                return 0;
            }

            NodeVec get_parents() const {
                return NodeVec {};
            }

            NodeVec get_arguments() const {
                return NodeVec {};
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                logger()->error() << err.msg;
                throw err;
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if(name == op->name){
                    auto cast_op = std::static_pointer_cast<const SharedInput>(op);
                    return var->id == cast_op->var->id;
                }
                return false;
            }
        };

        /** Operator for wrapping Symbolic Integers */
        class SymIntWrapper : public Operator {
        public:
            SymInt value;

            SymIntWrapper(GraphInPtr graph, SymInt value) :
                    Operator("SymInt", graph),
                    value(value) { }

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<SymIntWrapper>(graph, value);
            }

            dType get_dtype() const {
                return graph->max_int;
            }

            Shape get_shape() const {
                return scalar_shape;
            }

            nodeType get_node_type() const {
                return value.is_constant() ? CONSTANT : CONSTANT_DERIVED;
            };

            unsigned short get_grad_level() const {
                return 0;
            }

            NodeVec get_parents() const {
                return NodeVec {};
            }

            NodeVec get_arguments() const {
                return NodeVec {};
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                auto err = WrongGradient(NodeVec{owner, my_grad}, name);
                logger()->error() << err.msg;
                throw err;
            }

            bool equals(std::shared_ptr<const Operator> const op) const {
                if (this->name == op->name) {
                    auto cast_op = std::static_pointer_cast<const SymIntWrapper>(op);
                    return cast_op->value == value;
                }
                return false;
            }
        };
    }

    namespace core {
        Node GraphInternal::wrap(SymInt value) {
            std::shared_ptr<Operator> op = std::make_shared<op::SymIntWrapper>(this, value);
            return derived_node(op);
        }

        Node GraphInternal::tensor4(dType dtype,
                                    std::array<SymInt, 4> shape,
                                    std::string name) {
            auto result = std::make_shared<NodeInternal>(
                    shared_from_this().get(),
                    default_device,
                    nodes.size(),
                    name,
                    INPUT,
                    dtype,
                    shape,
                    std::make_shared<op::Input>(shared_from_this().get(), dtype),
                    0,
                    current_group
            );
            nodes.push_back(result);
            result->op->owner = result;
            return result;
        }

        Node GraphInternal::shared_variable(SharedPtr shared, std::string name) {
            std::shared_ptr<Operator> op = std::make_shared<op::SharedInput>(this, shared);
            Node node = derived_node(op);
            node->name = name;
            return node;
        }

#ifdef AFAPI
        Node GraphInternal::shared_variable(af::array value, std::string name) {
            SharedPtr shared = shared::make_shared(value, name);
            std::shared_ptr<Operator> op = std::make_shared<op::SharedInput>(this, shared);
            Node node = derived_node(op);
            node->name = name;
            return node;
        };
#endif
    }
}
#endif //METADIFF_SPECIAL_H
