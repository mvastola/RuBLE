#pragma once
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include "./types.hpp"

#include "DebugUtils.hpp"
#include <string>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <type_traits>
#include <format>
#include <ranges>

namespace views = std::views;
namespace ranges = std::ranges;
using namespace std::string_view_literals;

namespace SimpleRbBLE {
    namespace Utils {
        namespace Strings {

            constexpr const auto hex_string_prefix = "0x"sv;
            constexpr const auto hex_prefix_len = hex_string_prefix.length();

            // Testing:
            template <bool prefix, std::integral T>
            std::string to_hex_string(const T &num);
            template <bool prefix, typename T> requires std::is_pointer_v<T>
            std::string to_hex_addr(const T &ptr);
            template <bool prefix, typename T>
            std::string to_hex_addr(const std::shared_ptr<T> &ptr);
            template <bool prefix, typename T> requires std::is_base_of_v<Rice::Object, T>
            std::string to_hex_addr(const T &obj);
            template <bool prefix, typename T>
            std::string to_hex_addr(const T &obj);
            template<bool prefix, ranges::viewable_range T>
            std::string to_hex_data(const T &t);
            template<bool prefix, class T>
            std::string to_hex_data(const T &t);
            // End Testing

            inline std::string byte_to_hex(const std::byte &byte) {
                std::string hex_str(3, '\0');
                std::to_chars(&*hex_str.begin(), &*hex_str.end(), std::to_integer<uint8_t>(byte), 16);
                hex_str.erase(ranges::find(hex_str, '\0'), hex_str.end());
                hex_str.insert(0, "0", 0, hex_str.size() - 2);
                return hex_str;
            }

            template <bool prefix = true, std::integral T = void>
            std::string ref_to_hex_string(const T &num) {
                constexpr std::size_t result_len = std::max(std::size_t(2), sizeof (T) * 2) + (prefix ? 2 : 0);
                constexpr const auto fmt_str = prefix ? "{:#0{}x}" : "{:0{}x}";
                return std::format(fmt_str, num, result_len);
            }

            template <bool prefix = true, typename T = void> requires std::is_pointer_v<T>
            std::string ptr_to_hex_addr(const T &ptr) {
                return to_hex_string<prefix>(reinterpret_cast<uintptr_t>(std::to_address(ptr)));
            }

            template <bool prefix = true, typename T = void>
            std::string sptr_to_hex_addr(const std::shared_ptr<T> &ptr) {
                return to_hex_addr<prefix>(ptr.get());
            }

            template <bool prefix = true, typename T = void> requires std::is_base_of_v<Rice::Object, T>
            std::string rb_to_hex_addr(const T &obj) {
                return to_hex_string<prefix>(obj.value());
            }

            template <bool prefix = true, typename T = void>
            std::string ref_to_hex_addr(const T &obj) {
                return to_hex_addr<prefix>(&obj);
            }

            template<bool prefix = true, ranges::viewable_range T = void>
            std::string range_to_hex_data(const T &t) {
                std::size_t byte_count = ranges::size(t);
                using char_pair = str_char_type[2];
                using char_span = std::span<str_char_type>;
                using char_pairs_span = std::span<char_pair>;
                constexpr std::size_t prefix_len = prefix ? hex_prefix_len : 0;
                constexpr auto null_byte = static_cast<std::byte>(0);
                constexpr auto is_null = [](const std::byte &b) { return null_byte == b; };

                // we can't simply use `std::as_bytes(t) | views::transform | views::join` due to
                // https://en.cppreference.com/mwiki/index.php?title=cpp/ranges/join_view&oldid=155702#Notes

                std::string output = prefix ? std::string(hex_string_prefix) : "";
                auto span = std::span(t.begin(), t.end());
                auto in = std::as_bytes(span) | views::drop_while(is_null) |
                          views::transform(byte_to_hex);

                auto out = ranges::owning_view(std::move(in)) | views::join;

//            ranges::move(in_joined, std::inserter(output, output.end()));
                std::size_t moved_hex_pairs = output.length() - prefix_len;

                const std::string null_as_hex = byte_to_hex(null_byte);
                if (moved_hex_pairs == 0) output.append(null_as_hex);
                return output;
            }

            template<bool prefix = true, class T>
            std::string ref_to_hex_data(const T &t) {
                auto span = std::span(&t, 1);
                return to_hex_data<prefix, decltype(span)>(span);
            }

            // Testing:
            template <bool prefix = true, std::integral T = void>
            std::string to_hex_string(const T &num) { return ref_to_hex_string(num); }
            template <bool prefix = true, typename T = void> requires std::is_pointer_v<T>
            std::string to_hex_addr(const T &ptr) { return ptr_to_hex_addr(ptr); }
            template <bool prefix = true, typename T = void>
            std::string to_hex_addr(const std::shared_ptr<T> &ptr) { return sptr_to_hex_addr(ptr); }
            template <bool prefix = true, typename T = void> requires std::is_base_of_v<Rice::Object, T>
            std::string to_hex_addr(const T &obj) { return rb_to_hex_addr(obj); }
            template <bool prefix = true, typename T = void>
            std::string to_hex_addr(const T &obj) { return ref_to_hex_addr(obj); }
            template<bool prefix = true, ranges::viewable_range T>
            std::string to_hex_data(const T &t) { return range_to_hex_data(t); }
            template<bool prefix = true, class T>
            std::string to_hex_data(const T &t) { return ref_to_hex_data(t); }

            std::string ruby_inspect(Object obj);

            std::string ruby_quote_string(const std::string &str);

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

            // split out the closing '>' into its own function to make enhancing this method easier
            std::string basic_object_inspect_start(const Object &o);

//        template <HasRubyObject T>
//        std::string basic_object_inspect_start(T &t) {
//            Object obj = t.self();
//            return basic_object_inspect_start(obj);
//        }

            template <class T> // requires (!HasRubyObject<T>)
            constexpr std::string basic_object_inspect_start(T &t) {
                std::ostringstream oss;
                oss << "#<" << human_type_name<T>() << ":" << to_hex_addr<true, T>(t);
                return oss.str();
            }

            constexpr std::string basic_object_inspect(auto &o) {
                return basic_object_inspect_start(o) + ">";
            }
        };
        using namespace Strings;

    } // Utils
} // SimpleRbBLE

