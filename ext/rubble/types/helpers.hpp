#pragma once
#include <concepts>

namespace std {
    struct nullopt_t; // NOLINT(*-dcl58-cpp)
}

namespace RubBLE::Utils {
    template<typename T>
    constexpr bool Nullopt = std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    // because exact, constexpr division with round up
    template<std::integral auto A, std::integral auto B,
            std::integral Res = std::common_type_t<decltype(A), decltype(B)>>
    constexpr const Res divmod_ceil = (static_cast<Res>(A) / static_cast<Res>(B)) +
                                      ((static_cast<Res>(A) % static_cast<Res>(B) == 0) ? 0 : 1);
}

