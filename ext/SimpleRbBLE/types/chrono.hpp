#pragma once
#include <chrono>
#include <optional>
#include <type_traits>

namespace chrono = std::chrono;

namespace SimpleRbBLE {
    template<typename T>
    struct is_duration {
        constexpr static const bool value = false;
    };

    template<typename ...Types>
    struct is_duration<std::chrono::duration<Types...>> {
        constexpr static const bool value = true;
    };
    template<typename T>
    concept Duration = is_duration<T>::value;

    template<class T>
    concept OptionalDuration = Duration<T> || std::same_as<std::nullopt_t, T>;

}