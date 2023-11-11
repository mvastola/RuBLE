#pragma once
#include <ranges>
#include <functional>
#include <map>
#include <numeric>

namespace views = std::views;
namespace ranges = std::ranges;

namespace RuBLE::Utils {
    namespace Containers {
        constexpr std::string join(const ranges::viewable_range auto &range, const std::string_view &sep) {
            if (ranges::empty(range)) return "";
            if (ranges::size(range) == 1) return std::string(*ranges::cbegin(range));
            const auto fold = [&sep](const std::string_view &a, const std::string_view &b) {
                return std::string(a) + std::string(sep) + std::string(b);
            };
            return std::accumulate(std::next(ranges::cbegin(range)), ranges::cend(range),
                                   std::string(*ranges::cbegin(range)), fold);
        }

        template<class KeyT, class ValueT,
                class FnT = std::function<std::remove_reference_t<KeyT>(const std::remove_cvref_t<ValueT> &)>,
                class ResultT = std::map<std::remove_reference_t<KeyT>, std::remove_reference_t<ValueT>>,
                ranges::viewable_range RangeT> requires requires(FnT &&fnT, RangeT &&rangeT) {
                    { fnT(**ranges::begin(rangeT)) } -> std::convertible_to<std::remove_reference_t<KeyT>>;
                }
        [[maybe_unused]] ResultT map_by(RangeT &&r, FnT &&mapFn) {
            auto makePair = [&mapFn](ValueT &val) -> auto {
                KeyT key = mapFn(*val);
                return std::make_pair(std::move(key), val);
            };
            auto pairView = views::transform(r, makePair);
            return {pairView.begin(), pairView.end()};
        }

    } // Containers
    using namespace Containers;
} // RuBLE::Utils


