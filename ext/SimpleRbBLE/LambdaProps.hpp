#pragma once

#include "types.hpp"
#include <unordered_set>
#include <string>
// Used to confirm callbacks accept the number of arguments needed for callbacks
// This is preferable to alerting when callbacks are invoked, where it would be
// hard to decipher what's wrong from the ArgumentError
// unfortunately, Proc#arity isn't sufficiently informative because it doesn't
// take into account required kw args.
namespace SimpleRbBLE {
    class LambdaProps {
        bool _is_lambda = false;
        std::unordered_map<std::string, uint_least8_t> _map;
    public:
        using MapValueType = decltype(_map)::mapped_type;
        static const inline std::unordered_set<std::string> ParameterTypes {
            "req", "opt", "rest", "keyreq", "key", "keyrest"
        };
//        enum class ParameterType { positional, keyword };
//        enum class Requiredness { required, optional, rest };
//
//        using Details = std::tuple<ParameterType, Requiredness>;
//        inline static const std::unordered_map<std::string, Details> ParameterInfoMap{
//                {"req",     {ParameterType::positional, Requiredness::required}},
//                {"opt",     {ParameterType::positional, Requiredness::optional}},
//                {"rest",    {ParameterType::positional, Requiredness::rest}},
//                {"keyreq",  {ParameterType::keyword,    Requiredness::required}},
//                {"key",     {ParameterType::keyword,    Requiredness::optional}},
//                {"keyrest", {ParameterType::keyword,    Requiredness::rest}},
//        };

        inline LambdaProps(const VALUE &val) {
            if (RB_NIL_OR_UNDEF_P(val) || rb_obj_is_proc(val) != Qtrue)
                throw std::invalid_argument("Argument must be a proc or lambda");

            for (const auto &key : ParameterTypes) _map[key] = 0;
            Rice::Array params = Object(val).call("parameters");
            for (auto el : params) {
                Array array = el.value();
                std::string type = rb_id2name(rb_sym2id(array[0].value()));
                ++_map[type];
            }
            _is_lambda = rb_proc_lambda_p(val) == Qtrue;
        }

        constexpr bool is_lambda() const { return _is_lambda; }
        constexpr bool is_proc() const { return !_is_lambda; }
        constexpr const MapValueType &operator[](const std::string &type) const {
            return _map.at(type);
        }

        // whether the lambda would accept exactly N positional args
        constexpr bool accepts_n_args(int n) const {
            if (!_is_lambda) return true;
            if (_map.at("keyreq") > 0) return false; // if any keyword arguments are required
            MapValueType min = _map.at("req");
            MapValueType max = _map.at("rest") > 0 ? std::numeric_limits<MapValueType>::max() : min + _map.at("opt");
            return min <= n && n <= max;
        }

        // throws an exception if the argument can't be called with exactly N positional args
        static inline void assert_accepts_n_args(const VALUE &val, const uint8_t &n, bool allow_nil = true) {
            if (RB_NIL_OR_UNDEF_P(val)) {
                if (allow_nil) return;
                throw std::invalid_argument("Argument must be a proc or lambda (nil values not accepted)");
            }
            if (LambdaProps(val).accepts_n_args(n)) return;
            throw std::invalid_argument("Argument must be callable with exactly "s +
                std::to_string(n) + " positional arguments.");
        }
    };
}