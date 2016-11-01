//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_PRINT_H
#define METADIFF_CORE_PRINT_H

namespace md{
    namespace core{

        std::string to_string(DataType data_type);

        inline std::ostream &operator<<(std::ostream &f, const DataType data_type) {
            return f << to_string(data_type);
        }

        std::string to_string(DeviceType device_type);

        inline std::ostream &operator<<(std::ostream &f, const DeviceType device_type) {
            return f << to_string(device_type);
        }

        std::string to_string(Policy policy);

        inline std::ostream &operator<<(std::ostream &f, const Policy policy) {
            return f << to_string(policy);
        }

        std::string to_string(Shape shape);

        inline std::ostream &operator<<(std::ostream &f, const Shape shape) {
            return f << to_string(shape);
        }

        std::string to_string(Device device);

        inline std::ostream &operator<<(std::ostream &f, const Device device) {
            return f << to_string(device);
        }

        std::string to_string(Node node);

        inline std::ostream &operator<<(std::ostream &f, const Node node) {
            return f << to_string(node);
        }

        std::string to_string(NodeVec nodes);

        inline std::ostream &operator<<(std::ostream &f, const NodeVec nodes) {
            return f << to_string(nodes);
        }
    }
}
#endif //METADIFF_CORE_PRINT_H
