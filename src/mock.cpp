//
// Created by alex on 07/11/16.
//

#include "graph_ir.h"

namespace md{
    namespace backend{
        namespace mock {
            Outputs MockFunction::eval(VarVec & inputs) {
                if(inputs.size() != gf.inputs.size()){
                    std::string const msg = fmt::format("Incorrect number of inputs. Expected {}, but got {}.",
                                                        gf.inputs.size(), inputs.size());
                    function_logger(gf.graph->name)->error("{}", msg);
                    throw std::invalid_argument(std::move(msg));
                }
                // Check if the shapes have changed since the last call to the function
                bool changed = false;
                if(last_shapes.size() == 0){
                    changed = true;
                } else {
                    for(auto i = 0; i < inputs.size(); ++i){
                        if(last_shapes[i] != inputs[i]->shape){
                            changed = true;
                            break;
                        }
                    }
                }
                std::vector<std::pair<SymInt, sym::C>> implicit_shapes;
                if(changed) {
                    // If they have make variable deduction
                    implicit_shapes = params_shapes;
                    for (auto i = 0; i < inputs.size(); ++i) {
                        for (auto j = 0; j < 4; ++j) {
                            if (not gf.inputs[i]->shape[j].is_constant()) {
                                implicit_shapes.push_back({gf.inputs[i]->shape[j], inputs[i]->shape[j]});
                            } else if (gf.inputs[i]->shape[j].eval() != inputs[i]->shape[j]) {
                                std::string const msg = fmt::format("Incorrect shape of input at index {}. "
                                                                            "The shape on dimension {} does not match."
                                                                            "Expected {}, but got {}.",
                                                                    i, j, gf.inputs[i]->shape[j].eval(),
                                                                    inputs[i]->shape[j]);
                                function_logger(gf.graph->name)->error("{}", msg);
                                throw std::invalid_argument(std::move(msg));
                            }
                        }
                    }
                    if (implicit_shapes.size() > 0) {
                        auto deduced_shapes = sym::registry()->deduce_values(implicit_shapes);
                        fmt::MemoryWriter w;
                        w.write("Deduced symbolics: ");
                        for (auto i = deduced_shapes.begin(); i != deduced_shapes.end(); ++i) {
                            w.write("{}={}, ", ((char)(i->first + 'a')), i->second);
                        }
                        function_logger(gf.graph->name)->debug(std::move(std::string(w.c_str())));
                        last_deduced = deduced_shapes;
                    }
                    // Finally make the last shapes represent the current input shapes
                    last_shapes.clear();
                    for(auto i = 0; i < inputs.size(); ++i){
                        last_shapes.push_back(inputs[i]->shape);
                    }
                    // Update memory manager
                    manager->calculate_exact_map(last_deduced);
                } else {
                    function_logger(gf.graph->name)->debug("No change in the input dimensions.");
                }
                manager->set_memory(backend->request(manager->current_size));
                return function->f(inputs, constants, params, manager, last_deduced);
            }

            void MockFunction::initialize(std::vector<std::pair<SymInt, sym::C>> const & provided) {
                auto deduced = sym::registry()->deduce_values(provided);
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
                            function_logger(gf.graph->name)->error(msg);
                            throw std::invalid_argument(msg);
                        }
                    }
                    params.push_back(backend->initialize_param(cast_op->full_name, cast_op->data_type, shape));
                }
                params_shapes = provided;
                initialized = true;
            }

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
                    backend_logger(name)->debug("Trying to repeat initialization of parameter {}", full_name);
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

            std::shared_ptr<MockFunction> MockBackend::make_function(GraphFunction const & gf) {
                generate_sources(gf);
                compile(gf);
                auto dll_path = lib_dir / gf.graph->name / "main.so";
                auto func_ptr = std::make_shared<symbol>(dll_path, RTLD_LAZY, "eval");
                // Build the memory manager
                auto manager = std::make_shared<MockMemoryManager>();
                SymInt offset = 0;
                for(auto i=0; i<gf.graph->nodes.size(); ++i){
                    auto node = gf.graph->nodes[i];
                    if(node->op->name != "Input" and node->op->name != "Parameter" and node->op->name != "Monitor"){
                        auto is_out = std::find_if(gf.outputs.begin(),
                                                   gf.outputs.end(),
                                                   [=](Node n){return n->id == node->id;});
                        if(is_out == gf.outputs.end()){
                            // TODO this should be update according to the byte size of the data type
                            manager->set_entry(node->id, offset, number_of_elements(node->shape) * 4);
                            offset = offset + number_of_elements(node->shape);
                        }
                    }
                }
                return std::make_shared<MockFunction>(shared_from_this(), gf, func_ptr, manager);
            };

            typedef std::function<std::string(std::vector<std::string> &)> RepFunc;
            typedef std::unordered_map<size_t, RepFunc> RepMap;
            void write_api(std::ostream &f);
            RepFunc build_rep(Node node, std::string name = "");
            void write_api(std::ostream &f);
            void write_op(Operator op, RepMap &repmap, std::ostream &f);


            void MockBackend::compile(GraphFunction const & gf) const {
                auto source_dir = this->source_dir / gf.graph->name;
                auto lib_dir = this->lib_dir / gf.graph->name;
                if (not exists(lib_dir)) {
                    create_directories(lib_dir);
                }
                auto source_path = source_dir / "main.cpp";
                auto dll_path = lib_dir / "main.so";
                auto log_path = lib_dir / "compile.log";
                std::string command = "g++ -O3 -Wall -shared -fPIC -std=c++11 ";
                command += "-Werror=return-type -Wno-unused-variable -Wno-narrowing ";
                command += " -o " + dll_path.string() + " " + source_path.string();
                command += " > " + (lib_dir / "compile.log").string() + " 2>&1";
                backend_logger(name)->debug("Compiling file {} to shared library {}",
                                            source_path.string(), dll_path.string());
                backend_logger(name)->debug("Compilation command: {}", command);
                int response = std::system(command.c_str());
                if (response != 0) {
                    std::ifstream log_file(log_path.string());
                    std::string err_msg((std::istreambuf_iterator<char>(log_file)),
                                        std::istreambuf_iterator<char>());
                    backend_logger(name)->debug("Bad compilation response {} - {}", response, err_msg);
                    throw 3;
                }
            }

            void MockBackend::generate_sources(GraphFunction const & gf) const {
                auto source_dir = this->source_dir / gf.graph->name;
                if (not exists(source_dir)) {
                    create_directories(source_dir);
                }
                auto source_path = source_dir / "main.cpp";
                backend_logger(name)->debug("Generating source file {}", source_path.string());
                std::ofstream f;
                f.open(source_path.string());
                write_api(f);
                RepMap repmap;
                // Generate symbolic expressions
                f << "\t// Generating symbolic expressions" << std::endl;
                for(auto i = gf.unique_symbolics.begin(); i != gf.unique_symbolics.end(); ++i){
                    f << "\tint64_t " << ((char)((*i) + 'a')) << " = deduced[" << (*i) << "];" << std::endl;
                }
                // Generate inputs representation
                f << "\t// Generating input expressions" << std::endl;
                for (auto i = 0; i < gf.inputs.size(); ++i) {
                    f << "\tauto node_" << gf.inputs[i]->id << " = inputs[" << i << "]->get<float>();" << std::endl;
                    repmap.insert({gf.inputs[i]->id, build_rep(gf.inputs[i])});
                }
                // Generate parameters representation
                f << "\t// Generating parameter expressions" << std::endl;
                auto params = gf.graph->op_map["Parameter"];
                for (auto i = 0; i < params.size(); ++i) {
                    f << "\tauto node_" << params[i]->id << " = params[" << i << "]->get<float>();" << std::endl;
                    repmap.insert({params[i]->id, build_rep(params[i])});
                }
                // Generate outputs
                f << "\t// Generating output expressions" << std::endl;
                f << "\tVarVec outputs;\n";
                for (auto i = 0; i < gf.outputs.size(); ++i) {
                    f << "\toutputs.push_back(std::make_shared<MockStorage>(f32, std::array<long, 4> {"
                      << sym::to_code(gf.outputs[i]->shape[0]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[1]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[2]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[3]) << "}));\n";
                    f << "\tauto node_" << gf.outputs[i]->id << " = outputs[" << i << "]->get<float>();" << std::endl;
                    repmap.insert({gf.outputs[i]->id, build_rep(gf.outputs[i])});
                }
                // Generate monitors
                f << "\t// Generating monitor expressions" << std::endl;
                auto m = gf.graph->op_map.find("Monitor");
                auto monitors = (m == gf.graph->op_map.end()) ? NodeVec{} : m->second;
                f << "\tVarVec monitors;\n";
                for (auto i = 0; i < monitors.size(); ++i) {
                    f << "\toutputs.push_back(std::make_shared<MockStorage>(f32, std::array<long, 4> {"
                      << sym::to_code(gf.outputs[i]->shape[0]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[1]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[2]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[3]) << "}));\n";
                    f << "\tauto node_" << monitors[i]->id << " = monitors[" << i << "]->get<float>();" << std::endl;
                    repmap.insert({monitors[i]->id, build_rep(monitors[i])});
                }
                // Generate all nodes
                for (auto i = 0; i < gf.graph->nodes.size(); ++i) {
//                    f << "\tstd::cout << " << i << " << std::endl;" << std::endl;
                    auto node = gf.graph->nodes[i];
                    if (node->op->name != "Input" and node->op->name != "Parameter") {
                        auto is_out = std::find_if(gf.outputs.begin(),
                                                   gf.outputs.end(),
                                                   [=](Node n){return n->id == node->id;});
                        if(is_out == gf.outputs.end()) {
                            f << "\tauto node_" << node->id << " = manager->get(" << node->id << ");" << std::endl;
                            repmap.insert({node->id, build_rep(node)});
                        }
                        write_op(gf.graph->nodes[i]->op, repmap, f);
                    }
                }
                f << "\treturn {outputs, monitors};" << std::endl << "};" << std::endl << std::endl;
                f.close();
                backend_logger(name)->debug("Generating {} completed.", source_path.string());
            }

            void write_op(Operator op, RepMap &repmap, std::ostream &f) {
                auto result = op->result;
                std::string tabs = "\t";
                std::vector<std::string> iters{"it0", "it1", "it2", "it3"};
                if (op->name == "Add") {
                    f << tabs << "// " << result << std::endl;
                    auto cast_op = std::dynamic_pointer_cast<op::Add>(op);
                    auto parents = cast_op->get_parents();
                    for (auto i = 0; i < result.order(); ++i) {
                        f << tabs << "for(auto it" << i << "=0;"
                          << " it" << i << "<(" << result->shape[i] << ");"
                          << " ++it" << i << "){" << std::endl;
                        tabs += "\t";
                    }
                    f << tabs << repmap[result->id](iters) << " =";
                    for (auto i = 0; i < parents.size(); ++i) {
                        if (cast_op->neg[i]) {
                            f << " - ";
                        } else {
                            f << " + ";
                        }
                        f << repmap[parents[i]->id](iters);
                    }
                    f << ";" << std::endl;
                    for (auto i = 0; i < result.order(); ++i) {
                        tabs = tabs.substr(0, tabs.length() - 1);
                        f << tabs << "}" << std::endl;
                    }
                }
            }

            RepFunc build_rep(Node node, std::string name) {
                if (name == "") {
                    name = "node_" + std::to_string(node->id);
                }
                return [=](std::vector<std::string> &it) {
                    std::string rep = name;
                    if (node.order() == 0) {
                        rep += "[0]";
                    } else if (node.order() == 1) {
                        rep += "[" + it[0] + "]";
                    } else if (node.order() == 2) {
                        rep += "[" + it[0] + " + " +
                               it[1] + " * (" +
                               sym::to_code(node->shape[0]) + ")]";
                    } else if (node.order() == 3) {
                        rep += "[" + it[0] + " + " +
                               it[1] + " * (" +
                               sym::to_code(node->shape[0]) + ")" +
                               it[2] + " * (" +
                               sym::to_code(node->shape[0]) + ") * (" +
                               sym::to_code(node->shape[1]) + ")]";
                    } else if (node.order() == 4) {
                        rep += "[" + it[0] + " + " +
                               it[1] + " * (" +
                               sym::to_code(node->shape[0]) + ")" +
                               it[2] + " * (" +
                               sym::to_code(node->shape[0]) + ") * (" +
                               sym::to_code(node->shape[1]) + ")" +
                               it[3] + " * (" +
                               sym::to_code(node->shape[0]) + ") * (" +
                               sym::to_code(node->shape[1]) + ") * (" +
                               sym::to_code(node->shape[2]) + ")]";
                    }
                    return rep;
                };
            }

            void write_api(std::ostream &f) {
                f << "// Disclaimer: Do not edit unless you know what you are doing\n"
                        "// Auto generated by Metadiff - MockBackend\n\n"
                        "// Includes\n"
                        "#include <utility>\n"
                        "#include <array>\n"
                        "#include <vector>\n"
                        "#include <iostream>\n"
                        "#include <unordered_map>\n"
                        "#include <memory>\n\n"
                        "// ***** API DEFINITIONS *****\n"
                        "/** Data type of the storage */\n"
                        "enum DataType{\n"
                        "\t/** 8 bit boolean */\n"
                        "\t\tb8 = 0,\n"
                        "\t/** 8 bit unsigned integer */\n"
                        "\t\tu8 = 1,\n"
                        "\t/** 16 bit unsigned integer */\n"
                        "\t\tu16 = 2,\n"
                        "\t/** 32 bit unsigned integer */\n"
                        "\t\tu32 = 3,\n"
                        "\t/** 64 bit unsigned integer */\n"
                        "\t\tu64 = 4,\n"
                        "\t/** 8 bit signed integer */\n"
                        "\t\ti8 = 5,\n"
                        "\t/** 16 bit signed integer */\n"
                        "\t\ti16 = 6,\n"
                        "\t/** 32 bit signed integer */\n"
                        "\t\ti32 = 7,\n"
                        "\t/** 64 bit signed integer */\n"
                        "\t\ti64 = 8,\n"
                        "\t/** 8 bit floating point */\n"
                        "\t\tf8 = 9,\n"
                        "\t/** 16 bit floating point */\n"
                        "\t\tf16 = 10,\n"
                        "\t/** 32 bit floating point */\n"
                        "\t\tf32 = 11,\n"
                        "\t/** 64 bit floating point */\n"
                        "\t\tf64 = 12\n"
                        "};\n\n"
                        "class MockStorage {\n"
                        "public:\n"
                        "\tDataType const data_type;\n"
                        "\tstd::array<long, 4> const shape;\n"
                        "\tvoid *data;\n\n"
                        "\tMockStorage(DataType data_type, std::array<long, 4> shape) :\n"
                        "\t\tdata_type(data_type), shape(shape) {\n"
                        "\t\tdata = std::malloc(total_size() * 4);\n"
                        "\t}\n\n"
                        "\t~MockStorage() {\n"
                        "\t\tfree(data);\n"
                        "\t}\n\n"
                        "\tlong total_size() {\n"
                        "\t\treturn shape[0] * shape[1] * shape[2] * shape[3];\n"
                        "\t}\n\n"
                        "\ttemplate<typename T>\n"
                        "\tT *get() {\n"
                        "\t\treturn (T *) data;\n"
                        "\t}\n"
                        "};\n\n"
                        "\tinline std::shared_ptr<MockStorage> make_var(DataType data_type, std::array<long, 4> shape) {\n"
                        "\t\treturn std::make_shared<MockStorage>(data_type, shape);\n"
                        "\t}\n\n"
                        "class AbstractMemoryManager {\n"
                        "public:\n"
                        "\tvirtual float *get(size_t id)  = 0;\n"
                        "\tvirtual void set_memory(void * memory) = 0;\n"
                        "};\n\n"
                        "typedef std::vector<std::shared_ptr<MockStorage>> VarVec;\n"
                        "// ***** END API DEFINITIONS *****\n\n"
                        "// Main function\n"
                        "extern \"C\" std::pair<VarVec, VarVec> eval(VarVec & inputs, \n"
                        "\t\t\tVarVec & constants,\n"
                        "\t\t\tVarVec & params,\n"
                        "\t\t\tstd::shared_ptr<AbstractMemoryManager> manager,\n"
                        "\t\t\tstd::unordered_map<uint16_t, int64_t> & deduced) {\n";
            }
        }
    }
}

