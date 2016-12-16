//
// Created by alex on 07/11/16.
//

#include "mock.h"
typedef std::chrono::high_resolution_clock timer;

namespace md{
    namespace backend{
        namespace mock {
            std::shared_ptr<MockStorage> MockBackend::get_param(std::string full_name) const {
                auto found = params.find(full_name);
                if( found != params.end()){
                    return params.at(full_name);
                } else {
                    std::shared_ptr<MockStorage> result;
                    return result;
                }
            };

            std::shared_ptr<MockStorage> MockBackend::initialize_param(std::string full_name,
                                                                       DataType data_type,
                                                                       std::array<long, 4> shape) {
                auto found = params.find(full_name);
                if( found != params.end()){
                    backend_logger(name)->info("Trying to repeat initialization of parameter {}", full_name);
                    return params.at(full_name);
                } else {
                    backend_logger(name)->debug("Initializing parameter {}", full_name);
                    params[full_name] = std::make_shared<MockStorage>(data_type, shape);
                    Var result = params[full_name];

                    for(auto i=0; i < result->total_size(); ++i){
                        result->get<float>()[i] = 0.0;
                    }
                    return result;
                }
            }

            void * MockBackend::request(uint64_t size){
                backend_logger(name)->debug("Requesting {} bytes of memory", size);
                if(not allocated or current_size < size) {
                    release();
                    backend_logger(name)->debug("Allocating {} bytes of memory", size);
                    memory = std::malloc(4 * size);
                    current_size = size;
                }
                return memory;
            }

            void MockBackend::release(){
                if(allocated) {
                    backend_logger(name)->debug("Releasing {} bytes of memory", current_size);
                    free(memory);
                }
                allocated = true;
            }

            void AbstractMockFunction::initialize(std::vector<std::pair<SymInt, int64_t >> const & provided) {
                auto deduced = sym::deduce_values(provided);
                auto ps = gf.graph->op_map["Parameter"];
                std::array<long, 4> shape;
                for(auto i=0; i<ps.size(); ++i){
                    auto cast_op = std::dynamic_pointer_cast<op::Parameter>(ps[i]->op);
                    for(auto j=0; j<4; ++j){
                        try {
                            shape[j] = cast_op->shape[j].eval(deduced);
                        } catch (const std::runtime_error& error) {
                            std::string const msg =
                                    fmt::format("The provided sizes were not enough to deduce the shape "
                                                        "of parameter {} with shape {}",
                                                cast_op->full_name, to_string(cast_op->shape));
                            function_logger(gf.name)->error(msg);
                            throw std::invalid_argument(msg);
                        }
                    }
                    params.push_back(backend->initialize_param(cast_op->full_name, cast_op->data_type, shape));
                }
                implicit = provided;
                initialized = true;
            }

            Outputs AbstractMockFunction::eval(VarVec & inputs) {
                auto const start = timer::now();
                // Check correct number of arguments
                if(inputs.size() != gf.inputs.size()){
                    std::string const msg = fmt::format("Incorrect number of inputs. Expected {}, but got {}.",
                                                        gf.inputs.size(), inputs.size());
                    function_logger(gf.name)->error("{}", msg);
                    throw std::invalid_argument(std::move(msg));
                }
                std::vector<std::array<long, 4>> input_shapes(inputs.size());
                std::transform(inputs.begin(), inputs.end(), input_shapes.begin(), [](Var x){return x->shape;});
                // Verify arguments
                bool changed;
                try {
                    changed = verify_shapes(input_shapes, last_shapes, gf.inputs, last_deduced, implicit);
                } catch (std::exception & e){
                    function_logger(gf.name)->error(
                            "{} Please verify the shape of the inputs, "
                                    "the shapes of the parameters as well as that the graph "
                                    "the constraints on these shapes are met.", e.what());
                    throw e;
                }
                if(changed) {
                    // Update last shapes
                    last_shapes = std::move(input_shapes);
                    // Calculate memory
                    manager->calculate_exact_map(last_deduced);
                    // Set memory
                    manager->set_memory(backend->request(manager->current_size));
                }
                auto const pre = timer::now();
                // Execute function
                auto const result = this->internal_eval(inputs, constants, params, manager, last_deduced);
                auto const end = timer::now();
                function_logger(gf.name)->debug("Execution time: {}ms, Overhead: {}ms",
                                                std::chrono::duration_cast<std::chrono::microseconds>(end - pre).count(),
                                                std::chrono::duration_cast<std::chrono::microseconds>(pre - start).count());
                return std::move(result);
            }
        }
    }
}

