//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_EXCEPTIONS_H
#define METADIFF_CORE_EXCEPTIONS_H

namespace md{
    namespace core {

        class GraphError : public std::exception {
        public:
            NodeVec const nodes;
            std::string const msg;

            GraphError() : msg("") {};

            GraphError(NodeVec const nodes,
                       std::string const msg) :
                    nodes(nodes), msg(msg) {};

            const char *what() const throw() {
                return msg.c_str();
            }
        };

        class UnsupportedGradient : public GraphError {
        public:
            UnsupportedGradient() : GraphError() {};

            UnsupportedGradient(Node node) :
                    GraphError(NodeVec{node},
                               "Error: Taking gradient is only possible with respect to scalar objectives. "
                               + to_string(node)){};
        };

        class WrongGradient : public GraphError {
        public:
            WrongGradient() : GraphError() {};

            WrongGradient(NodeVec inputs, std::string op_name) :
                    GraphError(inputs,
                               "Error: The gradient node with id " + std::to_string(inputs[1]->id) +
                               " was sent to node with id " + std::to_string(inputs[0]->id) +
                               " and operator " + inputs[0]->op->name + " , but all its parents are constant. " +
                               to_string(inputs)) {}
        };

        class OtherError : public GraphError {
        public:
            OtherError() : GraphError() {};

            OtherError(NodeVec inputs, std::string msg) :
                    GraphError(inputs, "Error: " + msg + " " + to_string(inputs)) {};
        };

        class OperatorError : public GraphError {
        public:
            std::string const op_name;
            std::string const err;

            OperatorError() : GraphError() {};

            OperatorError(NodeVec inputs, std::string op_name, std::string err) :
                    GraphError(inputs, err + " " + to_string(inputs)), op_name(op_name), err(err) {}
        };

        class ImplicitBroadcast : public OperatorError {
        public:
            ImplicitBroadcast() : OperatorError() {};

            ImplicitBroadcast(NodeVec inputs, std::string op_name) :
                    OperatorError(inputs, op_name, "Performing implicit broadcast.") {}
        };

        class IncompatibleShapes : public OperatorError {
        public:
            IncompatibleShapes() : OperatorError() {};

            IncompatibleShapes(NodeVec inputs, std::string op_name) :
                    OperatorError(inputs, op_name, "Incompatible shapes of inputs") {}
        };

        class InvalidArguments : public OperatorError {
        public:
            InvalidArguments() : OperatorError() {};

            InvalidArguments(NodeVec inputs, std::string op_name, std::string err) :
                    OperatorError(inputs, op_name, err) {}
        };

        class MissingRequiredInput : public std::exception {
        private:
            std::string generate_message() {
                std::stringstream msg;
                msg << "Error: Missing required input when trying to compile a function.\n"
                    << "Missing node:" << std::endl
                    << missing << std::endl << "Target nodes:" << std::endl;
                for (size_t i = 0; i < targets.size(); i++) {
                    msg << targets[i] << std::endl;
                }
                msg << "Provided inputs:" << std::endl;
                for (size_t i = 0; i < inputs.size(); i++) {
                    msg << inputs[i];
                    if (i < inputs.size() - 1) {
                        msg << std::endl;
                    }
                }
                return msg.str();
            }

        public:
            NodeVec targets;
            NodeVec inputs;
            Node missing;
            std::string msg;

            MissingRequiredInput() : msg("") {};

            MissingRequiredInput(NodeVec targets, NodeVec inputs, Node missing) :
                    targets(targets),
                    inputs(inputs),
                    missing(missing),
                    msg(generate_message()) {}

            const char *what() const throw() {
                return msg.c_str();
            }
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
                std::stringstream msg;
                msg << "The input to the function at index " << input_index << "(zero based), "
                    << "corresponding to node with id " << id << " has expected shape"
                    << "(" << expected_shape[0] << "," << expected_shape[1] << ","
                    << expected_shape[2] << "," << expected_shape[3] << "), "
                    << "but the provided input had shape"
                    << "(" << provided_shape[0] << "," << provided_shape[1] << ","
                    << provided_shape[2] << "," << provided_shape[3] << ").";
                return msg.str();
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
        inline void operate_policy(errorPolicy policy,
                                   std::shared_ptr<spdlog::logger> const logger,
                                   GraphError const & err){
            switch (policy){
                case QUIET: return;
                case WARN: {
                    logger->warn(err.msg);
                    return;
                }
                default: {
                    logger->error(err.msg);
                    throw err;
                }
            }
        }
    }
}

#endif //METADIFF_CORE_EXCEPTIONS_H
