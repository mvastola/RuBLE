#pragma once
#include <concepts>

namespace std {
    struct nullopt_t; // NOLINT(*-dcl58-cpp)
}

namespace RuBLE::Utils {
    template<typename T>
    constexpr bool is_nullopt = std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;
}

