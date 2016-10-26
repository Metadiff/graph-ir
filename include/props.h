//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_CORE_UTILS_PROPS_H
#define METADIFF_CORE_UTILS_PROPS_H
namespace md{
    namespace core{
        /**
         * This class contains gloal properties which should be loaded from file at begining of execution of the program
         * or filled in with default values;
         */
        class Properties{
        public:
            std::string http_proxy;
            std::string base_group_prefix;
            char group_delimiter;
            Device default_device;
            dataType max_float;
            dataType max_int;
            Policies policies;
            dataType promotion_table[13][13];
            unsigned long long random_seed;

            Properties():
                    http_proxy("HTTP_PROXY"),
                    base_group_prefix("Grads"),
                    group_delimiter('.'),
                    default_device(host()),
                    max_float(f32),
                    max_int(i32),
                    policies(WARN, WARN, WARN, WARN),
                    random_seed(0) {
                for(auto i=0; i<13; ++i){
                    for(auto j=0; j<13; ++j){
                        promotion_table[i][j] = default_promotion_table[i][j];
                    }
                }
            };

            Properties(std::shared_ptr<Properties> ptr):
                    http_proxy(ptr->http_proxy),
                    base_group_prefix(ptr->base_group_prefix),
                    group_delimiter(ptr->group_delimiter),
                    default_device(ptr->default_device),
                    max_float(ptr->max_float),
                    max_int(ptr->max_int),
                    policies(ptr->policies) {
                for(auto i=0; i<13; ++i){
                    for(auto j=0; j<13; ++j){
                        promotion_table[i][j] = ptr->promotion_table[i][j];
                    }
                }
            }

            Properties(Properties* const ptr):
                    http_proxy(ptr->http_proxy),
                    base_group_prefix(ptr->base_group_prefix),
                    group_delimiter(ptr->group_delimiter),
                    default_device(ptr->default_device),
                    max_float(ptr->max_float),
                    max_int(ptr->max_int),
                    policies(ptr->policies) {
                for(auto i=0; i<13; ++i){
                    for(auto j=0; j<13; ++j){
                        promotion_table[i][j] = ptr->promotion_table[i][j];
                    }
                }
            }
        };

        /** These should be loaded from a system file at start */
        std::shared_ptr<Properties> default_properties();
    }
}
#endif //METADIFF_CORE_UTILS_PROPS_H
