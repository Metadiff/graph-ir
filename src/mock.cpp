//
// Created by alex on 07/11/16.
//

#include "graph_ir.h"

namespace md{
    namespace backend{
        namespace mock {
            typedef std::chrono::high_resolution_clock timer;
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

            void AbstractMockFunction::initialize(std::vector<std::pair<SymInt, int64_t>> const & provided) {
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
                            function_logger(gf.name)->error(msg);
                            throw std::invalid_argument(msg);
                        }
                    }
                    params.push_back(backend->initialize_param(cast_op->full_name, cast_op->data_type, shape));
                }
                implicit = provided;
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

            std::shared_ptr<AbstractMockFunction> MockBackend::make_source_gen_function(GraphFunction const &gf){
                generate_sources(gf);
                compile(gf);
                auto dll_path = lib_dir / gf.name / "main.so";
                auto func_ptr = std::make_shared<symbol>(dll_path, RTLD_LAZY, "eval");
                // Build the memory manager
                auto manager = std::make_shared<MockMemoryManager>();
                SymInt bytes, offset = 0;
                for(auto i=0; i<gf.graph->nodes.size(); ++i){
                    auto node = gf.graph->nodes[i];
                    if(node->op->name != "Input" and node->op->name != "Parameter" and node->op->name != "Monitor"){
                        auto is_out = std::find_if(gf.outputs.begin(),
                                                   gf.outputs.end(),
                                                   [=](Node n){return n->id == node->id;});
                        if(is_out == gf.outputs.end()){
                            bytes = number_of_elements(node->shape) * byte_size(node->data_type);
                            manager->set_entry(node->id, offset, bytes);
                            offset = offset + bytes;
                        }
                    }
                }
                return std::make_shared<MockSourceGenFunction>(shared_from_this(), gf, func_ptr, manager);
            };

            std::shared_ptr<AbstractMockFunction> MockBackend::make_in_memory_function(GraphFunction const &gf){
                return make_source_gen_function(gf);
            }

            typedef std::function<std::string(std::vector<std::string> &)> RepFunc;
            typedef std::unordered_map<size_t, RepFunc> RepMap;
            void write_api(std::ostream &f);
            RepFunc build_rep(Node node, std::string name = "");
            void write_api(std::ostream &f);
            void write_op(Operator op, RepMap &repmap, std::ostream &f);
            std::string to_code(DataType data_type);


            void MockBackend::compile(GraphFunction const & gf) const {
                auto source_dir = this->source_dir / gf.name;
                auto lib_dir = this->lib_dir / gf.name;
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
                    backend_logger(name)->error("Bad compilation response {} - {}", response, err_msg);
                    throw std::runtime_error(std::move(err_msg));
                }
            }

            void MockBackend::generate_sources(GraphFunction const & gf) const {
                auto source_dir = this->source_dir / gf.name;
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
                    f << "\tauto node_" << gf.inputs[i]->id << " = inputs[" << i << "]->get<"
                      << to_code(gf.inputs[i]->data_type) << ">();" << std::endl;
                    repmap.insert({gf.inputs[i]->id, build_rep(gf.inputs[i])});
                }
                // Generate parameters representation
                f << "\t// Generating parameter expressions" << std::endl;
                auto params = gf.graph->op_map["Parameter"];
                for (auto i = 0; i < params.size(); ++i) {
                    f << "\tauto node_" << params[i]->id << " = params[" << i << "]->get<"
                      << to_code(params[i]->data_type) << ">();" << std::endl;
                    repmap.insert({params[i]->id, build_rep(params[i])});
                }
                // Generate outputs
                f << "\t// Generating output expressions" << std::endl;
                f << "\tVarVec outputs;\n";
                for (auto i = 0; i < gf.outputs.size(); ++i) {
                    f << "\toutputs.push_back(make_var(" << gf.outputs[i]->data_type << ", std::array<long, 4> {"
                      << sym::to_code(gf.outputs[i]->shape[0]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[1]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[2]) << ", "
                      << sym::to_code(gf.outputs[i]->shape[3]) << "}));\n";
                    f << "\tauto node_" << gf.outputs[i]->id << " = outputs[" << i << "]->get<"
                      << to_code(params[i]->data_type) << ">();" << std::endl;
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
                    f << "\tauto node_" << monitors[i]->id << " = monitors[" << i << "]->get<"
                      << to_code(monitors[i]->data_type) << ">();" << std::endl;
                    repmap.insert({monitors[i]->id, build_rep(monitors[i])});
                }
                // Generate all nodes
                for (auto i = 0; i < gf.graph->nodes.size(); ++i) {
                    auto node = gf.graph->nodes[i];
                    if (node->op->name != "Input" and node->op->name != "Parameter") {
                        auto is_out = std::find_if(gf.outputs.begin(),
                                                   gf.outputs.end(),
                                                   [=](Node n){return n->id == node->id;});
                        if(is_out == gf.outputs.end()) {
                            f << "\tauto node_" << node->id << " = static_cast<" << to_code(gf.graph->nodes[i]->data_type)
                              << "*>(manager->get(" << node->id << "));" << std::endl;
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
                if(op->name == "Input" or op->name == "Parameter"){
                    return;
                }
                // Write loop start
                f << tabs << "// " << result << std::endl;
//                f << tabs << "std::cout << " << result->id << " << std::endl;" << std::endl;
                for (auto i = 0; i < result.order(); ++i) {
                    f << tabs << "for(auto it" << i << "=0;"
                      << " it" << i << "<(" << sym::to_code(result->shape[i]) << ");"
                      << " ++it" << i << "){" << std::endl;
                    tabs += "\t";
                }
                if (op->name == "Add") {
                    auto cast_op = std::dynamic_pointer_cast<op::Add>(op);
                    auto parents = cast_op->get_parents();
                    f << tabs << repmap[result->id](iters) << " = (0";
                    for (auto i = 0; i < parents.size(); ++i) {
                        if (not cast_op->neg[i]) {
                            f << " + " << repmap[parents[i]->id](iters);
                        }
                    }
                    f << ") - (0";
                    for (auto i = 0; i < parents.size(); ++i) {
                        if (cast_op->neg[i]) {
                            f << " + " << repmap[parents[i]->id](iters);
                        }
                    }
                    f << ");" << std::endl;
//                    f << "std::cout << " << repmap[result->id](iters) << " << std::endl;" << std::endl;
                } else if (op->name == "Mul"){
                    auto cast_op = std::dynamic_pointer_cast<op::Mul>(op);
                    auto parents = cast_op->get_parents();
                    f << tabs << repmap[result->id](iters) << " = (1";
                    for (auto i = 0; i < parents.size(); ++i) {
                        if (not cast_op->div[i]) {
                            f << " * " << repmap[parents[i]->id](iters);
                        }
                    }
                    f << ") / (1";
                    for (auto i = 0; i < parents.size(); ++i) {
                        if (cast_op->div[i]) {
                            f << " * " << repmap[parents[i]->id](iters);
                        }
                    }
                    f << ");" << std::endl;
//                    f << "std::cout << " << repmap[result->id](iters) << " << std::endl;" << std::endl;
                }
                // Write loop end
                for (auto i = 0; i < result.order(); ++i) {
                    tabs = tabs.substr(0, tabs.length() - 1);
                    f << tabs << "}" << std::endl;
                }
            }
            std::string to_code(DataType data_type){
                switch (data_type.type) {
                    case BOOLEAN: return "bool";
                    case UNSIGNED_INT: {
                        switch (data_type.precision){
                            case p8: return "uint8_t";
                            case p16: return "uint16_t";
                            case p32: return "uint32_t";
                            default: return "uint64_t"; // p64
                        }
                    }
                    case SIGNED_INT :{
                        switch (data_type.precision){
                            case p8: return "int8_t";
                            case p16: return "int16_t";
                            case p32: return "int32_t";
                            default: return "int64_t"; // p64
                        }
                    }
                    case FLOAT : {
                        switch (data_type.precision){
                            case p8: throw std::runtime_error("FLOAT 8 bit not supported");
                            case p16: throw std::runtime_error("FLOAT 16 bit not supported");
                            case p32: return "float";
                            default: return "double"; // p64
                        }
                    }
                    default: { // COMPLEX
                        switch (data_type.precision){
                            case p8: throw std::runtime_error("COMPLEX 8 bit not supported");
                            case p16: throw std::runtime_error("COMPLEX 16 bit not supported");
                            case p32: return "std::complex<float>";
                            default: return "std::complex<double>"; // p64
                        }
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
                f << "// what?" << std::endl;
                f << "// Disclaimer: Do not edit unless you know what you are doing\n"
                        "// Auto generated by Metadiff - MockBackend\n\n"
                        "// Includes\n"
                        "#include <utility>\n"
                        "#include <array>\n"
                        "#include <complex>\n"
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
                        "\t\treturn static_cast<T*>(data);\n"
                        "\t}\n"
                        "};\n\n"
                        "inline std::shared_ptr<MockStorage> make_var(DataType data_type, std::array<long, 4> shape) {\n"
                        "\treturn std::make_shared<MockStorage>(data_type, shape);\n"
                        "}\n\n"
                        "class AbstractMemoryManager {\n"
                        "public:\n"
                        "\tvirtual void * get(size_t id)  = 0;\n"
                        "};\n\n"
                        "typedef std::vector<std::shared_ptr<MockStorage>> VarVec;\n"
                        "// ***** END API DEFINITIONS *****\n\n"
                        "// Main function\n"
                        "extern \"C\" std::pair<VarVec, VarVec> eval(VarVec & inputs, \n"
                        "\t\t\tVarVec & constants,\n"
                        "\t\t\tVarVec & params,\n"
                        "\t\t\tstd::shared_ptr<AbstractMemoryManager> manager,\n"
                        "\t\t\tstd::unordered_map<std::string, int64_t> & deduced) {\n";
            }
        }
    }
}

