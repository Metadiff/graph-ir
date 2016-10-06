//
// Created by alex on 29/09/16.
//
#include "metadiff.h"
#include "iostream"

int main(){
    std::cout << md::core::CONSTANT_DERIVED << std::endl;
    md::utils::log_sink()->add_sink(std::make_shared<spdlog::sinks::stdout_sink_st>());
    auto log = md::utils::logger("try");
    log->info("AS");
    log->error("Hi there {} zzz-{}-", "Hi", 132);
    std::cout << md::core::host() << std::endl;
    std::cout << md::core::auto_infer << std::endl;
    return 0;
}
