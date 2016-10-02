//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_CORE_H
#define METADIFF_CORE_CORE_H
namespace md{
    namespace core{
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
    }
}
#endif //METADIFF_CORE_CORE_H
