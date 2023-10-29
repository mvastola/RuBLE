#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>
#include <concepts>
#include <any>
#include <cxxabi.h>
#include <iostream>

#include <rice/rice.hpp>
#include <rice/stl.hpp>


namespace SimpleRbBLE::Utils {
    namespace HumanTypeNames {
        // need to predeclare, since the templated fns call the fully specified version
        template<typename T>
        constexpr std::string human_type_name();

        template<typename T> //requires (!std::convertible_to<T, const std::type_info&>)
        constexpr std::string human_type_name(const T &) { return human_type_name<T>(); }

        template<typename = void>
        constexpr std::string human_type_name(const std::type_info &type) {
            int status;
            std::string original_name = type.name();
            std::string result;
            char *result_ptr = abi::__cxa_demangle(original_name.c_str(), nullptr, nullptr, &status);

            if (result_ptr == nullptr || status) {
                std::cerr << "Warning. Demangling of type name " << original_name;
                std::cerr << " failed with status " << status << std::endl;
                result = original_name;
            } else {
                result = result_ptr;
            }
            if (result_ptr != nullptr) std::free(result_ptr);
            return result;
        }

        template<typename T>
        constexpr std::string human_type_name() { return human_type_name(typeid(T)); }

        template<>
        [[maybe_unused]] constexpr std::string human_type_name(const std::any &any) { return human_type_name(any.type()); }
    }
    using namespace HumanTypeNames;
} // SimpleRbBLE::Utils
