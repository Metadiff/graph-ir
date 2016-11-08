//
// Created by alex on 07/11/16.
//

#include "graph_ir.h"

namespace md{
    namespace backend{
        using namespace filesystem;

        void write_api(std::ostream & f);

        void MockBackend::link(){
            load_dll()
            eval_func = std::make_shared<symbol>(dll_path, RTLD_LAZY, "eval");
        }

        void MockBackend::compile() {
            if(not exists(lib_dir)){
                create_directories(lib_dir);
            }
            path source_path = source_dir / "main.cpp";
            path dll_path = lib_dir / "main.so";
            path log_path = lib_dir / "compile.log";
            std::string command = "g++ -O3 -Wall -shared -fPIC -std=c++11 ";
            command += "-Werror=return-type -Wno-unused-variable -Wno-narrowing ";
            command += " -o " + dll_path.string() + " " + source_path.string();
            command += " > " + (lib_dir/ "compile.log").string() + " 2>&1";
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
            this->dll_path = dll_path;
        }

        void MockBackend::generate_sources() {
            if(not exists(source_dir)){
                create_directories(source_dir);
            }
            path source_path = source_dir / "main.cpp";
            backend_logger(name)->debug("Generating source file {}", source_path.string());
            std::ofstream f;
            f.open(source_path.string());
            write_api(f);
            f << "    VarVec outputs, monitors;" << std::endl;
            f << "    return {outputs, monitors};" << std::endl
              << "};" << std::endl << std::endl;
            f.close();
            backend_logger(name)->debug("Generating source file completed");
        }

        void write_api(std::ostream& f){
            f <<    "// Disclaimer: Do not edit unless you know what you are doing\n"
                    "// Auto generated by Metadiff - MockBackend\n\n"
                    "// Includes\n"
                    "#include <utility>\n"
                    "#include <array>\n"
                    "#include <vector>\n\n"
                    "// ***** API DEFINITIONS *****\n"
                    "/** Data type of the storage */\n"
                    "        enum DataType{\n"
                    "            /** 8 bit boolean */\n"
                    "                    b8 = 0,\n"
                    "            /** 8 bit unsigned integer */\n"
                    "                    u8 = 1,\n"
                    "            /** 16 bit unsigned integer */\n"
                    "                    u16 = 2,\n"
                    "            /** 32 bit unsigned integer */\n"
                    "                    u32 = 3,\n"
                    "            /** 64 bit unsigned integer */\n"
                    "                    u64 = 4,\n"
                    "            /** 8 bit signed integer */\n"
                    "                    i8 = 5,\n"
                    "            /** 16 bit signed integer */\n"
                    "                    i16 = 6,\n"
                    "            /** 32 bit signed integer */\n"
                    "                    i32 = 7,\n"
                    "            /** 64 bit signed integer */\n"
                    "                    i64 = 8,\n"
                    "            /** 8 bit floating point */\n"
                    "                    f8 = 9,\n"
                    "            /** 16 bit floating point */\n"
                    "                    f16 = 10,\n"
                    "            /** 32 bit floating point */\n"
                    "                    f32 = 11,\n"
                    "            /** 64 bit floating point */\n"
                    "                    f64 = 12\n"
                    "        };\n\n"
                    "class MockVariable{\n"
                    "            public:\n"
                    "                DataType const data_type;\n"
                    "                std::array<long, 4> const shape;\n"
                    "                bool const managed;\n"
                    "                void * data;\n"
                    "                MockVariable(DataType data_type, std::array<long, 4> shape):\n"
                    "                        data_type(data_type), shape(shape), managed(false){\n"
                    "                    int bytes;\n"
                    "                    if(data_type == b8){\n"
                    "                        bytes = 1;\n"
                    "                    } else {\n"
                    "                        bytes = (data_type % 4);\n"
                    "                        bytes = bytes == 0 ? 4 : bytes;\n"
                    "                    }\n"
                    "                    data = std::malloc(total_size() * bytes);\n"
                    "                }\n"
                    "\n"
                    "                MockVariable(DataType data_type, std::array<long, 4> shape, void * data):\n"
                    "                        data_type(data_type), shape(shape), managed(true), data(data){}\n"
                    "\n"
                    "                ~MockVariable(){\n"
                    "                    if(not managed) {\n"
                    "                        free(data);\n"
                    "                    }\n"
                    "                }\n"
                    "\n"
                    "                long total_size(){\n"
                    "                    return shape[0] * shape[1] * shape[2] * shape[3];\n"
                    "                }\n"
                    "                \n"
                    "                template <typename T>\n"
                    "                T* get(){\n"
                    "                    return (T*) data;\n"
                    "                }\n"
                    "            };\n\n"
                    "typedef std::vector<MockVariable> VarVec;\n"
                    "// ***** END API DEFINITIONS *****\n\n"
                    "// Main function\n"
                    "extern \"C\" std::pair<VarVec, VarVec> eval(VarVec & inputs, VarVec & constants, VarVec & shared) {\n";
        }
    }
}

