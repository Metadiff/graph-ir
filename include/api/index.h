//
// Created by alex on 24/11/16.
//

#ifndef GRAPH_IR_API_INDEX_H
#define GRAPH_IR_API_INDEX_H

namespace md{
    namespace api{

        Node slice(Node node, Axes axes, std::vector<std::pair<SymInt, SymInt>> slices);

        Node slice(Node node, int axis, SymInt start, SymInt end);

        Node index(Node node, Axes axes, std::vector<Node> indexes);

        Node index(Node node, int axis, Node index);

        Node cross_index(Node node, int axis, Node index);
    }
}
#endif //GRAPH_IR_API_INDEX_H
