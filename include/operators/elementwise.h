//
// Created by alex on 15/12/15.
//

#ifndef METADIFF_ELEMENTWISE_FUNC_H
#define METADIFF_ELEMENTWISE_FUNC_H
namespace metadiff {
    namespace op {
        using namespace core;
        using namespace exceptions;
        
        /** Explicit operator for square */
        class Square : public UnaryOperator {
        public:
            Square(GraphInPtr graph, Node parent) :
                    UnaryOperator("Square", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Square>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node two = graph->constant_value(2.0);
                two->grad_level = my_grad->grad_level;
                return Node::mul({my_grad, two, parent});
            }
        };

        /** Exponential */
        class Exp : public UnaryOperator {
        public:
            Exp(GraphInPtr graph, Node parent) :
                    UnaryOperator("Exp", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Exp>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, owner});
            }
        };

        /** Logarithm */
        class Log : public UnaryOperator {
        public:
            Log(GraphInPtr graph, Node parent) :
                    UnaryOperator("Log", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Log>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.div()});
            }
        };

        /** Logarithm in base 10 */
        class Log10 : public UnaryOperator {
        public:
            Log10(GraphInPtr graph, Node parent) :
                    UnaryOperator("Log10", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Log>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.div(), graph->LN_10().div()});
            }
        };

        /** Absolute value */
        class Abs : public UnaryOperator {
        public:
            Abs(GraphInPtr graph, Node parent) :
                    UnaryOperator("Abs", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Abs>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node zero = graph->constant_value(0.0);
                zero->grad_level = my_grad->grad_level;
                return Node::mul(NodeVec{my_grad, parent.ge(zero)});
            }
        };

        /** Logarithm of x + 1 */
        class Log1p : public UnaryOperator {
        public:
            Log1p(GraphInPtr graph,
                  Node parent) :
                    UnaryOperator("Log1p", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Log1p>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.sigmoid()});
            }
        };

        /** Trigonometric sine function */
        class Sin : public UnaryOperator {
        public:
            Sin(GraphInPtr graph, Node parent) :
                    UnaryOperator("Sin", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Sin>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.cos()});
            }
        };

        /** Trigonometric cosine function */
        class Cos : public UnaryOperator {
        public:
            Cos(GraphInPtr graph, Node parent) :
                    UnaryOperator("Cos", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Cos>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.sin().neg()});
            }
        };

        /** Trigonometric tangent function */
        class Tan : public UnaryOperator {
        public:
            Tan(GraphInPtr graph, Node parent) :
                    UnaryOperator("Tan", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Tan>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.cos().square().div()});
            }
        };

        /** Trigonometric cotangent function */
        class Cot : public UnaryOperator {
        public:
            Cot(GraphInPtr graph, Node parent) :
                    UnaryOperator("Cot", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Cot>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.sin().square().div()}).neg();
            }
        };

        /** Hyperbolic sine function */
        class Sinh : public UnaryOperator {
        public:
            Sinh(GraphInPtr graph, Node parent) :
                    UnaryOperator("Sinh", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Sinh>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.cosh()});
            }
        };

        /** Hyperbolic cosine function */
        class Cosh : public UnaryOperator {
        public:
            Cosh(GraphInPtr graph, Node parent) :
                    UnaryOperator("Cosh", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Cosh>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                return Node::mul(NodeVec{my_grad, parent.sinh()});
            }
        };

        /** Hyperbolic tangent function */
        class Tanh : public UnaryOperator {
        public:
            Tanh(GraphInPtr graph, Node parent) :
                    UnaryOperator("Tanh", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Tanh>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node derivative = Node::add(NodeVec{graph->constant_value(1.0), owner.square().neg()});
                return Node::mul(NodeVec{my_grad, derivative});
            }
        };

        /** Hyperbolic cotangent function */
        class Coth : public UnaryOperator {
        public:
            Coth(GraphInPtr graph, Node parent) :
                    UnaryOperator("Coth", graph, parent) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Coth>(graph, ancestors[0]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node derivative = graph->constant_value(1.0).add(NodeVec{owner.square().neg()});
                return Node::mul(NodeVec{my_grad, derivative});
            }
        };

        /** Takes the first input to the power of the second elementwise */
        class Pow : public ElementwiseBinary {
        public:
            Pow(GraphInPtr graph, Node parent1, Node parent2) :
                    ElementwiseBinary("Pow", graph, parent1, parent2) { };

            std::shared_ptr<Operator> copy_to(GraphInPtr graph, NodeVec ancestors) const {
                return std::make_shared<Pow>(graph, ancestors[0], ancestors[1]);
            }

            Node get_parent_grad(Node my_grad, unsigned short index) {
                Node product = Node::mul(NodeVec{my_grad, owner});
                if (index == 0) {
                    Node factor = Node::mul(NodeVec{parent2, parent1.div()});
                    return Node::mul(NodeVec{product, factor});
                } else {
                    return Node::mul(NodeVec{product, parent1.log()});
                }
            }
        };
    }

    namespace core {
        Node Node::square() {
            return apply<op::Square>(this);
        }

        Node Node::exp() {
            return apply<op::Exp>(this);
        }

        Node Node::sigmoid() {
            GraphInPtr graph = unwrap()->graph;
            return graph->constant_value(1.0) / (graph->constant_value(1.0) + this->neg().exp());
        }

        Node Node::log() {
            return apply<op::Log>(this);
        }

        Node Node::log10() {
            return apply<op::Log10>(this);
        }

        Node Node::abs() {
            return apply<op::Abs>(this);
        }

        Node Node::log1p() {
            return apply<op::Log1p>(this);
        }

        Node Node::softplus(int threshold) {
            if(threshold <= 0){
                return exp().log1p();
            } else {
                Node condition = this->ge(unwrap()->graph->constant_value(threshold));
                return condition.select(this, this->exp().log1p());
            }
        }

        Node Node::sin() {
            return apply<op::Sin>(this);
        }

        Node Node::cos() {
            return apply<op::Cos>(this);
        }

        Node Node::tan() {
            return apply<op::Tan>(this);
        }

        Node Node::cot() {
            return apply<op::Cot>(this);
        }

        Node Node::sinh() {
            return apply<op::Sinh>(this);
        }

        Node Node::cosh() {
            return apply<op::Cosh>(this);
        }

        Node Node::tanh() {
            return apply<op::Tanh>(this);
        }

        Node Node::coth() {
            return apply<op::Coth>(this);
        }

        Node Node::pow(Node power) {
            std::shared_ptr<NodeInternal> ptr = unwrap();
            return ptr->graph->derived_node(std::make_shared<op::Pow>(ptr->graph, this, power));
        }
    }
}

#endif //METADIFF_ELEMENTWISE_FUNC_H
