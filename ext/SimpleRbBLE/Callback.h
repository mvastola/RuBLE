#pragma once

#include "common.h"

namespace SimpleRbBLE {
    struct Callback {
    protected:
        VALUE _cb = Qnil;
    public:
        void set(Object);
        void clear();

        operator bool() const; // NOLINT(*-explicit-constructor)
        template<class... Types>
        void fire(Types ...args) {
            if (!_cb || _cb == Qnil) {
//                std::cout << "Could not Fire: no cb set" << std::endl << std::flush;
                return;
            }
            const std::initializer_list<Object> argList { args... };
            Rice::Array argArray(argList.begin(), argList.end());
//            std::cout << "In Callback::fire" << std::endl << std::flush;
//            std::cout << "Callback: " << Object(_cb).inspect() << std::endl << std::flush;
//            std::cout << "Argument(s): " << Object(argArray).inspect() << std::endl << std::flush;
//            std::cout << "Fire!! " << std::endl << std::flush;
            Object(_cb).vcall("call", argArray); // NOLINT(*-unnecessary-value-param)
        }

        template<typename T>
        friend void Rice::ruby_mark(T *);
    };
}

namespace Rice {
    template<> void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback*);
}

