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
    auto p = boost::filesystem::path(default_properties()->default_work_dir);
    if(boost::filesystem::exists(p)){
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
    g->set_scope("Inputs");
    md::Node input = g->matrix(md::f32, {d[0], batch_size}, "input");
    // Parameters
    md::NodeVec params;
    Node W, b, h;
    // Network
    h = input;
    for(int i=1;i<N;i++){
        g->set_scope("Layer" + std::to_string(i));
        W = g->parameter("W", md::f32, {d[i], d[i-1], 1, 1});
        b = g->parameter("b", md::f32, {d[i], 1, 1, 1});
        params.push_back(W);
        params.push_back(b);
        h = tanh(dot(W, h) + b);
//        debug(h = dot(W, h);)
    }
    // Calculate only logits here
    g->set_scope("Layer" + std::to_string(N));
    W = g->parameter("W", md::f32, {d[N], d[N - 1], 1, 1});
    b = g->parameter("b", md::f32, {d[N], 1, 1, 1});
    params.push_back(W);
    params.push_back(b);
    h = dot(W, h) + b;
    // Loss
    g->set_scope("Objective");
    Node loss = sum(square(h - input));
    auto grads = gradient(loss, params);
    // Some test nodes
    auto a = !grads[0];
    auto r = grads[0] > (grads[0] + 1);
    return GraphFunction(g, NodeVec{input}, grads);
}

md::GraphFunction simple_model(){
    auto g = create_graph();
    g->name = "simple";
//    auto a = new_sym();
    auto a = 10;
    auto input = g->matrix(md::f32, a, 12);
    auto p = g->parameter("p", md::f32, {a, 12, 1, 1});
    auto s1 = input + p;
    auto s2 = s1 + input;
    return GraphFunction(g, NodeVec{input}, NodeVec{s2});
}

int main(){
    md::gir::console_logging(true);
//    auto gf = build_model();
    auto gf = simple_model();
    std::ofstream f;
    f.open("test.html");
    md::cytoscape::export_graph(gf.graph, f);
    f.close();
    f.open("test.json");
    md::json::export_graph(gf.graph, f);
    f.close();
    auto backend = std::make_shared<mock::MockBackend>(false);
    auto in = mock::make_var(f32, std::array<long, 4> {10, 12, 1, 1});
    for(auto i=0; i<120; ++i){
        in->get<float>()[i] = 3.0;
    }
    auto func = backend->make_function(gf);
    mock::VarVec ins = {in}, outputs;
    std::tie(outputs, std::ignore) = func->eval(ins);
    std::cout << outputs.size() << std::endl;
    auto o0 = outputs[0];
    std::cout << "Shape: (" << o0->shape[0] << ", "
            << o0->shape[1] << ", "
            << o0->shape[2] << ", "
            << o0->shape[3] << ")" << std::endl << "Data:" << std::endl;
    auto ptr = o0->get<float>();
    for(auto i=0; i<o0->shape[0]; ++i){
        for(auto j=0; j<o0->shape[1]; ++j) {
            std::cout << ptr[j * o0->shape[0] + i] << ",";
        }
        std::cout << std::endl;
    }
    return 0;
}