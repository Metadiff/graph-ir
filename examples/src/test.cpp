//
// Created by alex on 29/09/16.
//
#include "graph_ir.h"
#include "boost/filesystem.hpp"
#include "iostream"
#include "fstream"

/**
#define debug(fmt, ...)\
try{\
    fmt\
} catch(const std::exception& e){\
    std::cerr<< "Exception at line " << __LINE__  << std::endl;\
    throw e;\
}
**/

using namespace md::api;
using namespace md::backend;

md::GraphFunction build_model(){
    auto p = filesystem::path(default_properties()->default_work_dir);
    if(exists(p)){
        std::cout << "Exists" << std::endl;
    } else {
        std::cout << "Does not exists" << std::endl;
    }
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
//        debug(h = dot(W, h);)
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
    auto grads = gradient(loss, params);
    // Some test nodes
    auto a = !grads[0];
    auto r = grads[0] > (grads[0] + 1);
    return GraphFunction(g, NodeVec{input}, grads);
}

int main(){
    md::gir::console_logging(true);
    auto gf = build_model();
    std::ofstream f;
    f.open("test.html");
    md::cytoscape::export_graph(gf.graph, f);
    f.close();
    f.open("test.json");
    md::json::export_graph(gf.graph, f);
    f.close();
    auto backend = MockBackend(gf, "mock", false);
    backend.complete();
    VarVec vars;
    auto r = backend(vars);
    std::cout << r.first.size() << " - " << r.second.size() << std::endl;
    return 0;
}