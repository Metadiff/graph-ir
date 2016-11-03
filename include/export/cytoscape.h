//
// Created by alex on 19/10/16.
//

#ifndef METADIFF_GRAPH_IR_VISUAL_CYTOSCAPE_H
#define METADIFF_GRAPH_IR_VISUAL_CYTOSCAPE_H

#include "set"

namespace md{
    using namespace gir;
    namespace cytoscape{
        typedef std::vector<std::pair<size_t, size_t>> Edges;
        typedef std::set<std::string> GroupSet;

        void export_graph(Graph g, std::ostream& s);

        std::pair<Edges, GroupSet> export_nodes(Graph g, std::ostream& s);

        void export_edges(Edges& edges, std::ostream& s);

        void export_groups(Graph g, GroupSet& groups, std::ostream& s);

        void export_header(std::string name, std::ostream& s);

        void export_footer(std::string name, std::ostream& s);

    }
}
#endif //METADIFF_GRAPH_IR_EXPORT_CYTOSCAPEa_H
