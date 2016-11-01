//
// Created by alex on 27/10/16.
//

#ifndef METADIFF_API_INPUT_H
#define METADIFF_API_INPUT_H
namespace md{
    namespace api {

//        /** @brief Creates a four dimensional variable in the default_graph
//         *
//         * @param data_type
//         * @param shape
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor4(DataType data_type,
//                     std::array<SymInt, 4> shape,
//                     std::string name = "Tensor4",
//                     Graph g = default_graph);
//
//        /** @brief Creates a four dimensional variable in the default_graph
//         *
//         * @param data_type
//         * @param shape0
//         * @param shape1
//         * @param shape2
//         * @param shape3
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor4(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     SymInt shape3,
//                     std::string name = "Tensor4",
//                     Graph g = default_graph);
//
//        /** @brief Creates a four dimensional variable in default_graph
//         *
//         * @param data_type
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor4(DataType data_type,
//                     std::string name = "Tensor4",
//                     Graph g = default_graph);
//
//        /** @brief Creates a three dimensional variable in the default_graph
//         *
//         * @param data_type
//         * @param shape
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor3(DataType data_type,
//                     std::array<SymInt, 3> shape,
//                     std::string name = "Tensor3",
//                     Graph g = default_graph);
//
//        /** @brief Creates a three dimensional variable in the default_graph
//         *
//         * @param data_type
//         * @param shape0
//         * @param shape1
//         * @param shape2
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor3(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     std::string name = "Tensor3",
//                     Graph g = default_graph);
//
//        /** @brief Creates a three dimensional variable in the default_graph
//         *
//         * @param data_type
//         * @param name
//         * @param g
//         * @return
//         */
//        Node tensor3(DataType data_type,
//                     std::string name = "Tensor3",
//                     Graph g = default_graph);
//
//        /** @brief Creates a matrix variable in the default_graph
//         *
//         * @param data_type
//         * @param shape
//         * @param name
//         * @param g
//         * @return
//         */
//        Node matrix(DataType data_type,
//                    std::array<SymInt, 2> shape,
//                    std::string name = "Matrix",
//                    Graph g = default_graph);
//
//        /** @brief Creates a matrix variable in the default_graph
//         *
//         * @param data_type
//         * @param shape0
//         * @param shape1
//         * @param name
//         * @param g
//         * @return
//         */
//        Node matrix(DataType data_type,
//                    SymInt shape0,
//                    SymInt shape1,
//                    std::string name = "Matrix",
//                    Graph g = default_graph);
//
//        /** @brief Creates a matrix variable in the default_graph
//         *
//         * @param data_type
//         * @param name
//         * @param g
//         * @return
//         */
//        Node matrix(DataType data_type,
//                    std::string name = "Matrix",
//                    Graph g = default_graph);
//
//        /** @brief Creates a square matrix variable in the default_graph
//         *
//         * @param data_type
//         * @param shape
//         * @param name
//         * @param g
//         * @return
//         */
//        Node square_matrix(DataType data_type,
//                           SymInt shape,
//                           std::string name = "Matrix",
//                           Graph g = default_graph);
//
//        /** @brief Creates a row vector variable in the default_graph
//         *
//         * @param data_type
//         * @param shape
//         * @param name
//         * @param g
//         * @return
//         */
//        Node vector(DataType data_type,
//                    SymInt shape,
//                    std::string name = "Vector",
//                    Graph g = default_graph);
//
//        /** @brief Creates a row vector variable in the default_graph
//         *
//         * @param data_type
//         * @param name
//         * @param g
//         * @return
//         */
//        Node vector(DataType data_type,
//                    std::string name = "Vector",
//                    Graph g = default_graph);
//
//        /** @brief Creates a scalar variable in the default_graph
//         *
//         * @param data_type
//         * @param name
//         * @param g
//         * @return
//         */
//        Node scalar(DataType data_type,
//                    std::string name = "Scalar",
//                    Graph g = default_graph);

        /** @brief Creates a variable in the default_graph, with the same attributes as the one provided
         *
         * @param node
         * @param name
         * @return
         */
        Node tensor_as(Node node, std::string name = "");

        /** @brief Creates a new symbloic integer
         *
         * @return
         */
        inline SymInt new_sym(){
            return sym::registry()->new_variable();
        }
    }
}

#endif //METADIFF_API_INPUT_H
