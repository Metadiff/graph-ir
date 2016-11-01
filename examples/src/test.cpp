//
// Created by alex on 29/09/16.
//
#include <metadiff.h>
//#include "symbolic_integers.h"
//#include "iostream"

using namespace md::api;

md::Graph build_model(){
    // Use default graph
    auto g = default_graph();
    g->name = "MNIST";
    // Dynamic Batch size
    auto batch_size = new_sym();
    // Architecture
    int const N = 8;
    int d[N + 1] = {784, 1000, 500, 250, 30, 250, 500, 1000, 784};
    // Input data
    g->set_group("Inputs");
    md::Node input = g->matrix(md::f32, {d[0], batch_size}, "Input");
    // Parameters
    md::NodeVec params;
    Node W, b, h;
    // Network
    h = input;
    for(int i=1;i<N;i++){
        g->set_group("Layer" + std::to_string(i));
        W = g->shared_var(md::make_shared(md::f32, {d[i], d[i-1], 1, 1}, "W_" + std::to_string(i)));
        b = g->shared_var(md::make_shared(md::f32, {d[i], 1, 1, 1}, "b_" + std::to_string(i)));
        params.push_back(W);
        params.push_back(b);
        h = tanh(dot(W, h) + b);
    }
    // Calculate only logits here
    g->set_group("Layer" + std::to_string(N));
    W = g->shared_var(md::make_shared(md::f32, {d[N], d[N - 1], 1, 1}, "W_" + std::to_string(N)));
    b = g->shared_var(md::make_shared(md::f32, {d[N], 1, 1, 1}, "b_" + std::to_string(N)));
    params.push_back(W);
    params.push_back(b);
    h = dot(W, h) + b;
    // Loss
    g->set_group("Objective");
    Node loss = sum(square(h - input));
    auto grads = g->gradient(loss, params);
    // Some test nodes
    auto a = !grads[0];
    auto r = grads[0] > (grads[0] + 1);
    return g;
}

int main(){
    auto batch_size = new_sym();
    std::cout << batch_size << std::endl;
    md::utils::log_sink()->add_sink(std::make_shared<spdlog::sinks::stdout_sink_st>());
    auto g = build_model();
    std::ofstream f;
    f.open("test.html");
    md::cytoscape::write_graph(g, f);
    f.close();
    md::json::write_graph(g, std::cout);
    std::cout << std::endl << g->name << " " << g->nodes.size() << std::endl;
    std::cout << md::sym::registry()->total_ids << " "
            << md::sym::registry()->floor_registry.size() << " "
            << md::sym::registry()->ceil_registry.size() << std::endl;
    return 0;
}