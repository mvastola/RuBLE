#pragma once

#include <ruby/ruby.h>
#include <unordered_set>
#include <unordered_map>
#include <string>

// Used to confirm callbacks accept the number of arguments needed for callbacks
// This is preferable to alerting when callbacks are invoked, where it would be
// hard to decipher what's wrong from the ArgumentError
// unfortunately, Proc#arity isn't sufficiently informative because it doesn't
// take into account required kw args.
namespace RuBLE::Utils::Ruby {
    static constexpr std::initializer_list<std::string_view> FnParameterTypes {
        "req", "opt", "rest", "keyreq", "key", "keyrest"
    };

    class CallbackTraits {
        bool _is_lambda = false;
        std::unordered_map<std::string_view, uint_least8_t> _map;
    public:
        using MapValueType = decltype(_map)::mapped_type;

        CallbackTraits(const VALUE &val);

        [[maybe_unused]] constexpr bool is_lambda() const { return _is_lambda; }

        [[maybe_unused]] constexpr bool is_proc() const { return !_is_lambda; }
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
        static void assert_accepts_n_args(const VALUE &val, const uint8_t &n, bool allow_nil = true);
    };
}
namespace RuBLE {
    namespace Ruby = Utils::Ruby;
}