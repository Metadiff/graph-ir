//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_GRAPH_IR_SHARED_H
#define METADIFF_GRAPH_IR_SHARED_H

namespace md{
    namespace gir{
        /** A class for initializing all shared variables */
        class Initializer{
        public:
            virtual Node initialize(std::array<long long int, 4> const real_shape) = 0;
        };

        typedef std::shared_ptr<Initializer> Init;

        /** A shared variable is a like a static variable, which is synchronized between devices */
        class SharedVariable {
        private:
            /** A pointer to the buffer on the host side */
            void * memory_ptr;
            /** The real shape after initialization */
            std::array<long long int, 4> real_shape;
            /** Indicator if the variable has been initialized */
            bool initialized = false;
        public:
            size_t const id;
            DataType const data_type;
            Shape const shape;
            std::string const name;
//            Init init;
        public:
            SharedVariable(size_t const id, DataType const data_type,
                           Shape const shape, std::string const name):
                    id(id),
                    data_type(data_type),
                    shape(shape),
                    name(name) {};

//            void initialize(Init init = Init());
        };

        typedef std::shared_ptr<SharedVariable> SharedVar;

        inline std::shared_ptr<std::vector<SharedVar>> get_all_shared(){
            static std::shared_ptr<std::vector<SharedVar>> shared_vars;
            if(not shared_vars){
                shared_vars = std::make_shared<std::vector<SharedVar>>();
            }
            return shared_vars;
        }

        inline SharedVar get_shared(size_t const id){
            return get_all_shared()->at(id);
        }

        inline SharedVar make_shared(DataType data_type, Shape shape, std::string name){
            SharedVar var = std::make_shared<SharedVariable>(get_all_shared()->size(), data_type, shape, name);
            get_all_shared()->push_back(var);
            return get_all_shared()->back();
        }
    }
}

#endif //METADIFF_GRAPH_IR_SHARED_H
