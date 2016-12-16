//
// Created by alex on 05/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_H
#define METADIFF_GRAPH_IR_BACKEND_H

namespace md {
    namespace backend {
        std::function<std::string(std::string)> const print_str = [](std::string id) {return id;};

        template<typename T>
        struct dll_symbol {
        protected:
            std::shared_ptr<void> handle;
        public:
            T const *f;

            dll_symbol(const boost::filesystem::path dl_path, int flag, std::string name) {
                char *error_msg;
                auto h = dlopen(dl_path.c_str(), flag);
                if (!h) {
                    std::string const msg = "Error when trying to load dynamic library " + name;
                    md::gir::logger("dll_symbol")->error(msg);
                    throw std::runtime_error(std::move(msg));
                }
                // set handle to cleanup the dlopen:
                handle = std::shared_ptr<void>(h, [](void *handle) {
                    int r = dlclose(handle);
                    assert(r == 0);
                });
                *(void **) (&f) = dlsym(handle.get(), name.c_str());
            }

            dll_symbol(const std::string name, std::shared_ptr<void> const handle) :
                    handle(std::move(handle)) {
                *(void **) (&f) = dlsym(handle.get(), name.c_str());
            }
        };

        template<typename T>
        T get_dll_symbol(void *dll_handle, std::string symbol_name) {
            auto handle = (T) dlsym(dll_handle, symbol_name.c_str());
            char *error_msg;
            if ((error_msg = dlerror()) != NULL) {
                std::string const msg = fmt::format("Error finding in the DLL the symbol {}, reason: {}", symbol_name, error_msg);
                md::gir::logger("DLL")->error(msg);
                throw std::runtime_error(std::move(msg));
            }
            return handle;
        }

        inline void *load_dll(std::string path) {
            char *error_msg;
            auto dll_handle = dlopen((path).c_str(), RTLD_LAZY);
            if (!dll_handle) {
                std::string const msg = "Error when trying to load DLL " + path;
                md::gir::logger("DLL")->error(msg);
                throw std::runtime_error(std::move(msg));
            }
            return dll_handle;
        }

        class AbstractMemoryManager {
        public:
            virtual void *get(size_t id)  = 0;
        };

        typedef std::shared_ptr<AbstractMemoryManager> MemoryManager;

        class AbstractBackend {
        public:
            /** Name of the backend */
            std::string const name;

            /** Path to working directory */
            boost::filesystem::path work_dir;

            /** Path to the source directory */
            boost::filesystem::path source_dir;

            /** Path to the lib directory */
            boost::filesystem::path lib_dir;

            /** Path to the logging directory */
            boost::filesystem::path log_dir;

            /** Boolean flag to debug the compilation */
            bool debug;

            /** Constructs the backend with the default working directory */
            AbstractBackend(std::string const name, bool debug) :
                    name(name), work_dir(gir::default_properties()->default_work_dir),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            /** Constructs the backend with specified working directory */
            AbstractBackend(std::string const name, std::string work_dir_path, bool debug) :
                    name(name), work_dir(work_dir_path),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            virtual ~AbstractBackend() {};
        };
    }
}

#endif //METADIFF_GRAPH_IR_BACKEND_H
