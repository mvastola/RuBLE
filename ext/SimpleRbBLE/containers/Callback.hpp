#pragma once
#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "utils/CallableTraits.hpp"
#include "utils/garbage_collection.hpp"

using namespace std::string_literals;
namespace SimpleRbBLE {
    struct Callback {
    protected:
        std::size_t  _arg_count;
        VALUE _cb = Qnil;
    public:
        explicit Callback(const std::size_t &arg_count) : _arg_count(arg_count) {}

        // TODO: confirm this is a proc (taking the right number of args)
        constexpr void set(VALUE cb) {
            Utils::CallableTraits::assert_accepts_n_args(cb, _arg_count, true);
            _cb = cb;
        }

        [[maybe_unused]] [[nodiscard]] constexpr const std::size_t &arg_count() const { return _arg_count; }

        [[maybe_unused]] constexpr void clear() { set(Qnil); }

        template<class... Types>
        Object fire(const Types& ...args) {
            Object cb(_cb);
            if (!cb.test()) return Qnil;
            const std::initializer_list<Object> argList { args... };
            Rice::Array argArray(argList.begin(), argList.end());
            return cb.vcall("call", argArray);
        }

        constexpr operator bool() const {
            return _cb != Qnil && _cb != Qfalse;
        }

        constexpr void ruby_mark() const {
            if (*this) rb_gc_mark(_cb);
        }
    };
}

namespace Rice {
    template<>
    constexpr void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback *cb) { cb->ruby_mark(); }
}


