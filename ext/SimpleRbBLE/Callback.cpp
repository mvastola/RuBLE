#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"

#include "common.h"
#include "Callback.h"
#include "Peripheral.h"
#include "ruby/thread.h"

namespace SimpleRbBLE {

//    Callback::Callback()  {
//#ifdef DEBUG
//        VALGRIND_PRINTF_BACKTRACE("Instantiating callback at address 0x%lxd", // (ruby object id %lu)",
//                                  reinterpret_cast<uint64_t>(this));
//#endif
//    }
//    Callback::~Callback() {
//#ifdef DEBUG
//        VALGRIND_PRINTF_BACKTRACE("Destructing callback at address 0x%lxd", // (ruby object id %lu)",
//                                  reinterpret_cast<uint64_t>(this));
//#endif
////                              self().object_id());
//    }

    void Callback::set(Object cb) {
//        if (Object(_cb).test()) rb_gc_unregister_address(&_cb);
        _cb = cb;
//        if (Object(_cb).test()) rb_gc_register_address(&_cb);
    }

    Callback::operator bool() const { return _cb != Qnil; }

    void Callback::clear() { set(Qnil); }


//    Object Callback::self() const {
//        return _self;
//        throw std::runtime_error("Callback class must be constructed via CallbackWrapper");
//    }
}

namespace Rice {
    template<> void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback *cb) {
//        rb_gc_mark(cb->self());
        if (cb->_cb) rb_gc_mark(cb->_cb);
    }
}

#pragma clang diagnostic pop
