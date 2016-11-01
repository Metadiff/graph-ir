//
// Created by alex on 25/10/16.
//

#include "metadiff.h"

namespace md{
    namespace json{
        void write_graph(Graph const g, std::ostream& s){
            StringBuffer sb;
            PrettyWriter<StringBuffer> writer(sb);
            write_graph(g, writer);
            s << sb.GetString();
        }

        void write_graph(Graph const g, PrettyWriter<StringBuffer>& writer){
            writer.StartObject();
            writer.String("name");
            writer.String(g->name);
            writer.String("props");
            write_props(g->props, writer);
            writer.String("groups");
            write_groups(g->groups, writer);
            writer.String("nodes");
            write_nodes(g->nodes, writer);
            writer.EndObject();
        }

        void write_props(Properties const properties, PrettyWriter<StringBuffer>& writer){
            writer.StartObject();
            writer.String("http_proxy");
            writer.String(properties.http_proxy);
            writer.String("group_delimiter");
            writer.String(std::string(1, properties.group_delimiter));
            writer.String("max_float");
            writer.String(to_string(properties.max_float));
            writer.String("max_int");
            writer.String(to_string(properties.max_int));
            writer.String("policies");
            write_policies(properties.policies, writer);
            writer.String("random_seed");
            writer.Uint64(properties.random_seed);
            writer.EndObject();
        }

        void write_policies(GraphPolicies const policies, PrettyWriter<StringBuffer>& writer){
            writer.StartObject();
            writer.String("implicit_broadcast");
            writer.String(to_string(policies.implicit_broadcast));
            writer.String("data_type_promotion");
            writer.String(to_string(policies.data_type_promotion));
            writer.String("cast");
            writer.String(to_string(policies.cast));
            writer.String("non_dependable_gradient");
            writer.String(to_string(policies.non_dependable_gradient));
            writer.EndObject();
        }

        void write_groups(std::vector<std::shared_ptr<NodeGroup>> const & groups,
                          PrettyWriter<StringBuffer>& writer){
            writer.StartArray();
            for(auto i=0; i < groups.size(); ++i){
                writer.String(groups[i]->full_name);
            }
            writer.EndArray();
        }

        void write_nodes(std::vector<std::shared_ptr<NodeData>> const & nodes,
                         PrettyWriter<StringBuffer>& writer){
            writer.StartArray();
            for(auto i=0; i<nodes.size(); ++i){
                writer.StartObject();
                writer.String("id");
                writer.Uint64(nodes[i]->id);
                writer.String("name");
                writer.String(nodes[i]->name);
                writer.String("operator");
                write_op(nodes[i]->op, writer);
                writer.String("grad_level");
                writer.Uint(nodes[i]->grad_level);
                writer.String("group");
                writer.String(nodes[i]->group.lock()->full_name);
                // TODO ExecutionData
                writer.EndObject();
            }
            writer.EndArray();
        }

        void write_shape(Shape const shape,  PrettyWriter<StringBuffer>& writer){
            writer.StartArray();
            for(auto i=0; i<4; ++i){
                if(shape[i].is_constant()){
                    writer.Int(shape[i].eval());
                } else {
                    write_sym_int(shape[i], writer);
                }
            }
            writer.EndArray();
        }

        void write_sym_int(SymInt const sym_int,  PrettyWriter<StringBuffer>& writer){
            // TODO
            writer.StartObject();
            writer.String("SymInt");
            writer.String("TODO");
            writer.EndObject();
        }

        void write_op(Operator const op,  PrettyWriter<StringBuffer>& writer){
            auto parents = op->get_parents();
            auto arguments = op->get_arguments();
            writer.StartObject();
            writer.String("name");
            writer.String(op->name);
            // Parents
            writer.String("parents");
            writer.StartArray();
            for(auto i=0; i<parents.size(); ++i){
                writer.Uint64(parents[i]->id);
            }
            writer.EndArray();
            // Arguments
            writer.String("arguments");
            writer.StartArray();
            for(auto i=0; i<arguments.size(); ++i){
                writer.Uint64(arguments[i]->id);
            }
            writer.EndArray();
            // Specifics for operators with other arguments
            {
                // ConstantOperator
                auto cast_op = std::dynamic_pointer_cast<op::ConstantOperator>(op);
                if (cast_op) {
                    writer.String("data_type");
                    writer.String(to_string(cast_op->data_type));
                }
            }
            {
                // ReductionOperator
                auto cast_op = std::dynamic_pointer_cast<op::ReductionOperator>(op);
                if (cast_op) {
                    writer.String("axes");
                    writer.StartArray();
                    for(auto i=0;i<cast_op->axes.size(); ++i) {
                        writer.Int(cast_op->axes[i]);
                    }
                    writer.EndArray();
                }
            }
            {
                // Input
                auto cast_op = std::dynamic_pointer_cast<op::Input>(op);
                if (cast_op) {
                    writer.String("data_type");
                    writer.String(to_string(cast_op->data_type));
                    writer.String("shape");
                    write_shape(cast_op->shape, writer);
                }
            }
            {
                // SharedVar
                auto cast_op = std::dynamic_pointer_cast<op::SharedInput>(op);
                if (cast_op) {
                    writer.String("id");
                    writer.Uint64(cast_op->var->id);
                    writer.String("name");
                    writer.String(cast_op->var->name);
                    writer.String("data_type");
                    writer.String(to_string(cast_op->var->data_type));
                    writer.String("shape");
                    write_shape(cast_op->var->shape, writer);
                }
            }
            {
                // ConstantValue
                auto cast_op = std::dynamic_pointer_cast<op::ConstantValue>(op);
                if (cast_op) {
                    writer.String("value");
                    writer.Double(cast_op->value);
                    writer.String("shape");
                    write_shape(cast_op->shape, writer);
                }
            }
            {
                // SymIntWrapper
                auto cast_op = std::dynamic_pointer_cast<op::SymIntWrapper>(op);
                if (cast_op) {
                    writer.String("value");
                    write_sym_int(cast_op->value, writer);
                }
            }
            {
                // Range
                auto cast_op = std::dynamic_pointer_cast<op::Range>(op);
                if (cast_op) {
                    writer.String("start");
                    write_sym_int(cast_op->start, writer);
                    writer.String("end");
                    write_sym_int(cast_op->end, writer);
                }
            }
            {
                // Eye
                auto cast_op = std::dynamic_pointer_cast<op::Eye>(op);
                if (cast_op) {
                    writer.String("size");
                    write_sym_int(cast_op->size, writer);
                }
            }
            {
                // Cast
                auto cast_op = std::dynamic_pointer_cast<op::Cast>(op);
                if (cast_op) {
                    writer.String("data_type");
                    writer.String(to_string(cast_op->data_type));
                }
            }
            {
                // Broadcast
                auto cast_op = std::dynamic_pointer_cast<op::Broadcast>(op);
                if (cast_op) {
                    writer.String("to_shape");
                    write_shape(cast_op->to_shape, writer);
                }
            }
            {
                // Reshape
                auto cast_op = std::dynamic_pointer_cast<op::Reshape>(op);
                if (cast_op) {
                    writer.String("shape");
                    write_shape(cast_op->shape, writer);
                }
            }
            {
                // Reorder
                auto cast_op = std::dynamic_pointer_cast<op::Reorder>(op);
                if (cast_op) {
                    writer.String("order");
                    writer.StartArray();
                    for(auto i=0; i<cast_op->order.size(); ++i){
                        writer.Int(cast_op->order[i]);
                    }
                    writer.EndArray();
                }
            }
            {
                // ApproximatelyEquals
                auto cast_op = std::dynamic_pointer_cast<op::ApproximatelyEquals>(op);
                if (cast_op) {
                    writer.String("tolerance");
                    writer.Double(cast_op->tolerance);
                }
            }
            {
                // MatrixMul
                auto cast_op = std::dynamic_pointer_cast<op::MatrixMul>(op);
                if (cast_op) {
                    writer.String("transpositions");
                    writer.StartArray();
                    for(auto i=0; i<cast_op->transpositions.size(); ++i){
                        writer.Bool(cast_op->transpositions[i]);
                    }
                    writer.EndArray();
                }
            }
            {
                // Softplus
                auto cast_op = std::dynamic_pointer_cast<op::Softplus>(op);
                if (cast_op) {
                    writer.String("threshold");
                    writer.Double(cast_op->threshold);
                }
            }
            {
                // LogSumExp
                auto cast_op = std::dynamic_pointer_cast<op::LogSumExp>(op);
                if (cast_op) {
                    writer.String("threshold");
                    writer.Double(cast_op->threshold);
                }
            }
            {
                // Softmax
                auto cast_op = std::dynamic_pointer_cast<op::Softmax>(op);
                if (cast_op) {
                    writer.String("axes");
                    writer.StartArray();
                    for(auto i=0; i<cast_op->axes.size(); ++i){
                        writer.Int(cast_op->axes[i]);
                    }
                    writer.EndArray();
                }
            }
            {
                // RandomUniform
                auto cast_op = std::dynamic_pointer_cast<op::RandomUniform>(op);
                if (cast_op) {
                    writer.String("shape");
                    write_shape(cast_op->shape, writer);
                }
            }
            {
                // RandomNormal
                auto cast_op = std::dynamic_pointer_cast<op::RandomNormal>(op);
                if (cast_op) {
                    writer.String("shape");
                    write_shape(cast_op->shape, writer);
                }
            }
            {
                // Print
                auto cast_op = std::dynamic_pointer_cast<op::Print>(op);
                if (cast_op) {
                    writer.String("msg");
                    writer.String(cast_op->msg);
                    writer.String("monitored");
                    writer.Uint64(cast_op->monitored->id);
                }
            }
            {
                // Retrieve
                auto cast_op = std::dynamic_pointer_cast<op::Retrieve>(op);
                if (cast_op) {
                    writer.String("msg");
                    writer.String(cast_op->msg);
                    writer.String("monitored");
                    writer.Uint64(cast_op->monitored->id);
                }
            }
            {
                // LogToFile
                auto cast_op = std::dynamic_pointer_cast<op::LogToFile>(op);
                if (cast_op) {
                    writer.String("msg");
                    writer.String(cast_op->msg);
                    writer.String("monitored");
                    writer.Uint64(cast_op->monitored->id);
                }
            }
            {
                // Guard
                auto cast_op = std::dynamic_pointer_cast<op::Guard>(op);
                if (cast_op) {
                    writer.String("msg");
                    writer.String(cast_op->msg);
                    writer.String("monitored");
                    writer.Uint64(cast_op->monitored->id);
                    writer.String("low");
                    writer.Double(cast_op->low);
                    writer.String("high");
                    writer.Double(cast_op->high);
                }
            }

            writer.EndObject();
        }

    }
}