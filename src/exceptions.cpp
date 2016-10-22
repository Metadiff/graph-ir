//
// Created by alex on 06/10/16.
//

#include "metadiff.h"

namespace md {
    namespace core {
        void UnsupportedGradient::log(std::shared_ptr<spdlog::logger> const logger,
                                      const spdlog::level::level_enum level) const {
            logger->log(level,
                        "Requested gradient with respect to node {}, but it is not a scalar.",
                        to_string(nodes[0]));
        }

        void OtherError::log(std::shared_ptr<spdlog::logger> const logger,
                             const spdlog::level::level_enum level) const {
            logger->log(level, "{}. Nodes involved:{}", msg, to_string(nodes));
        }

        void WrongGradient::log(std::shared_ptr<spdlog::logger> const logger,
                                const spdlog::level::level_enum level) const {
            logger->log(level,
                        "The gradient node with id {} was sent to node with id {} "
                                "and operator {}, "
                                "but all the parents of that node are constant.",
                        nodes[1]->id, nodes[0]->id, op_name);
        }

        void ImplicitBroadcast::log(std::shared_ptr<spdlog::logger> const logger,
                                    const spdlog::level::level_enum level) const {
            logger->log(level,
                        "Implicit broadcast in operator {} of nodes:\n{}",
                        op_name,
                        to_string(nodes));
        }

        void IncompatibleShapes::log(std::shared_ptr<spdlog::logger> const logger,
                                     const spdlog::level::level_enum level) const {
            logger->log(level,
                        "Incompatible shapes in operator {} of nodes: {}",
                        op_name,
                        to_string(nodes));
        }

        void TypePromotion::log(std::shared_ptr<spdlog::logger> const logger,
                                const spdlog::level::level_enum level) const {
            logger->log(level,
                        "Promoting expected type {} to {} in operator {}.\nNodes: {}",
                        from,
                        to,
                        op_name,
                        to_string(nodes));
        }


        void InvalidArguments::log(std::shared_ptr<spdlog::logger> const logger,
                                   const spdlog::level::level_enum level) const {
            logger->log(level,
                        "Invalid arguments to operator {}. Reason: {}.\nNodes: {}",
                        op_name,
                        reason,
                        to_string(nodes));
        }

        void MissingRequiredInput::log(std::shared_ptr<spdlog::logger> const logger,
                                       const spdlog::level::level_enum level) const {
            logger->log(level, "Missing required input when trying to compile the graph.\n"
                                "Missing node: {}\nTarget nodes: {}\nProvided inputs: {}",
                        to_string(nodes[0]), to_string(targets), to_string(inputs));
        }

    }
}
//                std::string msg = "";
//                msg += "Missing required input when trying to compile a function."  "\n"
//                               "Missing node:"  "\n"
//                       + to_string(missing) + "\n"  "Target nodes:"  "\n";
//                for (size_t i = 0; i < targets.size(); i++) {
//                    msg += to_string(targets[i]) + "\n";
//                }
//                msg += "Provided inputs:" "\n";
//                for (size_t i = 0; i < inputs.size(); i++) {
//                    if (i < inputs.size() - 1) {
//                        msg += to_string(inputs[i]) + "\n";
//                    } else {
//                        msg += to_string(inputs[i]);
//                    }
//                }
//                return msg;
//            }