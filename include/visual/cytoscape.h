//
// Created by alex on 19/10/16.
//

#ifndef METADIFF_CORE_VISUAL_CYTOSCAPE_H
#define METADIFF_CORE_VISUAL_CYTOSCAPE_H

#include "unordered_map"

namespace md{
    using namespace core;
    namespace cytoscape{
        typedef std::vector<std::pair<size_t, size_t>> Edges;
        typedef std::unordered_map<std::string, Group> GroupMap;

        void write_graph(Graph g, std::ostream& s);

        std::pair<Edges, GroupMap> write_nodes(Graph g, std::ostream& s);

        void write_edges(Edges& edges, std::ostream& s);

        void write_groups(GroupMap& groups, std::ostream& s);

        void write_header(std::string name, std::ostream& s);

        void write_footer(std::string name, std::ostream& s);

    }
}
#endif //METADIFF_CORE_EXPORT_CYTOSCAPEa_H
