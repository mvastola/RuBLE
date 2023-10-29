#pragma once

#include <concepts>
#include <type_traits>
#include <span>
#include "types/declarations.hpp"
#include "types/SimpleBLE.hpp"

namespace SimpleRbBLE {

    template<typename T>
    concept CanSpanBytes = requires (const T &t) { std::as_bytes(t); };

    template<typename T>
    concept Span = std::same_as<std::remove_cvref_t<T>, std::span<typename T::element_type, T::extent>>;

    template<typename T>
    concept Byte = std::same_as<std::remove_cvref_t<T>, std::byte> ||
    std::same_as<std::remove_cvref_t<T>, std::underlying_type_t<std::byte>>;
}
