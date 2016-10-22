//
// Created by alex on 06/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        Node GraphInternal::tensor4(dataType data_type,
                                    Shape shape,
                                    std::string name) {
            auto op = std::make_shared<op::Input>(shared_from_this().get(), data_type, shape);
            auto result = std::make_shared<NodeData>(
                    shared_from_this().get(),
                    nodes.size(),
                    name,
                    default_device,
                    op,
                    0,
                    current_group);
            nodes.push_back(result);
            result->op->owner = result;
            return result;
        }

        Node GraphInternal::tensor4(dataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    SymInt shape3,
                                    std::string name) {
            Shape shape{shape0,
                        shape1,
                        shape2,
                        shape3};
            return tensor4(data_type, shape, name);
        }

        Node GraphInternal::tensor4(dataType data_type,
                                    std::string name) {
            Shape shape = {
                    get_new_symbolic_integer(),
                    get_new_symbolic_integer(),
                    get_new_symbolic_integer(),
                    get_new_symbolic_integer()
            };
            return tensor4(data_type, shape, name);
        }

        Node GraphInternal::tensor4_as(Node node, std::string name) {
            return tensor4(node->data_type, node->shape, name);
        }

        Node GraphInternal::tensor3(dataType data_type,
                                    std::array<SymInt, 3> shape,
                                    std::string name) {
            return tensor4(data_type, {shape[0], shape[1], shape[2], 1}, name);
        }

        Node GraphInternal::tensor3(dataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    std::string name) {
            return tensor4(data_type, Shape{
                                   shape0,
                                   shape1,
                                   shape2,
                                   1
                           }, name);
        }

        Node GraphInternal::tensor3(dataType data_type,
                                    std::string name) {
            auto shape0 = get_new_symbolic_integer();
            auto shape1 = get_new_symbolic_integer();
            auto shape2 = get_new_symbolic_integer();
            return tensor3(data_type, shape0, shape1, shape2, name);
        }


        Node GraphInternal::tensor3_as(Node node, std::string name) {
            if(node.dims() > 3){
                auto err = std::make_shared<InvalidArguments>(NodeVec{node},
                                                              "tensor3_as",
                                                              "The variables is not a tensor3");
                err->log(logger());
                throw err;
            }
            return tensor3(nodes[node->id]->data_type,
                           nodes[node->id]->shape[0],
                           nodes[node->id]->shape[1],
                           nodes[node->id]->shape[2],
                           name);
        }

        Node GraphInternal::matrix(dataType data_type,
                                   std::array<SymInt, 2> shape,
                                   std::string name) {
            return tensor4(data_type, {shape[0], shape[1], 1, 1}, name);
        }

        Node GraphInternal::matrix(dataType data_type,
                                   SymInt shape0,
                                   SymInt shape1,
                                   std::string name) {
            return tensor4(data_type, Shape{
                                   shape0,
                                   shape1,
                                   1,
                                   1},
                           name);
        }

        Node GraphInternal::matrix(dataType data_type,
                                   std::string name) {
            auto shape0 = get_new_symbolic_integer();
            auto shape1 = get_new_symbolic_integer();
            return matrix(data_type, shape0, shape1, name);
        }


        Node GraphInternal::matrix_as(Node node, std::string name) {
            if (node.dims() > 2) {
                auto err = std::make_shared<InvalidArguments>(NodeVec{node},
                                                              "matrix_as",
                                                              "The variables is not a matrix");
                err->log(logger());
                throw err;
            }
            return matrix(nodes[node->id]->data_type,
                          nodes[node->id]->shape[0],
                          nodes[node->id]->shape[1],
                          name);
        }

        Node GraphInternal::square_matrix(dataType data_type,
                                          SymInt shape,
                                          std::string name) {
            return tensor4(data_type, {shape, shape, 1, 1}, name);
        }

        Node GraphInternal::vector(dataType data_type,
                                   SymInt shape,
                                   std::string name) {
            return tensor4(data_type, {shape, 1, 1, 1}, name);
        }

        Node GraphInternal::vector(dataType data_type,
                                   std::string name) {
            auto shape0 = get_new_symbolic_integer();
            return vector(data_type, shape0, name);
        }


        Node GraphInternal::vector_as(Node node,
                                      std::string name) {
            if (node.dims() > 1) {
                auto err = std::make_shared<InvalidArguments>(NodeVec{node},
                                                              "vector_as",
                                                              "The variables is not a vector");
                err->log(logger());
                throw err;
            }
            return vector(nodes[node->id]->data_type,
                          nodes[node->id]->shape[0],
                          name);
        }

        Node GraphInternal::scalar(dataType data_type,
                                   std::string name) {
            return tensor4(data_type, {1, 1, 1, 1}, name);
        }

        Node GraphInternal::wrap(SharedVar var) {
            Operator op = std::make_shared<op::SharedInput>(this, var);
            Node node = derived_node(op);
            node->name = var->name;
            return node;
        }

        Node GraphInternal::wrap(SymInt value) {
            Operator op = std::make_shared<op::SymIntWrapper>(this, value);
            return derived_node(op);
        }


    }
}
