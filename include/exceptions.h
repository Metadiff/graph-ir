//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_EXCEPTIONS_H
#define METADIFF_CORE_EXCEPTIONS_H

namespace md{
    namespace core {
        class NotImplementedException: public std::exception {
        public:
            std::string const msg;
            NotImplementedException(std::string const msg): msg(msg) {};
            const char *what() const throw() {
                return (std::string("Not Implemented: ") + msg).c_str();
            }
        };

        /** Abstract class for graph nodes */
        class GraphError : public std::exception {
        public:
            /** List of nodes involved in the error */
            NodeVec const nodes;

            GraphError() {};

            GraphError(NodeVec const nodes) :
                    nodes(nodes) {};

            virtual void log(std::shared_ptr<spdlog::logger> const logger,
                             spdlog::level::level_enum const level = spdlog::level::err) const = 0;

            const char *what() const throw() {
                return "A GraphError occurred";
            }
        };

        typedef std::shared_ptr<GraphError> Error;

        /** The error is raised when the user requests a gradient to a non-scalar value */
        class UnsupportedGradient : public GraphError {
        public:
            UnsupportedGradient() : GraphError() {};

            UnsupportedGradient(Node node) :
                    GraphError(NodeVec{node}) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };


        /** For any other errors */
        class OtherError : public GraphError {
        public:
            std::string msg;
            OtherError() : GraphError() {};

            OtherError(NodeVec inputs, std::string msg) :
                    GraphError(inputs), msg(msg) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        /** An abstract class for any errors related to a specific Operator */
        class OperatorError : public GraphError {
        public:
            std::string op_name;

            OperatorError() : GraphError() {};

            OperatorError(NodeVec const inputs, std::string const op_name) :
                    GraphError(inputs), op_name(op_name) {};
        };


        /** The error is raised when a gradient messsage arrive at a constant node */
        class WrongGradient : public OperatorError {
        public:
            WrongGradient() : OperatorError() {};

            WrongGradient(NodeVec const inputs, std::string const op_name) :
                    OperatorError(inputs, op_name) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };


        class ImplicitBroadcast : public OperatorError {
        public:
            ImplicitBroadcast() : OperatorError() {};

            ImplicitBroadcast(NodeVec const inputs, std::string const op_name) :
                    OperatorError(inputs, op_name) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        class IncompatibleShapes : public OperatorError {
        public:
            IncompatibleShapes() : OperatorError() {};

            IncompatibleShapes(NodeVec const inputs, std::string const op_name) :
                    OperatorError(inputs, op_name) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        class TypePromotion: public OperatorError{
        public:
            dataType from;
            dataType to;

            TypePromotion() : OperatorError() {};

            TypePromotion(NodeVec const inputs,
                          std::string const op_name,
                          dataType from,
                          dataType to) :
                    OperatorError(inputs, op_name), from(from), to(to) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        class InvalidArguments : public OperatorError {
        public:
            std::string reason;
            InvalidArguments() : OperatorError() {};

            InvalidArguments(NodeVec const inputs, std::string const op_name, std::string const reason) :
                    OperatorError(inputs, op_name), reason(reason) {};

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        class MissingRequiredInput : public GraphError {
        public:
            NodeVec targets;
            NodeVec inputs;

            MissingRequiredInput() : GraphError() {};

            MissingRequiredInput(NodeVec targets, NodeVec inputs, Node missing) :
                    GraphError(NodeVec{missing}),
                    targets(targets),
                    inputs(inputs) {}

            void log(std::shared_ptr<spdlog::logger> const logger,
                     spdlog::level::level_enum const level = spdlog::level::err) const;
        };

        class CompilationFailed : public std::exception {
        public:
            std::string msg;

            CompilationFailed() : msg("") {};

            CompilationFailed(std::string msg) :
                    msg("Compilation failed due to:" + msg) {};

            const char *what() const throw() {
                return msg.c_str();
            }
        };

        class InvalidInputShape : public std::exception {
        private:
            std::string generate_message() {
                return "The input to the function at index " + std::to_string(input_index) + "(zero based), "
                        "corresponding to node with id " + std::to_string(id)
                       + " has expected shape("
                       + std::to_string(expected_shape[0]) +  ","
                       + std::to_string(expected_shape[1]) + ","
                       + std::to_string(expected_shape[2]) + ","
                       + std::to_string(expected_shape[3]) + "), "
                               "but the provided input had shape("
                       + std::to_string(provided_shape[0]) + ","
                       + std::to_string(provided_shape[1]) + ","
                       + std::to_string(provided_shape[2]) + ","
                       + std::to_string(provided_shape[3]) + ").";
            }

        public:
            size_t input_index;
            size_t id;
            std::array<size_t, 4> expected_shape;
            std::array<size_t, 4> provided_shape;
            std::string msg;

            InvalidInputShape() : msg("") {};

            InvalidInputShape(size_t const input_index,
                              size_t const id,
                              std::array<size_t, 4> expected_shape,
                              std::array<size_t, 4> provided_shape) :
                    input_index(input_index), id(id),
                    expected_shape(expected_shape),
                    provided_shape(provided_shape),
                    msg(generate_message()) {};

            const char *what() const throw() {
                return msg.c_str();
            }
        };

        /** Given an error executes the errorPolicy */
        inline void operate_policy(policy policy,
                                   std::shared_ptr<spdlog::logger> const logger,
                                   Error const & err){
            switch (policy){
                case QUIET: return;
                case WARN: {
                    err->log(logger, spdlog::level::warn);
                    return;
                }
                default: {
                    err->log(logger, spdlog::level::err);
                    throw err;
                }
            }
        }
    }
}

#endif //METADIFF_CORE_EXCEPTIONS_H
