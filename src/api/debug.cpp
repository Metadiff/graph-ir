//
// Created by alex on 31/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api{
        Node print(Node monitored, std::string msg, Node anchor){
            Graph g = monitored->g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if (not check_independent(anchor, monitored)) {
                // TODO throw proper exception
                utils::op_logger("Print")->error("Anchor is before monitored");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::Print>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node retrieve(Node monitored, std::string msg, Node anchor){
            Graph g = monitored->g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if (not check_independent(anchor, monitored)) {
                // TODO throw proper exception
                utils::op_logger("Retrieve")->error("Anchor is before monitored");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::Retrieve>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node log_to_file(Node monitored, std::string msg, Node anchor){
            Graph g = monitored->g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if (not check_independent(anchor, monitored)) {
                // TODO throw proper exception
                utils::op_logger("LogToFile")->error("Anchor is before monitored");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::LogToFile>(g.get(), anchor, monitored, msg);
            return g->derived_node(op);
        }

        Node guard(Node monitored, std::string msg, double low, double high, Node anchor){
            Graph g = monitored->g();
            if(anchor.ptr.expired()) {
                // If the anchor is empty use the monitored
                anchor = monitored;
            }
            if (not check_independent(anchor, monitored)) {
                // TODO throw proper exception
                utils::op_logger("Guard")->error("Anchor is before monitored");
                throw 1;
            }
            if (low >= high){
                // TODO throw proper exception
                utils::op_logger("Guard")->error("low is not <= high");
                throw 1;
            }
            // Standard
            Operator op = std::make_shared<op::Guard>(g.get(), anchor, monitored, msg, low, high);
            return g->derived_node(op);
        }
    }
}
