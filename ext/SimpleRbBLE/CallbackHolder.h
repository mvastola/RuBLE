#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
using namespace Rice;

struct CallbackHolder {
    Rice::Object _cb = Qnil;

    constexpr CallbackHolder() = default;
    void set(Rice::Object);
    void clear();
//    void fire(Rice::Object);
    template<typename Ret, class... Types> Ret fire(Types&...);
    operator bool() const; // NOLINT(*-explicit-constructor)
};

extern void Init_CallbackHolder();
