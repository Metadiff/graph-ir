//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_GRAPH_IR_PRINT_H
#define METADIFF_GRAPH_IR_PRINT_H

namespace md{
    namespace gir{
        std::string to_string(Precision const precision);

        inline std::ostream &operator<<(std::ostream &f, Precision const precision) {
            return f << to_string(precision);
        }

        std::string to_string(DataType const data_type);

        inline std::ostream &operator<<(std::ostream &f, DataType const data_type) {
            return f << to_string(data_type);
        }

        std::string to_string(DeviceType const device_type);

        inline std::ostream &operator<<(std::ostream &f, DeviceType const device_type) {
            return f << to_string(device_type);
        }

        std::string to_string(Policy const policy);

        inline std::ostream &operator<<(std::ostream &f, Policy const policy) {
            return f << to_string(policy);
        }

        std::string to_string(Shape const shape);

        inline std::ostream &operator<<(std::ostream &f, Shape const shape) {
            return f << to_string(shape);
        }

        std::string to_string(Device const device);

        inline std::ostream &operator<<(std::ostream &f, Device const device) {
            return f << to_string(device);
        }

        std::string to_string(Node const node);

        inline std::ostream &operator<<(std::ostream &f, Node const node) {
            return f << to_string(node);
        }

        std::string to_string(NodeVec const & nodes);

        inline std::ostream &operator<<(std::ostream &f, NodeVec const & nodes) {
            return f << to_string(nodes);
        }
    }
}
#endif //METADIFF_GRAPH_IR_PRINT_H
