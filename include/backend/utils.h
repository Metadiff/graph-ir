//
// Created by alex on 07/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_UTILS_H
#define METADIFF_GRAPH_IR_BACKEND_UTILS_H

namespace md{
    namespace backend{
        template<typename T>
        T get_dll_symbol(void * dll_handle, std::string symbol_name){
            auto handle = (T) dlsym(dll_handle, symbol_name.c_str());
            char * error_msg;
            if ((error_msg = dlerror()) != NULL) {
                md::gir::logger("DLL")->error("Error finding in the DLL the symbol {}, reason: {}",
                                              symbol_name, error_msg);
                auto ptr  = (int (*)(void))dlsym(dll_handle, "eval");
                std::cout << ptr() << std::endl;
                throw 1;
            }
            return handle;
        }

        inline void * load_dll(std::string path){
            char *error_msg;
            auto dll_handle = dlopen((path).c_str(), RTLD_LAZY);
            if (!dll_handle) {
                md::gir::logger("DLL")->error("Error when trying to load DLL {}", path);
                throw 2;
            }

            return dll_handle;
        }
    }
}
#endif //METADIFF_GRAPH_IR_BACKEND_UTILS_H
