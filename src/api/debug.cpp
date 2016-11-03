//
// Created by alex on 31/10/16.
//

#include "graph_ir.h"

namespace md{
    namespace api{
        Node print(Node monitored, std::string msg, Node anchor){
            Graph g = monitored.g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if(is_dependent(anchor, monitored)){
                op_logger("Print")->error("The anchor {} is dependent on the monitored {}.", anchor->id, monitored->id);
                throw InvalidOperatorArgument({anchor, monitored}, "Print", "The anchor is dependent on the monitored.");
            }
            // Standard
            Operator op = std::make_shared<op::Print>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node retrieve(Node monitored, std::string msg, Node anchor){
            Graph g = monitored.g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if(is_dependent(anchor, monitored)){
                op_logger("Retrieve")->error("The anchor {} is dependent on the monitored {}.", anchor->id, monitored->id);
                throw InvalidOperatorArgument({anchor, monitored}, "Retrieve", "The anchor is dependent on the monitored.");
            }
            // Standard
            Operator op = std::make_shared<op::Retrieve>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node log_to_file(Node monitored, std::string msg, Node anchor){
            Graph g = monitored.g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if(is_dependent(anchor, monitored)){
                op_logger("LogToFile")->error("The anchor {} is dependent on the monitored {}.", anchor->id, monitored->id);
                throw InvalidOperatorArgument({anchor, monitored}, "LogToFile",
                                              "The anchor " + std::to_string(anchor->id)
                                              + " is dependent on the monitored " + std::to_string(monitored->id) + ".");
            }
            // Standard
            Operator op = std::make_shared<op::LogToFile>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node guard(Node monitored, std::string msg, double low, double high, Node anchor){
            Graph g = monitored.g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if(is_dependent(anchor, monitored)){
                op_logger("Guard")->error("The anchor {} is dependent on the monitored {}.", anchor->id, monitored->id);
                throw InvalidOperatorArgument({anchor, monitored}, "Guard",
                                              "The anchor " + std::to_string(anchor->id)
                                              + " is dependent on the monitored " + std::to_string(monitored->id) + ".");
            }
            if (low >= high){
                op_logger("Guard")->error("The low value - {} is not < the high value {}.", low, high);
                throw InvalidOperatorArgument({anchor, monitored}, "Guard",
                                              "The low value - " + std::to_string(low)
                                              + " is not < the high value " + std::to_string(high) + ".");
            }
            // Standard
            Operator op = std::make_shared<op::Guard>(g.get(), anchor, monitored, msg, low, high);
            return g->derived_node(op);
        }
    }
}
