#pragma once

#include <version>
#include <string>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>
#include "types.h"
#include "ConvertableByteArray.h"

using namespace std::string_view_literals;
namespace SimpleRbBLE {
    namespace Utils {
        // need to predeclare, since the templated fns call the fully specified version
        template<typename T>
        std::string human_type_name();

        template<typename T>
        std::string human_type_name(const T &);

        template<>
        std::string human_type_name(const std::type_info &type);

        template<>
        std::string human_type_name(const std::any &any);

        template<typename T>
        std::string human_type_name() { return human_type_name(typeid(T)); }

        template<typename T>
        std::string human_type_name(const T &) { return human_type_name<T>(); }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        std::string enum_type_name(const Enum_T &enumVal) {
            Object rbEnumItem = Rice::detail::To_Ruby<Enum_T>().convert(enumVal);
            return rbEnumItem.class_name().str();
        }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        std::string enum_val_as_string(const Enum_T &enumVal) {
            Object rbEnumItem = Rice::detail::To_Ruby<Enum_T>().convert(enumVal);
            return Object(rbEnumItem).to_s().str();
        }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        std::string enum_val_inspect(const Enum_T &enumVal) {
            return enum_type_name(enumVal) + "::" + enum_val_as_string(enumVal);
        }

        std::string join(const ranges::viewable_range auto &range, const std::string_view &sep) {
            if (ranges::empty(range)) return "";
            if (ranges::size(range) == 1) return std::string(*ranges::cbegin(range));
            const auto fold = [&sep](const std::string_view &a, const std::string_view &b) {
                return std::string(a) + std::string(sep) + std::string(b);
            };
            return std::accumulate(std::next(ranges::cbegin(range)), ranges::cend(range),
                                   std::string(*ranges::cbegin(range)), fold);
        }

        // gdb doesn't seem to believe in constructing Objects, so hoping this helps
        std::string inspect_object(const Object &);

        std::string inspect_object(VALUE);

        template<class KeyT, class ValueT,
                class FnT = std::function<std::remove_reference_t<KeyT>(const std::remove_cvref_t<ValueT> &)>,
                class ResultT = std::map<std::remove_reference_t<KeyT>, std::remove_reference_t<ValueT>>,
                ranges::viewable_range RangeT>
        requires requires(FnT &&fnT, RangeT &&rangeT) {
            { fnT(**ranges::begin(rangeT)) } -> std::convertible_to<std::remove_reference_t<KeyT>>;
        }
        ResultT map_by(RangeT &&r, FnT &&mapFn) {
            auto makePair = [&mapFn](ValueT &val) -> auto {
                KeyT key = mapFn(*val);
                return std::make_pair(std::move(key), val);
            };
            auto pairView = views::transform(r, makePair);
            return {pairView.begin(), pairView.end()};
        }
    }
    using namespace Utils;
}