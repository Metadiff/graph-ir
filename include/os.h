//
// Created by alex on 02/11/16.
//

#ifndef METADIFF_GRAPH_IR_OS_H
#define METADIFF_GRAPH_IR_OS_H

namespace md{
    namespace os{
        /** Helper for writing to files */
        struct FtpFile {
            const char *filename;
            FILE *stream;
        };

        /** Path separator used for the file system */
        const char kPathSeparator =
#ifdef _WIN32
                '\\';
#else
                '/';
#endif

        /** Checks if a the file specified by the path exists on the file system */
        bool exists(std::string path);

        /** Returns false if the path does not exists or if it is not a directory
         * TODO - make this cross-platform */
        bool is_dir(std::string path);

        /** Helper function to create directories
         * If check is true the directory is created only if it does not exists */
        void create_dir(std::string path, bool check = false);

        /** Function to create a temporary directory and return its path */
        std::string make_temp_dir();

        /** Joins os paths **/
        std::string join_paths(std::vector<std::string> paths);

        /** Joins os paths **/
        std::string join_paths(std::string path1, std::string path2);

        /** Joins os paths **/
        std::string join_paths(std::string path1, std::string path2, std::string path3);

        /** Helper writes stream to files */
        size_t write_steram_to_file(void *buffer, size_t size, size_t nmemb, void *stream);

        /** Returns the size of a file in bytes */
        long long file_size(std::string file_name);

        /** Unzips a file */
        int unpack_gz(std::string gz_path);

        //        /** A helper function to download a file from a url */
        //        void download_file(std::string url, std::string local_path);
    }
}
#endif //METADIFF_GRAPH_IR_OS_H
