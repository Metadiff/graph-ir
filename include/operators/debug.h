//
// Created by alex on 21/10/16.
//

#ifndef METADIFF_CORE_OPERATORS_DEBUG_H
#define METADIFF_CORE_OPERATORS_DEBUG_H

namespace md{
    namespace core{
        namespace op{
            /** A Monitor attached to parent1, reporting parent2 */
            class Monitor: public MorphElementwiseOperator{
            public:
                Node reported;
                std::string id;
                bool print;
                bool log;
                bool provide;
                bool guard;

                Monitor(Node reported,
                                std::string id,
                                bool print, bool log, bool provide, bool guard):
                reported(reported), id(id),
                print(print), log(log), provide(provide), guard(guard) {};

                NodeVec get_arguments() const{
                    return {reported};
                }

                Node get_parent_grad(Node my_grad, short index){
                    return my_grad;
                }

                bool equals(Operator const op) const {
                    return false;
                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_DEBUG_H
