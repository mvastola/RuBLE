#pragma once
#include "types.hpp"
#include "utils.hpp"
#include "LambdaProps.hpp"

namespace Rice {
    template<> constexpr void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback*);
}

using namespace std::string_literals;
namespace SimpleRbBLE {
    struct Callback {
    protected:
        std::size_t  _arg_count;
        VALUE _cb = Qnil;
    public:
//        Callback() = delete;
        explicit Callback(const std::size_t &arg_count) : _arg_count(arg_count) {}

        // TODO: confirm this is a proc (taking the right number of args)
        constexpr void set(VALUE cb) {
            LambdaProps::assert_accepts_n_args(cb, _arg_count, true);
            _cb = cb;
        }

        [[nodiscard]] constexpr const std::size_t &arg_count() const { return _arg_count; }
        constexpr void clear() { set(Qnil); }

        template<class... Types>
        Object fire(const Types& ...args) {
            Object cb(_cb);
            if (!cb.test()) return Qnil;
            const std::initializer_list<Object> argList { args... };
            Rice::Array argArray(argList.begin(), argList.end());
            return cb.vcall("call", argArray);
        }

        constexpr operator bool() const { // NOLINT(*-explicit-constructor)
            return _cb != Qnil && _cb != Qfalse;
        }

        constexpr void ruby_mark() const {
            if (*this) rb_gc_mark(_cb);
        }

        template<typename T>
        friend void Rice::ruby_mark(T *);
    };
}


namespace Rice {
    template<> constexpr void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback *cb) { // NOLINT(*-inconsistent-declaration-parameter-name)
        cb->ruby_mark();
    }
}
