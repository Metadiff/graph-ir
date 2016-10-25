//
// Created by alex on 24/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{
        std::shared_ptr<Properties> default_properties(){
            static std::shared_ptr<Properties> props;
            if(not props){
                props = std::make_shared<Properties>();
            }
            return props;
        }
    }
}

