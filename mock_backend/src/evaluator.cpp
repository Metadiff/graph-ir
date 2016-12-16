//
// Created by alex on 07/12/16.
//

#include "mock.h"

namespace md {
    namespace backend {
        namespace mock {
            std::shared_ptr <AbstractMockFunction> MockBackend::make_in_memory_function(GraphFunction const &gf) {
                std::cout << "DA" << std::endl;
                return make_source_gen_function(gf);
            }
        }
    }
}