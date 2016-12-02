//
// Created by alex on 29/09/16.
//

#ifndef METADIFF_GRAPH_IR_H
#define METADIFF_GRAPH_IR_H

// External includes
#include "type_traits"
#include "string"
#include "symbolic_integers.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/dist_sink.h"
#include "boost/config.hpp"
#include "boost/filesystem.hpp"
#include "fmt/format.h"
#include "dlfcn.h"

namespace md{
    template<class...> struct disjunction : std::false_type { };
    template<class B1> struct disjunction<B1> : B1 { };

    template<class B1, class... Bn>
    struct disjunction<B1, Bn...>
            : std::conditional<B1::value != false, B1, disjunction<Bn...>>  { };

    template<class...> struct conjunction : std::true_type { };
    template<class B1> struct conjunction<B1> : B1 { };
    template<class B1, class... Bn>
    struct conjunction<B1, Bn...>
            : std::conditional<B1::value != false, conjunction<Bn...>, B1>  {};
}

#include "enums.h"
#include "definitions.h"
#include "props.h"
//#include "shared.h"
#include "exceptions.h"
#include "export.h"
#include "node.h"
#include "utils.h"
#include "print.h"
#include "graph.h"
#include "api.h"
#include "operators.h"
#include "backend.h"
#include "mock.h"

#endif //METADIFF_GRAPH_IR_H
