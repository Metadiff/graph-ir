//
// Created by alex on 07/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H
#define METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H

namespace md{
    namespace backend{
        using namespace boost;
        /** An interface for all backends to follow */
        template<typename T, typename C, typename S>
        class AbstractFunctionBackend {
        public:
            /** Name of the backend */
            std::string const name;

            /** The GraphFunction to be compiled */
            GraphFunction const gf;

            /** Path to working directory */
            filesystem::path work_dir;

            /** Path to the source dicretory */
            filesystem::path source_dir;

            /** Path to the lib dicretory */
            filesystem::path lib_dir;

            /** Path to the logging directory */
            filesystem::path log_dir;

            /** Boolean flag to debug the compilation */
            bool debug;

            /** Constructs the backedn with the default working directory */
            AbstractFunctionBackend(std::string const name, GraphFunction const gf, bool debug):
                    name(name), gf(gf), work_dir(gir::default_properties()->default_work_dir),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            AbstractFunctionBackend(std::string const name, GraphFunction const gf,
                                    std::string work_dir_path, bool debug):
                    name(name), gf(gf), work_dir(work_dir_path),
                    source_dir(work_dir / "src"),
                    lib_dir(work_dir / "lib"),
                    log_dir(work_dir / "log"),
                    debug(debug) {};

            /** This should be used to execute the function after it has been compiled */
            virtual std::pair<std::vector<T>, std::vector<T>> operator()(std::vector<T> & inputs) = 0;

            /** Any form of initialization required should be carried out here
             * Note that this also intializes all of the Shared variables */
            virtual void initialize() = 0;

            /** Generates the source code needed for the function */
            virtual void generate_sources() = 0;

            /** Compiles the source files which were generated to a dynamic library */
            virtual void compile() = 0;

            /** Links all of the compiled libraries and sets the state of the class such that it is ready to execute */
            virtual void link() = 0;

            /** This should execute all nessacary steps to prepare the function to be ready to execute */
            virtual void complete() = 0;
        };

        template<typename T, typename C, typename S>
        using Backend = std::shared_ptr<AbstractFunctionBackend<T,C,S>>;

        template<typename T>
        struct dll_symbol {
        private:
            std::shared_ptr<void> handle;
        public:
            T const * f;
            dll_symbol(const filesystem::path dl_path, int flag, std::string name){
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

//        struct dlib {
//        private:
//            std::shared_ptr<void> handle;
//        public:
//            template<class T>
//            template<class Sig>
//            std::function<Sig> function(const char* name) const {
//                // shared pointer to a function pointer:
//                auto pf = pfunc(name);
//                if (!pf) return {};
//                return [pf=std::move(pf)](auto&&...args)->decltype(auto){
//                        return (*pf)(decltype(args)(args)...);
//                };
//            }
//            std::shared_ptr<T> pfunc(std::string name) const {
//                if (!handle) return {};
//                void* sym = dlsym(handle.get(), name.c_str());
//                if (!sym) return {};
//                T* ret = 0;
//                // apparently approved hack to convert void* to function pointer
//                // in some silly compilers:
////                *reinterpret_cast<void**>(&ret) = sym;
//                ret = (T*) sym;
//                return {ret, handle};
//            }
//            // returns a smart pointer pointing at a function for name:
//            dlib() = default;
//            dlib(dlib const&)=default;
//            dlib(dlib &&)=default;
//            dlib& operator=(dlib const&)=default;
//            dlib& operator=(dlib &&)=default;
//
//            dlib(const char* name, int flag) {
//                char *error_msg;
//                auto h = dlopen(name, flag);
//                if (!h) {
//                    md::gir::logger("dlib")->error("Error when trying to load dynamic library {}", name);
//                    throw 2;
//                } else {
//                    // set handle to cleanup the dlopen:
//                    handle=std::shared_ptr<void>(h, [](void* handle){
//                        int r = dlclose(handle);
//                        assert(r==0);
//                    });
//                }
//            }
//
//            dlib(std::string name, int flag): dlib(name.c_str(), flag) {};
//            dlib(filesystem::path name, int flag): dlib(name.c_str(), flag) {};
//            explicit operator bool() const { return (bool)handle; }
//        };
    }
}

#endif //METADIFF_GRAPH_IR_BACKEND_ABSTRACT_H
