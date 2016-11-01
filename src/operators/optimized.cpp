//
// Created by alex on 21/10/16.
//

#include "metadiff.h"

// TODO implement checks for all operators here

namespace md{
    namespace core{
        Node GraphInternal::softplus(Node node, double threshold){
            Operator op = std::make_shared<op::Softplus>(this, node, threshold);
            return derived_node(op);
        }

        Node GraphInternal::log_sum_exp(Node node, Axes axes, double threshold){
            Operator op = std::make_shared<op::LogSumExp>(this, node, axes, threshold);
            return derived_node(op);
        }

        Node GraphInternal::log_sum_exp(Node node, int axis, double threshold){
            if(axis == 100){
                return log_sum_exp(node, auto_infer_axes(node), threshold);
            } else {
                return log_sum_exp(node, {axis}, threshold);
            }
        }

        Node GraphInternal::sigmoid(Node node){
            return apply<op::Sigmoid>(node);
        }

        Node GraphInternal::softmax(Node node){
            Operator op = std::make_shared<op::Softmax>(this, node);
            return derived_node(op);
        }

        Node GraphInternal::binary_cross_entropy_logits(Node p, Node q_logits){
            Operator op = std::make_shared<op::BinaryCrossEntropyLogits>(this, p, q_logits);
            return derived_node(op);
        }

        Node GraphInternal::categorical_cross_entropy_logits(Node p, Node q_logits){
            Operator op = std::make_shared<op::CategoricalCrossEntropyLogits>(this, p, q_logits);
            return derived_node(op);
        }
    }
}