//
// Created by alex on 25/10/16.
//

#ifndef METADIFF_CORE_JSON_H
#define METADIFF_CORE_JSON_H

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

namespace md{
    namespace json{
        using namespace rapidjson;

        /** Writes a graph to a stream in a json format */
        void write_graph(Graph const g, std::ostream& s);

        /** Writes a graph to a wrtier from RapidJson */
        void write_graph(Graph const g, PrettyWriter<StringBuffer>& writer);

        /** Writes a Properties object to a wrtier from RapidJson */
        void write_props(Properties const properties, PrettyWriter<StringBuffer>& writer);

        /** Writes a Policies object to a wrtier from RapidJson */
        void write_policies(GraphPolicies const policies, PrettyWriter<StringBuffer>& writer);

        /** Writes a vector of NodeGroup objects to a wrtier from RapidJson */
        void write_groups(std::vector<std::shared_ptr<NodeGroup>> const & groups,
                          PrettyWriter<StringBuffer>& writer);

        /** Writes a vector of NodeData objects to a wrtier from RapidJson */
        void write_nodes(std::vector<std::shared_ptr<NodeData>> const &  nodes,
                         PrettyWriter<StringBuffer>& writer);

        /** Writes an Operator object to a wrtier from RapidJson */
        void write_op(Operator const op,  PrettyWriter<StringBuffer>& writer);

        /** Write an Shape object to a writer from RapidJson */
        void write_shape(Shape const shape,  PrettyWriter<StringBuffer>& writer);

        /** Write an SymInt object to a writer from RapidJson */
        void write_sym_int(SymInt const sym_int,  PrettyWriter<StringBuffer>& writer);
    }
}

#endif //METADIFF_CORE_JSON_H
