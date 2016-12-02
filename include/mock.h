//
// Created by alex on 07/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_MOCK_H
#define METADIFF_GRAPH_IR_BACKEND_MOCK_H

namespace md{
    namespace backend{
        namespace mock {
            // Forward declarations
            class MockStorage;
            class MockMemoryStorage;
            class MockFunction;
            class MockBackend;
            typedef std::shared_ptr<MockStorage> Var;
            typedef std::vector<Var> VarVec;
            typedef std::pair<VarVec, VarVec> Outputs;
            typedef dll_symbol<Outputs(VarVec &inputs, VarVec &constants, VarVec &params,
                                       MemoryManager manager,
                                       std::unordered_map<sym::I, sym::C> & deduced)> symbol;

            class MockStorage {
            public:
                DataType const data_type;
                std::array<long, 4> const shape;
                void *data;

                MockStorage(DataType data_type, std::array<long, 4> shape) :
                        data_type(data_type), shape(shape) {
                    data = std::malloc(total_size() * 4);
                }

                ~MockStorage() {
                    free(data);
                }

                long total_size() {
                    return shape[0] * shape[1] * shape[2] * shape[3];
                }

                template<typename T>
                T *get() {
                    return (T *) data;
                }
            };

            inline std::shared_ptr<MockStorage> make_var(DataType data_type, std::array<long, 4> shape) {
                return std::make_shared<MockStorage>(data_type, shape);
            }

            class MockMemoryManager : public AbstractMemoryManager {
            public:
                void *memory;
                std::unordered_map<size_t, std::pair<SymInt, SymInt>> abstract_map;
                std::unordered_map<size_t, std::pair<sym::I, sym::I>> current_map;
                sym::I current_size;

                MockMemoryManager(): current_size(0) {};

                void set_entry(size_t id, SymInt offset, SymInt size) {
                    abstract_map[id] = {offset, size};
                }

                float *get(size_t id) {
                    return (float *) (((uint8_t *) memory) + current_map[id].first);
                }

                void set_memory(void * memory){
                    this->memory = memory;
                }

                void calculate_exact_map(std::unordered_map<sym::I, sym::C> const & provided){
                    current_size = 0;
                    for(auto i=abstract_map.begin(); i != abstract_map.end(); ++i){
                            current_map[i->first] = {i->second.first.eval(provided), i->second.second.eval(provided)};
                        current_size += current_map[i->first].second;
                    }
                }
            };

            class MockBackend: public AbstractBackend, public std::enable_shared_from_this<MockBackend> {
            private:
                /** All of the parameters */
                std::unordered_map<std::string, std::shared_ptr<MockStorage>> params;
                /** Flag of whether we have allocated memory */
                bool allocated;
                /** The pointer to the memory buffer */
                void * memory;
                /** Current size of the memory buffer in bytes */
                uint64_t current_size;

                void generate_sources(GraphFunction const & gf) const;

                void compile(GraphFunction const & gf) const;

                void link(GraphFunction const & gf) const;
            public:
                MockBackend(bool debug) :
                        AbstractBackend("Mock", debug), allocated(false), current_size(0) {};

                MockBackend(std::string work_dir, bool debug) :
                        AbstractBackend("Mock", work_dir, debug), allocated(false), current_size(0){};

                ~MockBackend() { release(); };

                std::shared_ptr<MockFunction> make_function(GraphFunction const & gf);

                std::shared_ptr<MockStorage> get_param(std::string full_name) const;

                std::shared_ptr<MockStorage> initialize_param(std::string full_name,
                                                              DataType data_type,
                                                              std::array<long, 4> shape);

                void * request(uint64_t size);

                void release();
            };

            class MockFunction {
            private:
                /** Shared pointer to the backend */
                std::shared_ptr<MockBackend> const backend;
                /** The graph function */
                GraphFunction const gf;
                /** Shared pointer to the dll function */
                std::shared_ptr<const symbol> const function;
                /** Shared pointer to the manager */
                std::shared_ptr<MockMemoryManager> manager;
                /** Whether the parameters have been initialized */
                bool initialized;
                /** Vector of constant expressions */
                VarVec constants;
                /** Vector of parameters */
                VarVec params;
                /** Deduced symbolic integers from parameters */
                std::vector<std::pair<SymInt, sym::C>> params_shapes;
                /** Last invocation shapes */
                std::vector<std::array<long, 4>> last_shapes;
                /** Last deduced symbolics including parameter deductions */
                std::unordered_map<sym::I, sym::C> last_deduced;
            public:
                MockFunction(std::shared_ptr<MockBackend> const backend,
                             GraphFunction const gf,
                             std::shared_ptr<const symbol> const function,
                             std::shared_ptr<MockMemoryManager> const manager) :
                        backend(backend), gf(gf), function(function), manager(manager), initialized(false) {};

                /** Initializes all the state of the function */
                void initialize(std::vector<std::pair<SymInt, sym::C>> const & provided = {});
                /** Evaluates the function for the inputs provided */
                Outputs eval(std::vector<std::shared_ptr<MockStorage>> & inputs);
            };


//            class MockBackend : public AbstractFunctionBackend<Var, Var, Var> {
//            private:
//                filesystem::path dll_path;
//
//                typedef Outputs (*func_ptr)(VarVec &inputs, VarVec &constants, VarVec &shared);
//
//                typedef dll_symbol<Outputs(VarVec &inputs,
//                                           VarVec &constants,
//                                           VarVec &shared,
//                                           std::shared_ptr<MemoryManager> manager)> symbol;
//                std::shared_ptr<symbol> eval_func;
//            public:
//                std::shared_ptr<MockMemoryManager> manager;
//                VarVec constants;
//                VarVec params;
//
//                MockBackend(GraphFunction const gf, std::string work_dir, bool debug) :
//                        AbstractFunctionBackend("Mock", gf, work_dir, debug),
//                        manager(std::make_shared<MockMemoryManager>()) {};
//
//                MockBackend(GraphFunction const gf, bool debug) :
//                        AbstractFunctionBackend("Mock", gf, debug),
//                        manager(std::make_shared<MockMemoryManager>()) {};
//
//                Outputs operator()(VarVec &inputs) {
//                    if (eval_func) {
//                        return eval_func->f(inputs, constants, params, manager);
//                    } else {
//                        throw 1;
//                    }
//                };
//
//                void initialize();
//
//
//            };
        }
    }
}
#endif //METADIFF_GRAPH_IR_BACKEND_MOCK_H
