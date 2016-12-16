//
// Created by alex on 27/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace gir{

        Node GraphInternal::parameter(std::string scope, std::string name, DataType data_type, Shape shape){
            auto var_name = scope;
            var_name += props.scope_delimiter;
            var_name += name;
            Operator op = std::make_shared<op::Parameter>(this, var_name, data_type, shape);
            Node node = derived_node(op);
            node->name = var_name;
            return node;
        }

        Node GraphInternal::parameter(std::string name, DataType data_type, Shape shape){
            return parameter(scope, name, data_type, shape);
        }

        Node GraphInternal::tensor4(DataType data_type,
                                    std::array<SymInt, 4> shape,
                                    std::string name) {
            auto op = std::make_shared<op::Input>(this, data_type, shape);
            auto result = std::make_shared<NodeData>(shared_from_this(), nodes.size(),
                                                     name, props.default_device,
                                                     op, 0, scope);
            nodes.push_back(result);
            result->op->result = result;
            // Add the node to the group map
            if(group_map.find(scope) == group_map.end()){
                group_map[scope] = NodeVec{result};
            } else {
                group_map[scope].push_back(result);
            }
            // Add the node to the op map
            if(op_map.find(op->name) == op_map.end()){
                op_map[op->name] = NodeVec{result};
            } else {
                op_map[op->name].push_back(result);
            }
            return result;
        }

        Node GraphInternal::tensor4(DataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    SymInt shape3,
                                    std::string name) {
            return tensor4(data_type, {shape0, shape1, shape2, shape3}, name);
        }

        Node GraphInternal::tensor4(DataType data_type,
                                    std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), new_sym()}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    std::array<SymInt, 3> shape,
                                    std::string name) {
            return tensor4(data_type, {shape[0], shape[1], shape[2], 1}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    std::string name) {
            return tensor4(data_type, {shape0, shape1, shape2, 1}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   std::array<SymInt, 2> shape,
                                   std::string name) {
            return tensor4(data_type, {shape[0], shape[1], 1, 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   SymInt shape0,
                                   SymInt shape1,
                                   std::string name) {
            return tensor4(data_type, {shape0, shape1, 1, 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), 1, 1}, name);
        }

        Node GraphInternal::square_matrix(DataType data_type,
                                          SymInt shape,
                                          std::string name) {
            return tensor4(data_type, {shape, shape, 1, 1}, name);
        }

        Node GraphInternal::vector(DataType data_type,
                                   SymInt shape,
                                   std::string name) {
            return tensor4(data_type, {shape, 1, 1, 1}, name);
        }

        Node GraphInternal::vector(DataType data_type,
                                   std::string name) {
            return tensor4(data_type, {new_sym(), 1, 1, 1}, name);
        }

        Node GraphInternal::scalar(DataType data_type, std::string name) {
            return tensor4(data_type, {1, 1, 1, 1}, name);
        }

        Node GraphInternal::tensor_as(Node node, std::string name) {
            if (name == "") {
                name = node->name + "_clone";
            }
            return node.g()->tensor4(node->data_type, node->shape, name);
        }
    }
//    namespace api {
//
//        Node tensor_as(Node node, std::string name) {
//            return node.g()->tensor_as(node, name);
//        }
//
//        Node tensor4(DataType data_type,
//                     std::array<SymInt, 4> shape,
//                     std::string name,
//                     Graph g) {
//            auto op = std::make_shared<op::Input>(g.get(), data_type, shape);
//            auto result = std::make_shared<NodeData>(g, g->nodes.size(),
//                    name, g->props.default_device,
//                    op, 0, g->current_group);
//            g->nodes.push_back(result);
//            result->op->result = result;
//            return result;
//        }
//
//        Node tensor4(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     SymInt shape3,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape0, shape1, shape2, shape3}, name, g);
//        }
//
//        Node tensor4(DataType data_type,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), new_sym()}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     std::array<SymInt, 3> shape,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape[0], shape[1], shape[2], 1}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape0, shape1, shape2, 1}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    std::array<SymInt, 2> shape,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape[0], shape[1], 1, 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    SymInt shape0,
//                    SymInt shape1,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape0, shape1, 1, 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), 1, 1}, name, g);
//        }
//
//        Node square_matrix(DataType data_type,
//                           SymInt shape,
//                           std::string name,
//                           Graph g) {
//            return tensor4(data_type, {shape, shape, 1, 1}, name, g);
//        }
//
//        Node vector(DataType data_type,
//                    SymInt shape,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape, 1, 1, 1}, name, g);
//        }
//
//        Node vector(DataType data_type,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {new_sym(), 1, 1, 1}, name, g);
//        }
//
//        Node scalar(DataType data_type, std::string name, Graph g) {
//            return tensor4(data_type, {1, 1, 1, 1}, name, g);
//        }
//
//        Node tensor_as(Node node, std::string name) {
//            if(name == ""){
//                name = node->name + "_clone";
//            }
//            return tensor4(node->data_type, node->shape, name, node.g());
//        }
//    }
}