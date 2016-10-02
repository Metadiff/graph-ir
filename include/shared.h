//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_CORE_SHARED_H
#define METADIFF_CORE_SHARED_H
namespace md{
    namespace core{
        /**
         * The initalizer is used for initializing all shared variables
         */
        class Initializer{
        public:
            virtual Node initialize( std::array<long long int, 4> const real_shape) = 0;
        };

        typedef std::shared_ptr<Initializer> Init;

        /** A shared variable is a like a static variable, which is synchronized between devices */
        class SharedVariable {
        private:
            void * memory_ptr;
            std::array<long long int, 4> real_shape;
            bool initialized = false;
        public:
            size_t const id;
            dataType const data_type;
            Shape const shape;
            std::string const name;
            Init init;
        public:
            SharedVariable(size_t const id, dataType const data_type,
                           Shape const shape, std::string const name):
                    id(id),
                    data_type(data_type),
                    shape(shape),
                    name(name) {};

            void initialize(Init init = Init());
        };

        typedef std::shared_ptr<SharedVariable> SharedVar;

        inline SharedVar get_shared(size_t const id){
            static std::vector<SharedVar> shared_vars;
            return shared_vars[id];
        }
    }
}

#endif //METADIFF_CORE_SHARED_H
