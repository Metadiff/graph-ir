//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_CORE_UTILS_PROPS_H
#define METADIFF_CORE_UTILS_PROPS_H
namespace md{
    namespace utils{
        class Properties{
        public:
            std::string http_proxy;
            std::string group_root;
            char group_delimiter;

            Properties():
                    http_proxy("HTTP_PROXY"),
                    group_root("_root"),
                    group_delimiter('/')
            {};
        };

        inline std::shared_ptr<Properties> props(){
            static std::shared_ptr<Properties> props;
            if(not props){
                props = std::make_shared<Properties>();
            }
            return props;
        }
    }
}
#endif //METADIFF_CORE_UTILS_PROPS_H
