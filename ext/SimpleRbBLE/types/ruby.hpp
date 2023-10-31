#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <ruby/ruby.h>
#include <type_traits>
#include <concepts>

namespace SimpleRbBLE {
    namespace Rice = ::Rice;
    using namespace ::Rice;

    template <typename T = void>
    using RbThreadCreateFn = VALUE(*)(T*);
    template <typename Ret = void, typename T = void>
    using RbThreadCallFn [[maybe_unused]] = Ret *(*)(T*);
    template <typename T = void>
    using RbUbfFn [[maybe_unused]] = void (*)(T*);


    template <class T>
    using ToRubyObjectResult = std::invoke_result_t<decltype(&T::self), const T&>;

    template <class T>
    concept ReturnsRubyValue = std::is_convertible_v<std::invoke_result_t<decltype(&T::value), const T&>, VALUE>;
    template <class T>
    concept RiceObject = std::derived_from<Rice::Object, T> && ReturnsRubyValue<T>;
    template <class T>
    concept RubyValue = RiceObject<T> || std::same_as<VALUE, T>;


    template<typename T>
    concept HasRubyObject = RubyValue<ToRubyObjectResult<T>> && requires { typename T::DataObject; };


//                         (std::derived_from<ArgT, Rice::Object> || std::same_as<VALUE, ArgT>)
}


