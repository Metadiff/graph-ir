//
// Created by alex on 07/11/16.
//

#ifndef METADIFF_GRAPH_IR_BACKEND_MOCK_H
#define METADIFF_GRAPH_IR_BACKEND_MOCK_H

namespace md{
    namespace backend{
        class MockVariable{
        public:
            DataType const data_type;
            std::array<long, 4> const shape;
            bool const managed;
            void * data;
            MockVariable(DataType data_type, std::array<long, 4> shape):
                    data_type(data_type), shape(shape), managed(false){
                data = std::malloc(total_size() * (data_type.precision + 1));
            }

            MockVariable(DataType data_type, std::array<long, 4> shape, void * data):
                    data_type(data_type), shape(shape), managed(true), data(data){}

            ~MockVariable(){
                if(not managed) {
                    free(data);
                }
            }

            long total_size(){
                return shape[0] * shape[1] * shape[2] * shape[3];
            }

            template <typename T>
            T* get(){
                return (T*) data;
            }
        };

        typedef std::vector<MockVariable> VarVec;
        typedef std::pair<std::vector<MockVariable>, std::vector<MockVariable>> Outputs;

        class MockBackend: public AbstractFunctionBackend<MockVariable, MockVariable, MockVariable>{
        private:
            filesystem::path dll_path;
            typedef Outputs (*func_ptr)(VarVec& inputs, VarVec& constants, VarVec& shared);
            typedef dll_symbol<Outputs(VarVec& inputs, VarVec& constants, VarVec& shared)> symbol;
            std::shared_ptr<symbol> eval_func;
        public:
            VarVec constants;
            VarVec shared;

            MockBackend(GraphFunction const gf, std::string work_dir, bool debug):
                    AbstractFunctionBackend("Mock", gf, work_dir, debug) {
            };

            MockBackend(GraphFunction const gf, bool debug):
                    AbstractFunctionBackend("Mock", gf, debug) {
            };

            Outputs operator()(VarVec & inputs){
                if(eval_func){
                    return eval_func->f(inputs, constants, shared);
                } else {
                    throw 1;
                }
            };

            void initialize(){
                // TODO Initialized shared variables
            }

            void generate_sources();

            void compile();

            void link();

            void complete(){
                if(not eval_func) {
                    generate_sources();
                    compile();
                    link();
                }
            }
        };
    }
}
#endif //METADIFF_GRAPH_IR_BACKEND_MOCK_H
