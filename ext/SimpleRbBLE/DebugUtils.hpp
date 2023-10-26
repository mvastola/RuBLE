#pragma once

#include <string>
#include <typeinfo>
#include <any>

#include <rice/rice.hpp>
#include <rice/stl.hpp>

namespace SimpleRbBLE {
    namespace Utils {
        namespace Debug {
            // need to predeclare, since the templated fns call the fully specified version
            template<typename T>
            std::string human_type_name();

            template<typename T>
            std::string human_type_name(const T &);

            template<>
            std::string human_type_name(const std::type_info &type);

            template<>
            std::string human_type_name(const std::any &any);

            template<typename T>
            std::string human_type_name() { return human_type_name(typeid(T)); }

            template<typename T>
            std::string human_type_name(const T &) { return human_type_name<T>(); }

        };
        using namespace Debug;
    } // Utils
} // SimpleRbBLE

//using namespace SimpleRbBLE::Utils::Debug;