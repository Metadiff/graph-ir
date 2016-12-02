//
// Created by alex on 07/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H
#define METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H

namespace md{
    namespace backend{

        template<typename T>
        struct dll_symbol {
        protected:
            std::shared_ptr<void> handle;
        public:
            T const * f;
            dll_symbol(const boost::filesystem::path dl_path, int flag, std::string name){
                char *error_msg;
                auto h = dlopen(dl_path.c_str(), flag);
                if (!h) {
                    md::gir::logger("dll_symbol")->error("Error when trying to load dynamic library {}", name);
                    throw 2;
                }
                // set handle to cleanup the dlopen:
                handle=std::shared_ptr<void>(h, [](void* handle){
                    int r = dlclose(handle);
                    assert(r==0);
                });
                *(void **)(&f) = dlsym( handle.get(), name.c_str());
            }

            dll_symbol( const std::string name, std::shared_ptr<void> const handle ) :
                    handle( std::move( handle ) ) {
                *(void **)(&f) = dlsym( handle.get(), name.c_str());
            }
        };

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

        class AbstractMemoryManager {
        public:
            virtual float *get(size_t id)  = 0;
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
            AbstractBackend(std::string const name, bool debug):
                    name(name), work_dir(gir::default_properties()->default_work_dir),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            /** Constructs the backend with specified working directory */
            AbstractBackend(std::string const name, std::string work_dir_path, bool debug):
                    name(name), work_dir(work_dir_path),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            virtual ~AbstractBackend() {};
        };


//        /** An interface for all backends to follow */
//        template<typename T, typename C, typename P, typename M>
//        class AbstractBackend: public std::enable_shared_from_this<AbstractBackend<T, C, P, M>> {
//        public:
//            /** Name of the backend */
//            std::string const name;
//
//            /** Path to working directory */
//            filesystem::path work_dir;
//
//            /** Path to the source directory */
//            filesystem::path source_dir;
//
//            /** Path to the lib directory */
//            filesystem::path lib_dir;
//
//            /** Path to the logging directory */
//            filesystem::path log_dir;
//
//            /** Boolean flag to debug the compilation */
//            bool debug;
//
//            /** Constructs the backend with the default working directory */
//            AbstractBackend(std::string const name, bool debug):
//                    name(name), work_dir(gir::default_properties()->default_work_dir),
//                    source_dir(work_dir / "src"),
//                    lib_dir(work_dir / "lib"),
//                    log_dir(work_dir / "log"),
//                    debug(debug) {};
//
//            /** Constructs the backend with specified working directory */
//            AbstractBackend(std::string const name, std::string work_dir_path, bool debug):
//                    name(name), work_dir(work_dir_path),
//                    source_dir(work_dir / "src"),
//                    lib_dir(work_dir / "lib"),
//                    log_dir(work_dir / "log"),
//                    debug(debug) {};
//
//            virtual ~AbstractBackend() {};
//
//            /** Creates a function from this backend based on the provided Graph Function */
//            virtual std::shared_ptr<AbstractFunction<T, C, P, M>> make_function(GraphFunction const & gf) = 0;
//
//            /** Requests a pointer to the memory where at least size bytes are valid */
//            virtual void * request(uint64_t size) = 0;
//
//            /** Releases any allocate memory (note this does not include the Parameters) */
//            virtual void release() = 0;
//
//            /** Returns the parameter residing under that name */
//            virtual std::shared_ptr<P> get_param(std::string full_name) = 0;
//
//            /** Returns the parameter residing under that name */
//            virtual std::shared_ptr<P> initialize_param(std::string full_name,
//                                                        DataType data_type,
//                                                        std::array<long, 4> shape) = 0;
//        };
//
//        template<typename T, typename C, typename P, typename M>
//        class AbstractFunction {
//        public:
//            typedef std::pair<std::vector<std::shared_ptr<T>>, std::vector<std::shared_ptr<T>>> Outputs;
//            typedef dll_symbol<Outputs(std::vector<std::shared_ptr<T>> &inputs,
//                                       std::vector<std::shared_ptr<C>> &constants,
//                                       std::vector<std::shared_ptr<P>> &params,
//                                       std::shared_ptr<M> manager)> symbol;
//            Backend<T, C, P, M> const backend;
//            GraphFunction const gf;
//            std::shared_ptr<symbol> const function;
//
//            AbstractFunction(Backend<T, C, P, M> const backend, GraphFunction const gf,
//                             std::shared_ptr<symbol> const function):
//                    backend(backend), gf(gf), function(function) {};
//
//            /** This should be used to execute the function after it has been compiled */
//            virtual Outputs eval(std::vector<std::shared_ptr<T>> & inputs) = 0;
//        };
    }
}

#endif //METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H
