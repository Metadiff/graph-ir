//
// Created by alex on 02/11/16.
//

#include "graph_ir.h"

namespace md{
    namespace os{

        bool exists(std::string path){
            /* TODO - make this cross-platform */
            struct stat buffer;
            return (stat (path.c_str(), &buffer) == 0);
        }

        bool is_dir(std::string path){
            /* TODO - make this cross-platform */
            struct stat info;
            if(stat( path.c_str(), &info ) != 0)
                return false;
            else if(info.st_mode & S_IFDIR)
                return true;
            else
                return false;
        }

        void create_dir(std::string path, bool check) {
            /* TODO - make this cross-platform */
            if((not check) or (not is_dir(path))) {
                const int dir_err = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (-1 == dir_err) {
                    throw 2;
                }
            }
        }

        std::string make_temp_dir(std::string& path_template) {
            /* TODO - make this cross-platform */
            char template_str[path_template.length() + 6];
            for(auto i=0; i<path_template.length(); ++i){
                template_str[i] = path_template[i];
            }
            for(auto i=0; i<6; ++i){
                template_str[i] = 'X';
            }
            return std::string(mkdtemp(template_str));
        };

        std::string join_paths(std::vector<std::string> paths){
            if(paths.size() == 0){
                return "";
            } else{
                std::string path = paths[0];
                for(auto i = 1; i < paths.size(); ++i){
                    if(path.back() != kPathSeparator){
                        path += kPathSeparator;
                    }
                    if(paths[i].front() == kPathSeparator){
                        path += paths[i].substr(1, paths[i].size());
                    } else {
                        path += paths[i];
                    }
                }
                if(path.back() == kPathSeparator){
                    return path.substr(0, path.size() - 1);
                } else {
                    return path;
                }
            }
        }

        size_t write_steram_to_file(void *buffer, size_t size, size_t nmemb, void *stream) {
            struct FtpFile *out=(struct FtpFile *)stream;
            if(out && !out->stream) {
                out->stream=fopen(out->filename, "wb");
                if(!out->stream)
                    return -1; /* failure, can't open file to write */
            }
            return fwrite(buffer, size, nmemb, out->stream);
        }

        long long file_size(std::string file_name) {
            /* TODO - make this cross-platform */
            struct stat st;
            if (stat(file_name.c_str(), &st) == -1)
                return 0;
            return ((long long)st.st_size);
        }

        int unpack_gz(std::string gz_path){
            /* TODO - make this cross-platform */
            return system(("gzip -d -f " + gz_path).c_str());
        }

//        void download_file(std::string url, std::string local_path){
//            CURL *curl;
//            CURLcode res;
//            curl_global_init(CURL_GLOBAL_DEFAULT);
//            curl = curl_easy_init();
//            struct FtpFile data={local_path.c_str(), NULL};
//            if(curl) {
//                if(std::getenv(props()->http_proxy.c_str())){
//                    curl_easy_setopt(curl, CURLOPT_PROXY, std::getenv(props()->http_proxy.c_str()));
//                }
//                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_steram_to_file);
//                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
//                res = curl_easy_perform(curl);
//                if(CURLE_OK != res) {
//                    fprintf(stderr, "curl told us %d\n", res);
//                }
//            }
//            /* always cleanup */
//            curl_easy_cleanup(curl);
//            if(data.stream) {
//                fclose(data.stream);
//            }
//            curl_global_cleanup();
//        }
    }
}
