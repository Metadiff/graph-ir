//
// Created by alex on 29/09/16.
//
#include <metadiff.h>
#include "metadiff.h"
#include "iostream"

md::Graph build_model(){
    auto g = md::create_graph();
    g->name = "MNIST";
    // Dynamic Batch size
    auto batch_size = g->new_sym();
    // Architecture
    int d[9] = {784, 1000, 500, 250, 30, 250, 500, 1000, 784};
    auto test = g->constant(20, md::i32);
    std::string layers[10] {"Inputs", "Encoder 1", "Encoder 2", "Encoder 3", "Encoder 4",
                            "Decoder 3", "Decoder 2", "Decoder 1", "Output Layer", "Objective"};
    // Input data
    g->set_group(layers[0]);
    md::NodeVec inputs = {g->matrix(md::f32, {d[0], batch_size}, "Input")};
    // Parameters
    md::NodeVec params;
    md::SharedVar var;
    for(int i=1;i<9;i++){
        g->set_group(layers[i]);
        var = md::make_shared(md::f32, {d[i], d[i-1], 1, 1}, "W_" + std::to_string(i));
        params.push_back(g->wrap(var));
        var = md::make_shared(md::f32, {d[i], 1, 1, 1}, "b_" + std::to_string(i));
        params.push_back(g->wrap(var));
    }
    // First layer
    g->set_group(layers[1]);
    auto h = g->tanh(g->add(g->dot(params[0], inputs[0]), params[1]));
    // All layers except one
    for(int i=1;i<7;i++){
        g->set_group(layers[i+1]);
        h = g->tanh(g->add(g->dot(params[2*i], h), params[2*i+1]));
    }
    // Calculate only logits here
    g->set_group(layers[8]);
    h = g->add(g->dot(params[14], h), params[15]);
    // Loss
    g->set_group(layers[9]);
    auto error = g->square(g->neg(h, inputs[0]));
    auto loss = g->sum(error);
    auto grads = g->gradient(loss, params);
    auto a = g->logical_not(grads[0]);
    auto b = g->greater_than(grads[0], g->add(grads[0], g->constant(1)));
    return g;
}

int main(){
    md::utils::log_sink()->add_sink(std::make_shared<spdlog::sinks::stdout_sink_st>());
    auto g = build_model();
    std::ofstream f;
    f.open("test.html");
    md::cytoscape::write_graph(g, f);
    f.close();
    md::json::write_graph(g, std::cout);
    return 0;
}
