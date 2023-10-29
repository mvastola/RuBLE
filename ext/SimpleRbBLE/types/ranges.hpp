#pragma once

#include <ranges>
#include <type_traits>
namespace views = std::views;
namespace ranges = std::ranges;

namespace SimpleRbBLE {
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