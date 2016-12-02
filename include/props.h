//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_GRAPH_IR_PROPS_H
#define METADIFF_GRAPH_IR_PROPS_H
namespace md{
    namespace gir{
        /** A container class for all user specified properties, which affect the response of the API */
        class Properties{
        public:
            /** A Http Proxy, if the API would need internet access this can be handy */
            std::string http_proxy;
            /** The delimiter used for combining group names */
            std::string scope_delimiter;
            /** The default device */
            Device default_device;
            /** The maixmum allowed floating numbers precision */
            Precision max_float;
            /** The maixmum allowed integer numbers precision */
            Precision max_int;
            /** The collection of policies for reaction to errors */
            GraphPolicies policies;
            /** Currently not used */
//            DataType promotion_table[13][13];
            /** Default working directory */
            std::string default_work_dir;

            Properties():
                    http_proxy("HTTP_PROXY"),
                    scope_delimiter("::"),
                    default_device(HOST),
                    max_float(p32),
                    max_int(p32),
                    policies(WARN, WARN, WARN),
                    default_work_dir("."){
//                for(auto i=0; i<13; ++i){
//                    for(auto j=0; j<13; ++j){
//                        promotion_table[i][j] = default_promotion_table[i][j];
//                    }
//                }
            };

            Properties(std::shared_ptr<Properties> ptr):
                    http_proxy(ptr->http_proxy),
                    scope_delimiter(ptr->scope_delimiter),
                    default_device(ptr->default_device),
                    max_float(ptr->max_float),
                    max_int(ptr->max_int),
                    policies(ptr->policies) {
//                for(auto i=0; i<13; ++i){
//                    for(auto j=0; j<13; ++j){
//                        promotion_table[i][j] = ptr->promotion_table[i][j];
//                    }
//                }
            }
        };

        /** @brief The default properties are defined by the configuration files and environmental flags on the system
         *
         * @return
         */
        inline std::shared_ptr<Properties> default_properties(){
            static std::shared_ptr<Properties> props;
            if(not props){
                // TODO Load this from a file
                props = std::make_shared<Properties>();
            }
            return props;
        }
    }
}
#endif //METADIFF_GRAPH_IR_PROPS_H
