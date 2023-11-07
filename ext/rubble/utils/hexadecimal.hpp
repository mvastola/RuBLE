#pragma once

#include "types/declarations.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <type_traits>
#include <format>
#include <ranges>

namespace Rubble::Utils {
    namespace Hexadecimal {
        namespace ranges = std::ranges;
        namespace views = std::views;
        using namespace std::string_view_literals;

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
        [[maybe_unused]] std::string to_hex_data(const T &t);
        template<bool prefix, class T>
        [[maybe_unused]] std::string to_hex_data(const T &t);
        // End Testing

        std::string byte_to_hex(const std::byte &byte);

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
        [[maybe_unused]] std::string to_hex_data(const T &t) { return range_to_hex_data(t); }
        template<bool prefix = true, class T>
        [[maybe_unused]] std::string to_hex_data(const T &t) { return ref_to_hex_data(t); }
    } // Hexadecimal
    using namespace Hexadecimal;
} // Rubble::Utils

