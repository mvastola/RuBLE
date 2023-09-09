#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
using namespace Rice;

struct CallbackHolder {
    Rice::Object _cb = Qnil;

    constexpr CallbackHolder() = default;
    void set(Rice::Object);
    void fire(Rice::Object);
    operator bool() const; // NOLINT(*-explicit-constructor)
    //    template<typename Ret, class... Types>
    //    Ret fire(const Types&...) const;
};

extern void Init_CallbackHolder();
