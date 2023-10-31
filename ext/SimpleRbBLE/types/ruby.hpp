#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <ruby/ruby.h>

namespace SimpleRbBLE {
    namespace Rice = ::Rice;
    using namespace ::Rice;

    template <typename T = void>
    using RbThreadCreateFn = VALUE(*)(T*);
    template <typename Ret = void, typename T = void>
    using RbThreadCallFn [[maybe_unused]] = Ret *(*)(T*);
    template <typename T = void>
    using RbUbfFn [[maybe_unused]] = void (*)(T*);

    template<typename T>
    concept HasRubyObject = requires(const T &t) {
        { t.self() } -> std::derived_from<Rice::Object>;
    } || requires(const T &t) {
        { t.self() } -> std::same_as<const VALUE>;
    };
}


