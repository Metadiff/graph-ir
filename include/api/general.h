//
// Created by alex on 27/10/16.
//

#ifndef METADIFF_API_GENERAL_H
#define METADIFF_API_GENERAL_H
#include "type_traits"
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

    namespace api{
        using namespace core;

        inline Graph default_graph(){
            static std::shared_ptr<GraphInternal> graph;
            if(not graph){
                graph = std::make_shared<GraphInternal>();
            }
            return graph;
        }

        bool check_independent(Node anchor, Node monitored);

        Axes validate_axes(Shape shape, Axes axes);

        Axes auto_infer_axes(Shape shape);
    }

}

#endif //METADIFF_API_GENERAL_H
