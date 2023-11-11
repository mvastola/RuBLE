#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>
#include <concepts>
#include <any>
#include <boost/core/demangle.hpp>
#include <iostream>

#include <rice/rice.hpp>
#include <rice/stl.hpp>


namespace RubBLE::Utils {
    namespace HumanTypeNames {
        using boost::core::demangle;

        // need to predeclare, since the templated fns call the fully specified version
        template<typename T>
        constexpr std::string human_type_name();

        template<typename T> //requires (!std::convertible_to<T, const std::type_info&>)
        constexpr std::string human_type_name(const T &) { return human_type_name<T>(); }

        template<typename = void>
        constexpr std::string human_type_name(const std::type_info &type) {
            return demangle(type.name());
        }

        template<typename T>
        constexpr std::string human_type_name() { return human_type_name(typeid(T)); }

        template<>
        [[maybe_unused]] constexpr std::string human_type_name(const std::any &any) { return human_type_name(any.type()); }
    }
    using namespace HumanTypeNames;
} // RubBLE::Utils
