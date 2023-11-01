#include "RubyCallbackTraits.hpp"
#include "types/ruby.hpp"

namespace SimpleRbBLE::Utils::Ruby {
    CallbackTraits::CallbackTraits(const VALUE &val) {
        if (RB_NIL_OR_UNDEF_P(val) || rb_obj_is_proc(val) != Qtrue)
            throw std::invalid_argument("Argument must be a proc or lambda");

        for (const auto &key: Ruby::FnParameterTypes) _map[key] = 0;
        Rice::Array params = Object(val).call("parameters");
        for (auto el: params) {
            Array array = el.value();
            std::string type = rb_id2name(rb_sym2id(array[0].value()));
            ++_map[type];
        }
        _is_lambda = rb_proc_lambda_p(val) == Qtrue;
    }

    void CallbackTraits::assert_accepts_n_args(const VALUE &val, const uint8_t &n, bool allow_nil) {
        if (RB_NIL_OR_UNDEF_P(val)) {
            if (allow_nil) return;
            throw std::invalid_argument("Argument must be a proc or lambda (nil values not accepted)");
        }
        if (CallbackTraits(val).accepts_n_args(n)) return;
        throw std::invalid_argument("Argument must be callable with exactly "s +
                                    std::to_string(n) + " positional arguments.");
    }
}