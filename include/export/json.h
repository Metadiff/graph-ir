//
// Created by alex on 25/10/16.
//

#ifndef METADIFF_GRAPH_IR_JSON_H
#define METADIFF_GRAPH_IR_JSON_H

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

namespace md{
    namespace json{
        using namespace rapidjson;

        /** Exprots the graph to a stream in a json format */
        void export_graph(Graph const g, std::ostream& s);

        /** Exports a graph to a wrtier from RapidJson */
        void export_graph(Graph const g, PrettyWriter<StringBuffer>& writer);

        /** Exports a Properties object to a wrtier from RapidJson */
        void export_props(Properties const properties, PrettyWriter<StringBuffer>& writer);

        /** Exports a Policies object to a wrtier from RapidJson */
        void export_policies(GraphPolicies const policies, PrettyWriter<StringBuffer>& writer);

        /** Exports a Shape object to a writer from RapidJson */
        void export_shape(Shape const shape,  PrettyWriter<StringBuffer>& writer);

        /** Exports a SymInt object to a writer from RapidJson */
        void export_sym_int(SymInt const sym_int,  PrettyWriter<StringBuffer>& writer);

        /** Exports an ExecutionData object to a writer from RapidJson */
        void export_execution_data(ExecutionData execution, PrettyWriter<StringBuffer>& writer);

        /** Exports a vector of NodeData objects to a wrtier from RapidJson */
        void export_nodes(std::vector<std::shared_ptr<NodeData>> const &  nodes,
                         PrettyWriter<StringBuffer>& writer);

        /** Exports an Operator object to a wrtier from RapidJson */
        void export_op(Operator const op,  PrettyWriter<StringBuffer>& writer);
    }
}

#endif //METADIFF_GRAPH_IR_JSON_H
