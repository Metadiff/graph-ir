//
// Created by alex on 25/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        Node GraphInternal::random_uniform(Shape shape) {
            Operator op = std::make_shared<op::RandomUniform>(this, shape);
            return derived_node(op);
        }

        Node GraphInternal::random_normal(Shape shape) {
            Operator op = std::make_shared<op::RandomNormal>(this, shape);
            return derived_node(op);
        }
    }
}

