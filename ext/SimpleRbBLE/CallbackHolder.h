#pragma once

#include <rice/rice.hpp>
#include <rice/stl.hpp>
using namespace Rice;

struct CallbackHolder {
    Rice::Object _cb;

    CallbackHolder();
    void set(Rice::Object);
    void clear();
    operator bool() const; // NOLINT(*-explicit-constructor)
    template<class... Types> void fire(Types& ...types) {
        if (!(*this)) {
            std::cout << "Could not Fire: no cb set" << std::endl << std::flush;
            return;
        }

        std::cout << "Fire!!" << std::endl << std::flush;
        _cb.call("call", types...); // NOLINT(*-unnecessary-value-param)
    }

};

