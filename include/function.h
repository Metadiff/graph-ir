//
// Created by alex on 03/11/16.
//

#ifndef METADIFF_GRAPH_IR_FUNCTION_H
#define METADIFF_GRAPH_IR_FUNCTION_H

namespace md{
    namespace gir{
        class GraphFunction{
        public:
            Graph graph;
            std::vector<Node> inputs;
            std::vector<Node> outputs;

            GraphFunction(Graph const full_graph,
                          std::vector<Node> inputs,
                          std::vector<Node> outputs,
                          Updates extra_updates){
                graph = api::create_graph();
                std::vector<Node> leafs = outputs;
                // Add the updates of the graph
                for(auto i=0; i<full_graph->updates.size(); ++i){
                    leafs.push_back(Node(extra_updates[i].second));
                }
                // Add the extra updates
                for(auto i=0; i<extra_updates.size(); ++i){
                    leafs.push_back(extra_updates[i].second);
                }
                // Get ancestor mask of all of the outputs
                auto ancestor_mask = full_graph->get_ancestors_mask(leafs);
                auto mapping = full_graph->copy(graph, ancestor_mask);
                // Add the extra updates
                for(auto i=0; i<extra_updates.size(); ++i){
                    api::update(mapping[extra_updates[i].first->id],
                                mapping[extra_updates[i].second->id]);
                }
            }
        };
    }
}
#endif //METADIFF_GRAPH_IR_FUNCTION_H
