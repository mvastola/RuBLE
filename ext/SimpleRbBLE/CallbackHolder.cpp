#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
#include "SimpleRbBLE.h"
#include "CallbackHolder.h"

CallbackHolder_DT rb_cCallbackHolder;

void Init_CallbackHolder() {
    rb_cCallbackHolder = define_class_under<CallbackHolder>(rb_mSimpleRbBLE, "CallbackHolder")
            .define_constructor(Constructor<CallbackHolder>())
            .define_method("set", &CallbackHolder::set)
            .define_method("fire", &CallbackHolder::fire);
}

void CallbackHolder::set(Object cb) { _cb = cb; }

void CallbackHolder::fire() const {
    if (_cb.test() && !_cb.is_nil()) {
        std::cout << "Fire!!" << std::endl << std::flush;
        _cb.call("call");
    } else {
        std::cout << "Could not Fire: no cb set" << std::endl << std::flush;
    }
}

#pragma clang diagnostic pop