//
// Created by alex on 30/09/16.
//

#ifndef METADIFF_GRAPH_IR_DEFINITIONS_H
#define METADIFF_GRAPH_IR_DEFINITIONS_H
namespace md{
    namespace gir{
        /** Logger */
        typedef std::shared_ptr<spdlog::logger> Logger;
        /** LogLevel */
        typedef spdlog::level::level_enum LogLevel;
        /** Logger Sink */
        typedef std::shared_ptr<spdlog::sinks::dist_sink_st> LogSink;
        /** Forward declaration */
        class GraphInternal;
        /** Just a pointer to GraphInternal */
        typedef GraphInternal* GraphInPtr;
        /** A symbolic integer is represented as a Polynomial over the integers */
        typedef sym::Polynomial SymInt;
        /** All shapes are represented as an array of 4 symbolic integers */
        typedef std::array<SymInt, 4> Shape;
        /** Axes is used for reduction-alike operators */
        typedef std::vector<int> Axes;
        namespace op {
            /** Forward declaration */
            class AbstractOperator;
        }
        /** Forward declaration */
        class NodeInternal;
        /** Forward declaration */
        class Node;
        /** Forward declaration */
        class NodeData;
        /** Operator is just a shared_ptr to AbstractOperator */
        typedef std::shared_ptr<op::AbstractOperator> Operator;
        /** Vector of Nodes */
        typedef std::vector<Node> NodeVec;
        /** An update is a pair of shared variable and a node */
        typedef std::pair<Node, Node> Update;
        /** A collection of updates */
        typedef std::vector<std::pair<Node, Node>> Updates;
        /** A shared_ptr to GraphInternal */
        typedef std::shared_ptr<gir::GraphInternal> Graph;

        /** A collection of all the policies we have on the graph */
        class GraphPolicies{
        public:
            /** Error policy for implicit broadcasts */
            Policy implicit_broadcast;
            /** Error policy for implicit cast */
            Policy implicit_cast;
            /** Error policy for requesting a derivative of f with respect to x, but f is independent of x */
            Policy independent_derivative;

            GraphPolicies(Policy const implicit_broadcast,
                          Policy const implicit_cast,
                          Policy const independent_derivative):
                    implicit_broadcast(implicit_broadcast),
                    implicit_cast(implicit_cast),
                    independent_derivative(independent_derivative) {};

            GraphPolicies(GraphPolicies const & graph_policies):
                    implicit_broadcast(graph_policies.implicit_broadcast),
                    implicit_cast(graph_policies.implicit_cast),
                    independent_derivative(graph_policies.independent_derivative) {};

        };

        /**
         * A single computational device to facilitate hetergenous execution
         * TODO not yet well designed, high probability it will change in the future
         */
        class Device {
        public:
            /** Type of the device - host or gpu*/
            DeviceType type;
            /** A unique identifier of the device */
            size_t id;

            Device(const DeviceType type, const size_t id) :
                    type(type),
                    id(id) { };

            Device(const std::shared_ptr<Device> ptr) :
                    type(ptr->type),
                    id(ptr->type) { };
            Device(Device const & device) :
                    type(device.type),
                    id(device.id) { };

        };

        /** The local CPU Device */
        Device const HOST = Device(CPU, 0);

        inline bool operator==(Device const & device1, Device const & device2){
            return device1.id == device2.id and device1.type == device2.type;
        }

        inline bool operator!=(Device const & device1, Device const & device2){
            return device1.id != device2.id or device1.type != device2.type;
        }

        /**
         * The class provides meta data which is populated by the optimizers, which specifies
         * specific recommendations of how the exact node computation should be executed.
         * This can be laveraged by the backends in order to produce code which is better,
         * however they are free to ignore it.
         */
        class ExecutionData {
        public:
            /** A flag whether the computation to be done in-place, reusing the memory of the ancestor
             * with this index. -1 indicates that this is should not be done in-place */
            int inplace_parent_id;
            /** For backends which use separate buffers per variable */
            size_t buffer_id;
            /** For backends which use a pool of memory this is the initial offset */
            sym::Polynomial buffer_offset;
            /** For backends which use a pool of memory this is the total size in bytes */
            sym::Polynomial buffer_size;
            /** For backends which release explicitely memory this is the first computation
             * in the topological sorting at which you can reuse the memory buffer */
            size_t lifespan;

            ExecutionData() :
                    inplace_parent_id(-1),
                    buffer_id(-1),
                    buffer_offset(-1),
                    buffer_size(-1) {};

            ExecutionData(ExecutionData const & data) :
                    inplace_parent_id(data.inplace_parent_id),
                    buffer_id(data.buffer_id),
                    buffer_offset(data.buffer_offset),
                    buffer_size(data.buffer_size) {};
        };

//        /**
//         * A NodeGroup is an abstraction which allows the user to group different nodes together.
//         * This is intended for purely vizualization reasons.
//         */
//        class NodeGroup {
//        public:
//            /** The name of this group */
//            std::string const name;
//            /** The owning graph */
//            GraphInPtr const graph;
//            /** The parent NodeGroup */
//            std::weak_ptr<NodeGroup> const parent;
//            /** This is the full name of the group, which depends on the parent */
//            std::string full_name;
//            /** The children groups */
//            std::vector<std::weak_ptr<NodeGroup>> children;
//
//            NodeGroup(const std::string name,
//                      const std::weak_ptr<NodeGroup> parent);
//
//            NodeGroup(const std::string name,
//                      const GraphInPtr graph);
//
//            bool is_base() const;
//
//        };
//        /** Group is a weak_ptr as it is owned by the graph */
//        typedef std::weak_ptr<gir::NodeGroup> Group;
    }
}
#endif //METADIFF_GRAPH_IR_DEFINITIONS_H
