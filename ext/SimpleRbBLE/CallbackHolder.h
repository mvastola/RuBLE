#pragma once

#include <rice/rice.hpp>
using namespace Rice;

struct CallbackHolder {
    Rice::Object _cb = Qnil;

    CallbackHolder() = default;
    void set(Rice::Object cb);
    void fire() const;

};

extern void Init_CallbackHolder();
