#pragma once
#include <chrono>
#include <optional>
#include <ranges>
#include <semaphore>
#include <type_traits>

namespace chrono = std::chrono;
namespace views = std::views;
namespace ranges = std::ranges;

namespace SimpleRbBLE {
    const constexpr auto semaphore_max = std::numeric_limits<uint16_t>::max();
    using Semaphore [[maybe_unused]] = std::counting_semaphore<semaphore_max>;

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

    template<class T, class R>
    concept RangeOf = requires(const R &r, T &t) {
        { *ranges::begin(r) } -> std::convertible_to<T>;
        { *ranges::end(r) } -> std::convertible_to<T>;
    };

    template<ranges::viewable_range RangeT>
    using RangeIteratorType = std::invoke_result_t<decltype(std::ranges::begin), RangeT &&>;
    template<ranges::viewable_range RangeT>
    using RangeElementType [[maybe_unused]] = std::invoke_result_t<decltype(RangeIteratorType<RangeT>::operator*), RangeIteratorType<RangeT> &&>;
}