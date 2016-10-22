//
// Created by alex on 30/09/16.
//

#include "string"
#include "iomanip"
#include "sstream"
#include "metadiff.h"

namespace md{
    namespace core{
        std::string to_string(dataType data_type) {
            switch (data_type){
                case b8: return "b8";
                case u8: return "u8";
                case u16: return "u16";
                case u32: return "u32";
                case u64: return "u64";
                case i8: return "i8";
                case i16: return "i16";
                case i32: return "i32";
                case i64: return "i64";
                case f8: return "f8";
                case f16: return "f16";
                case f32: return "f32";
                default: return "f64";
//                default: return "f64";
            }
        }

        std::string to_string(deviceType device_type) {
            switch (device_type){
                case CPU: return "CPU";
                default: return "GPU";
//                case GPU: return "GPU";
            }
        }

        std::string to_string(nodeType node_type) {
            switch (node_type){
                case CONSTANT: return "Const";
                case CONSTANT_DERIVED: return "ConstDerived";
                case INPUT: return "Input";
                case INPUT_DERIVED: return "InputDerived";
                default: return "InputDerivedConst";
//                case INPUT_DERIVED_CONSTANT: return "InputDerivedConst";
            }
        }

        std::string to_string(errorPolicy policy) {
            switch (policy){
                case QUIET: return "Quiet";
                case WARN: return "Warn";
                default: return "Raise";
//                case RAISE: return "Raise";
            }
        }

        std::string to_string(Device device){
            if(device == host()){
                return "Host";
            }
            switch (device.type) {
                case CPU: return "Cpu[" + std::to_string(device.id) + "]";
                default: return "Gpu[" + std::to_string(device.id) + "]";
//                case GPU:
            }
        }

        std::string to_string(Shape shape){
            return "(" + shape[0].to_string() + "," +
                   shape[1].to_string() + "," +
                   shape[2].to_string() + "," +
                   shape[3].to_string() + ")";
        }

        std::string to_string(Node node){
            std::stringstream ss;
            ss << "|"  << std::setw(4) << node->id << "|" << std::setw(12) << node->node_type << "|"
            << std::setw(20) << node->shape << "|" << node->data_type << "|" << node->op->name;
            return ss.str();
        }

        std::string to_string(NodeVec nodes){
            std::stringstream ss;
            for (size_t i = 0; i < nodes.size(); i++) {
                ss << nodes[i];
                if (i < nodes.size() - 1) {
                    ss << std::endl;
                }
            }
            return ss.str();
        }
    }
}

