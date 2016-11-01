//
// Created by alex on 19/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{

        Node matrix_mul(NodeVec nodes, std::vector<bool> transpositions){
            // Check there is at least one node
            if(nodes.size() == 0){
                // TODO throw proper error
                utils::op_logger("MatrixMul")->error("0 nodes");
                throw 1;
            }
            Graph g = nodes[0]->g();
            // Check transpositions are correct size
            if(transpositions.size() == 0){
                // Assume default so fill with false
                for(auto i=0;i<nodes.size(); ++i){
                    transpositions.push_back(false);
                }
            } else if(nodes.size() != transpositions.size()){
                // TODO throw proper error
                utils::op_logger("MatrixMul")->error("Different size of nodes and transpositions");
                throw 1;
            }
            if(nodes[0].dims() > 2){
                // TODO throw proper error
                utils::op_logger("MatrixMul")->error("Node 0 is not a matrix");
                throw 1;
            }
            SymInt size = nodes[0]->shape[!transpositions[0]];
            // Check all nodes are from the same graph and their shapes are correct
            for(auto i=1; i < nodes.size(); ++i){
                if(g != nodes[i]->g()){
                    // TODO throw proper error
                    utils::op_logger("MatrixMul")->error("Nodes from different graphs");
                    throw 1;
                } else if(nodes[i].dims() > 2){
                    // TODO throw proper error
                    utils::op_logger("MatrixMul")->error("Node {} is not a matrix", i);
                    throw 1;
                } else if(nodes[i]->shape[transpositions[i]] != size){
                    // TODO throw proper error
                    utils::op_logger("MatrixMul")->error("Incorrect shapes at ", i);
                    throw 1;
                    // TODO check any two consecutive are inverse of each other
                }
                size = nodes[i]->shape[!transpositions[i]];
            }
            // Standard
            auto op = std::make_shared<op::MatrixMul>(g.get(), nodes, transpositions);
            return g->derived_node(op);
        }

        Node dot(Node left, Node right, bool transpose_left, bool transpose_right){
            return matrix_mul(NodeVec{left, right}, std::vector<bool>{transpose_left, transpose_right});
        }

        Node matrix_inverse(Node node){
            Graph g = node->g();
            if(node.dims() > 2){
                // TODO throw proper error
                utils::op_logger("MatrixMul")->error("Node is not a matrix");
                throw 1;
            } else if(node->shape[0] != node->shape[1]){
                // TODO throw proper error
                utils::op_logger("MatrixMul")->error("Node is not a square matrix");
                throw 1;
            }
            // matrix_inv(matrix_inv(x)) = x
            auto base = get_base_node(node);
            if(base->op->name == "MatrixInv"){
                return alias(base->op->get_parents()[0]);
            }
            // Standard
            Operator op = std::make_shared<op::MatrixInverse>(g.get(), node);
            return g->derived_node(op);
        }

        Node matrix_inverse_mul(Node node1, Node node2, bool transpose){
            Graph g = node1->g();
            if(g != node2->g()){
                // TODO throw proper error
                utils::op_logger("MatrixInverse")->error("Nodes from different graphs");
                throw 1;
            }
            if(node1.dims() > 2){
                // TODO throw proper error
                utils::op_logger("MatrixInverse")->error("Node 0 is not a matrix");
                throw 1;
            } else if(node1->shape[0] != node1->shape[1]){
                // TODO throw proper error
                utils::op_logger("MatrixInverse")->error("Node 0 is not a square matrix");
                throw 1;
            } else if(node1->shape[1] != node2->shape[0]){
                // TODO throw proper error
                utils::op_logger("MatrixInverse")->error("Incorrect shapes");
                throw 1;
                // TODO check any two consecutive are inverse of each other
            }
            // matrix_inv(matrix_inv(x)) = x
            auto base = get_base_node(node1);
            if(base->op->name == "MatrixInv"){
                return dot(base->op->get_parents()[0], node2, transpose, false);
            }
            // matrix_inv(x) * x = I
            if(symbolic_equals(node1, node2)){
                return g->eye(node1->shape[0]);
            }
            Operator op = std::make_shared<op::MatrixInverseMul>(g.get(), node1, node2, transpose);
            return g->derived_node(op);
        }

        Node determinant(Node node){
            Graph g = node->g();
            // If it is scalar do nothing
            if(node.dims() == 0){
                return alias(node);
            } else if(node.dims() > 2){
                // TODO throw proper error
                utils::op_logger("Determinant")->error("Node 0 is not a matrix");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::Determinant>(g.get(), node);
            return g->derived_node(op);
        }

        Node log_det(Node node){
            Graph g = node->g();
            // If it is scalar do nothing
            if(node.dims() == 0){
                return alias(node);
            } else if(node.dims() > 2){
                // TODO throw proper error
                utils::op_logger("LogDeterminant")->error("Node 0 is not a matrix");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::LogDeterminant>(g.get(), node);
            return g->derived_node(op);
        }

        Node trace(Node node){
            Graph g = node->g();
            // If it is scalar do nothing
            if(node.dims() == 0){
                return alias(node);
            } else if(node.dims() > 2){
                // TODO throw proper error
                utils::op_logger("Trace")->error("Node 0 is not a matrix");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::Trace>(g.get(), node);
            return g->derived_node(op);
        }
    }
}