//
// Created by alex on 26/10/16.
//

#ifndef METADIFF_GRAPH_IR_API_H
#define METADIFF_GRAPH_IR_API_H

namespace md {
    namespace api {
        using namespace gir;

        inline Graph create_graph() {
            return std::make_shared<GraphInternal>();
        }

        inline Graph default_graph() {
            static std::shared_ptr<GraphInternal> graph;
            if (not graph) {
                graph = create_graph();
            }
            return graph;
        }
    }
}

#include "api/input.h"
#include "api/constant.h"
#include "api/special.h"
#include "api/shape.h"
#include "api/logical.h"
#include "api/arithmetic.h"
#include "api/reduction.h"
#include "api/elementwise.h"
#include "api/linalg.h"
#include "api/debug.h"
#include "api/optimized.h"
#include "api/grad.h"

#endif //METADIFF_GRAPH_IR_API_H
