#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
#include "SimpleRbBLE.h"
#include "CallbackHolder.h"

CallbackHolder_DT rb_cCallbackHolder;

void Init_CallbackHolder() {
    rb_cCallbackHolder = define_class_under<CallbackHolder>(rb_mSimpleRbBLE, "CallbackHolder")
            .define_constructor(Constructor<CallbackHolder>())
            .define_method("set", &CallbackHolder::set)
            .define_method("clear", &CallbackHolder::clear)
//            .define_method("fire", &CallbackHolder::fire)
            ;
}

void CallbackHolder::set(Object cb) { _cb = cb; } // NOLINT(*-unnecessary-value-param)

CallbackHolder::operator bool() const {
    return _cb.test() && !_cb.is_nil();
}

void CallbackHolder::clear() {
    _cb = Qnil;
}

template<typename Ret, class... Types>
Ret CallbackHolder::fire(Types &...types) {
    if (!(*this)) {
        std::cout << "Could not Fire: no cb set" << std::endl << std::flush;
        return;
    }

    std::cout << "Fire!!" << std::endl << std::flush;
    _cb.call("call", types...); // NOLINT(*-unnecessary-value-param)
}

#pragma clang diagnostic pop