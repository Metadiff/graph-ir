//
// Created by alex on 19/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{

        Node matrix_mul(NodeVec nodes, std::vector<bool> transpositions){
            // Check there is at least one node
            if(nodes.size() == 0){
                op_logger("MatrixMul")->error("Zero nodes provided.");
                throw InvalidOperatorArgument(nodes, "MatrixMul",
                                              "Zero nodes provided.");
            } else if(nodes.size() == 1){
                // TODO
                op_logger("MatrixMul")->error("One nodes provided.");
                throw InvalidOperatorArgument(nodes, "MatrixMul",
                                              "One nodes provided.");
            }
            Graph g = nodes[0].g();
            // Check transpositions are correct size
            if(transpositions.size() == 0){
                // Assume default so fill with false
                for(auto i=0;i<nodes.size(); ++i){
                    transpositions.push_back(false);
                }
            } else if(nodes.size() != transpositions.size()){
                op_logger("MatrixMul")->error("Different size of nodes and transpositions.");
                throw InvalidOperatorArgument(nodes, "MatrixMul",
                                              "Different size of nodes and transpositions.");
            }
            if(nodes[0].order() > 2){
                op_logger("MatrixMul")->error("Node 0 is not a matrix.");
                throw InvalidOperatorArgument(nodes, "MatrixMul",
                                              "Node 0 is not a matrix.");
            }
            SymInt size = nodes[0]->shape[!transpositions[0]];
            // Check all nodes are from the same graph and their shapes are correct
            for(auto i=1; i < nodes.size(); ++i){
                if(g != nodes[i].g()){
                    op_logger("MatrixMul")->error("The input variables are not from the same graph.");
                    throw InvalidOperatorArgument(nodes, "MatrixMul",
                                                  "The input variables are not from the same graph.");
                } else if(nodes[i].order() > 2){
                    op_logger("MatrixMul")->error("Node {} is not a matrix.", i);
                    throw InvalidOperatorArgument(nodes, "MatrixMul",
                                                  "Node " + std::to_string(i) + " is not a matrix.");
                } else if(nodes[i]->shape[transpositions[i]] != size){
                    op_logger("MatrixMul")->error("Incorrect shapes of node {} with shape {}. Expected size {}.",
                                                  i, to_string(nodes[i]->shape), sym::to_string(size));
                    throw InvalidOperatorArgument(nodes, "MatrixMul",
                                                  "Incorrect shapes of node "  + std::to_string(i)
                                                  + " with shape " + to_string(nodes[i]->shape)
                                                  + ". Expected size " + sym::to_string(size) + ".");
                }
                size = nodes[i]->shape[!transpositions[i]];
            }
            // TODO check any two consecutive are inverse of each other
            // Standard
            auto op = std::make_shared<op::MatrixMul>(g.get(), nodes, transpositions);
            return g->derived_node(op);
        }

        Node dot(Node left, Node right, bool transpose_left, bool transpose_right){
            return matrix_mul(NodeVec{left, right}, std::vector<bool>{transpose_left, transpose_right});
        }

        Node matrix_inverse(Node node, bool transpose){
            Graph g = node.g();
            if(node.order() != 2){
                op_logger("MatrixInverse")->error("Node is not a matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "MatrixInverse",
                                              "Node is not a matrix.");
            } else if(node->shape[0] != node->shape[1]){
                op_logger("MatrixInverse")->error("Node is not a square matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "MatrixInverse",
                                              "Node is not a square matrix.");
            }
            // matrix_inv(matrix_inv(x)) = x
            auto base = get_base_node(node);
            if(base->op->name == "MatrixInv"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::MatrixInverse>(g.get(), node, transpose);
            return g->derived_node(op);
        }

        Node matrix_inverse_mul(Node node1, Node node2, bool transpose_inv, bool transpose_mul){
            Graph g = node1.g();
            if(g != node2.g()){
                op_logger("MatrixInverseMul")->error("The input variables are not from the same graph.");
                throw InvalidOperatorArgument(NodeVec{node1, node2}, "MatrixInverseMul",
                                              "The input variables are not from the same graph.");
            }
            if(node1.order() != 2){
                op_logger("MatrixInverseMul")->error("Node 0 is not a matrix.");
                throw InvalidOperatorArgument(NodeVec{node1, node2}, "MatrixInverseMul",
                                              "Node 0 is not a matrix.");
            } else if(node1->shape[0] != node1->shape[1]){
                op_logger("MatrixInverseMul")->error("Node 0 is not a square matrix.");
                throw InvalidOperatorArgument(NodeVec{node1, node2}, "MatrixInverseMul",
                                              "Node 0 is not a square matrix.");
            } else if(node1->shape[not transpose_inv] != node2->shape[not transpose_mul]){
                op_logger("MatrixInverseMul")->error("Incorrect shapes - {} not equal to {}.",
                                                     sym::to_string(node1->shape[not transpose_inv]),
                                                     sym::to_string(node2->shape[not transpose_mul]));
                throw InvalidOperatorArgument(NodeVec{node1, node2}, "MatrixInverseMul",
                                              "Incorrect shapes - " + sym::to_string(node1->shape[not transpose_inv])
                                              + " not equal to " + sym::to_string(node2->shape[not transpose_mul]) + ".");
            }
            // TODO check any two consecutive are inverse of each other
            // matrix_inv(matrix_inv(x)) = x
            auto base = get_base_node(node1);
            if(base->op->name == "MatrixInv"){
                return dot(base->op->get_parents()[0], node2, transpose_inv, transpose_mul);
            }
            // matrix_inv(x) * x = I
            if(symbolic_equals(node1, node2)){
                return g->eye(node1->shape[0]);
            }
            Operator op = std::make_shared<op::MatrixInverseMul>(g.get(), node1, node2, transpose_inv, transpose_mul);
            return g->derived_node(op);
        }

        Node determinant(Node node){
            Graph g = node.g();
            // If it is scalar do nothing
            if(node.order() == 0){
                return alias(node);
            } else if(node.order() != 2){
                op_logger("Determinant")->error("Node is not a matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "Determinant",
                                              "Node is not a matrix.");
            } else if(node->shape[0] != node->shape[1]){
                op_logger("Determinant")->error("Node is not a square matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "Determinant",
                                              "Node is not a square matrix.");
            }
            // Standard
            Operator op = std::make_shared<op::Determinant>(g.get(), node);
            return g->derived_node(op);
        }

        Node log_det(Node node){
            Graph g = node.g();
            // If it is scalar do nothing
            if(node.order() == 0){
                return alias(node);
            } else if(node.order() != 2){
                op_logger("LogDeterminant")->error("Node is not a matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "LogDeterminant",
                                              "Node is not a matrix.");
            } else if(node->shape[0] != node->shape[1]){
                op_logger("LogDeterminant")->error("Node is not a square matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "LogDeterminant",
                                              "Node is not a square matrix.");
            }
            // Standard
            Operator op = std::make_shared<op::LogDeterminant>(g.get(), node);
            return g->derived_node(op);
        }

        Node trace(Node node){
            Graph g = node.g();
            // If it is scalar do nothing
            if(node.order() == 0){
                return alias(node);
            } else if(node.order() != 2){
                op_logger("Trace")->error("Node is not a matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "Trace",
                                              "Node is not a matrix.");
            } else if(node->shape[0] != node->shape[1]){
                op_logger("Trace")->error("Node is not a square matrix.");
                throw InvalidOperatorArgument(NodeVec{node}, "Trace",
                                              "Node is not a square matrix.");
            }
            // TODO if node is b8 cast it to max_int
            // Standard
            Operator op = std::make_shared<op::Trace>(g.get(), node);
            return g->derived_node(op);
        }

        Node cholesky_forward_diff_blas(Node cholesky, Node parent_derivative, bool lower){
            if(lower){
                // ls = L^-1 Sigma_dot
                auto ls = matrix_inverse_mul(cholesky, parent_derivative);
                // lsl = L^-1 ls^T = L^-1 Sigma_dot L^-T
                auto lsl = matrix_inverse_mul(cholesky, ls, false, true);
                auto phi = lower_tri(lsl, 0.5);
                return dot(cholesky, phi);
            } else {
                // rs = R^-T Sigma_dot
                auto rs = matrix_inverse_mul(cholesky, parent_derivative, true);
                // rsr = R^-T rs^T = R^-T Sigma_dot R^-1
                auto rsr = matrix_inverse_mul(cholesky, rs, true, true);
                auto phi = upper_tri(rsr, 0.5);
                return dot(phi, cholesky);
            }
        }

        Node cholesky_backward_diff_blas(Node cholesky, Node my_derivative, bool lower){
            if(lower){
                auto phi = lower_tri(dot(cholesky, my_derivative, true, false), 0.5);
                // s1 = L^-T Phi
                auto s1 = matrix_inverse_mul(cholesky, phi, true, false);
                // s2 = L^-T s1^T = L^-T Phi^T L^-1
                auto s2 = matrix_inverse_mul(cholesky, s1, true, true);
                return lower_tri(s2, 0.5) + upper_tri(s2, 0.5);
            } else {
                auto phi = upper_tri(dot(my_derivative, cholesky, false, true), 0.5);
                // s1 = R^-1 Phi
                auto s1 = matrix_inverse_mul(cholesky, phi);
                // s2 = R^-1 s1^T = R^-1 Phi^T R^-T
                auto s2 = matrix_inverse_mul(cholesky, s1, false, true);
                return lower_tri(s2, 0.5) + upper_tri(s2, 0.5);
            }
        }
    }
}