//
// Created by alex on 06/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core {
        Node GraphInternal::constant(double value, dataType data_type, Shape shape) {
            // Limit the data type based on the max allowed
            dataType limit = limit_type(data_type);
            if(limit != data_type){
                auto err = std::make_shared<TypePromotion>(NodeVec{}, "ConstantValue", data_type, limit);
                operate_policy(promotion_err_policy, logger(), err);
            }
            // Standard
            return derived_node(std::make_shared<op::ConstantValue>(this, value, limit, shape));
        }

        Node GraphInternal::PI() {
            return constant(M_PI, max_float);
        }
        Node GraphInternal::E(){
            return constant(M_E, max_float);
        }

        Node GraphInternal::LN_2(){
            return constant(M_LN2, max_float);
        }

        Node GraphInternal::LN_10(){
            return constant(M_LN10, max_float);
        }

        Node GraphInternal::zeros(Shape shape, dataType data_type){
            return constant(0.0, data_type, shape);
        }

        Node GraphInternal::zeros(Shape shape){
            return constant(0.0, max_float, shape);
        }

        Node GraphInternal::ones(Shape shape, dataType data_type){
            return constant(1.0, data_type, shape);
        }

        Node GraphInternal::ones(Shape shape){
            return constant(1.0, max_float, shape);
        }

        Node GraphInternal::make_constant(Node node){
            // If already a constant do nothing
            if(node->node_type == CONSTANT or node->node_type == CONSTANT_DERIVED){
                return alias(node);
            }
            // Standard
            return apply<op::MakeConstant>(node);
        }

        Node GraphInternal::range(SymInt start, SymInt end, dataType data_type){
            auto op = std::make_shared<op::Range>(this, start, end, data_type);
            return derived_node(op);
        }

        Node GraphInternal::range(SymInt start, SymInt end){
            return range(start, end, max_int);
        }

        Node GraphInternal::eye(SymInt size, dataType data_type){
            auto op = std::make_shared<op::Eye>(this, size, data_type);
            return derived_node(op);
        }

        Node GraphInternal::eye(SymInt size){
            return eye(size, max_float);
        }
    }
}