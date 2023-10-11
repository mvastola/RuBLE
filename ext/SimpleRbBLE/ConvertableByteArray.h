#pragma once

#include "types.h"
#include <utility>
#include <endian.h>
#include <span>
#include <array>
#include <algorithm>
#include <compare>

namespace SimpleRbBLE {
    template<typename T>
    constexpr bool is_unsigned_int_v = std::is_integral_v<T> && std::is_unsigned_v<T>;


    // TODO: Need to test this out
    class ConvertableByteArray {
        ByteArray _data;
        template<typename T>
        static constexpr const auto chars_per_val = sizeof(T) / sizeof(ByteArray::value_type);
    public:
        using ConvertableFrom = std::variant<
                ConvertableByteArray,
                ByteArray,
                uint64_t,
                char8_t,
                bool
                >;
        using default_integral_type = uint64_t;
        template <typename T>
        static constexpr T swap_endianness_if_le(const T &t) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
            // TODO: there has to be a better way to silence CLion's
            //  "This will always evaluate to false" warning
            if constexpr (std::endian::native == std::endian::big) return t;
#pragma clang diagnostic pop

            // From https://codereview.stackexchange.com/a/237565
            auto in = std::as_bytes(std::span(&t, 1));
            T result;
            auto out = std::as_writable_bytes(std::span(&result, 1));
            std::copy(in.rbegin(), in.rend(), out.begin());
            return result;
        }


//        template <std::size_t N, typename T> // FIXME: *UNTESTED*
//        static constexpr std::array<std::remove_cv_t<T>,N> swap_endianness_if_le(const T(&t)[N]) {
//            std::array<std::remove_cv_t<T>,N> result;
//            std::transform(&t[0], &t[N], result.begin(), swap_endianness_if_le<T>);
//            return result;
//        }

        ConvertableByteArray();
        constexpr ConvertableByteArray(ByteArray byteArray) : _data(std::move(byteArray)) {} // NOLINT(*-explicit-constructor)

        template <std::integral T = default_integral_type>
        constexpr ConvertableByteArray(const T &data) { // NOLINT(*-explicit-constructor)
            _data.assign(chars_per_val<T>, '\0');
            std::integral auto &dstVal = *reinterpret_cast<T*>(_data.data());
            dstVal = swap_endianness_if_le(data);
        }

        [[nodiscard]] const ByteArray &data() const;

        template<std::integral T = default_integral_type>
        [[nodiscard]] constexpr T to_i() const {
            assert(sizeof(T) >= sizeof(ByteArray::value_type));
            const std::integral auto &srcPtr = *reinterpret_cast<const T*>(_data.data());
            return swap_endianness_if_le(srcPtr);
        }
        [[nodiscard]] std::string to_s() const;

        template<std::integral T = default_integral_type>
        constexpr T operator+(const T &other) const { return to_i<T>() + other; }

        template<std::integral T = default_integral_type>
        constexpr T operator-(const T &other) const { return to_i<T>() - other; }

        template<std::integral T = default_integral_type>
        constexpr T operator*(const T &other) const { return to_i<T>() * other; }

        template<std::integral T = default_integral_type>
        constexpr T operator/(const T &other) const { return to_i<T>() / other; }

        template<std::integral T = default_integral_type>
        constexpr T operator%(const T &other) const { return to_i<T>() % other; }

        template<std::integral T = default_integral_type>
        constexpr T operator^(const T &other) const { return to_i<T>() ^ other; }

        template<std::integral T = default_integral_type>
        constexpr T operator&(const T &other) const { return to_i<T>() & other; }

        template<std::integral T = default_integral_type>
        constexpr T operator|(const T &other) const { return to_i<T>() | other; }

        constexpr auto operator~() const { return ~to_i<>(); }

        template<typename T = default_integral_type>
        constexpr int operator<=>(const T &other) const {
            std::strong_ordering result = to_i<T>() <=> other;
            if (result == std::strong_ordering::less) return -1;
            if (result == std::strong_ordering::greater) return 1;
            return 0;
        }

        template<typename T> requires std::is_integral_v<T>
        explicit constexpr operator T() const { return to_i<T>(); }

        constexpr operator default_integral_type() const { return to_i<default_integral_type>(); }

        constexpr operator const ByteArray &() const { return _data; } // NOLINT(*-explicit-constructor)
//        constexpr operator ByteArray &() { return _data; } // NOLINT(*-explicit-constructor)

//        template<typename T, std::size_t N> requires std::is_integral_v<T> // FIXME: *UNTESTED*
//        constexpr ConvertableByteArray(const T(&t)[N]) { // NOLINT(*-explicit-constructor)
//            // ensure type isn't too small
//            assert(sizeof(T) >= sizeof(ByteArray::value_type));
//            assert(N >= _data.size());
//            _data.assign(chars_per_val<T> * N, '\0');
//
//            const auto &dstArr = *reinterpret_cast<T(*)[N]>(_data.c_str());
//            dstArr = swap_endianness_if_le(t);
//        }
//        template<typename T = default_integral_type> requires std::is_integral_v<T> // FIXME: *UNTESTED*
//        constexpr auto to_array() const {
//            constexpr std::size_t N = _data.size() / chars_per_val<T> + (_data.size() % chars_per_val<T> == 0 ? 0 : 1);
//            std::array<T, N> result;
//
//            const auto &srcArr = *reinterpret_cast<const T(*)[N]>(_data.c_str());
//            return swap_endianness_if_le(srcArr);
//        }
//        template<std::size_t N, typename T = default_integral_type> requires std::is_integral_v<T> // FIXME: *UNTESTED*
//        constexpr std::array<T, N> to_a() const {
//                // ensure type isn't too small
//                assert(sizeof(T) >= sizeof(ByteArray::value_type));
//                assert(N * chars_per_val<T> >= _data.size());
//                const auto &srcArr = *reinterpret_cast<const T(*)[N]>(_data.c_str());
//                return swap_endianness_if_le(srcArr);
//        }
//        template<std::size_t N, typename T = default_integral_type> requires std::is_integral_v<T> // FIXME: *UNTESTED*
//        explicit constexpr operator std::array<T, N>() const { return to_a<N, T>(); }
    };
    void Init_ConvertableByteArray();
};

namespace Rice::detail {
    template<>
    class From_Ruby<SimpleRbBLE::ConvertableByteArray> {
    public:
        bool is_convertible(VALUE value) {
            return rb_type(value) == RUBY_T_FIXNUM || rb_type(value) == RUBY_T_STRING;
        }
        SimpleRbBLE::ConvertableByteArray convert(VALUE value) {
            if (rb_type(value) == RUBY_T_FIXNUM) {
                return { From_Ruby<uint64_t>{}.convert(value) };
            } else if (rb_type(value) == RUBY_T_STRING) {
                return { From_Ruby<std::string>{}.convert(value) };
            } else {
                throw std::invalid_argument("Could not convert value to SimpleRbBLE::ConvertableByteArray");
            }
        }
    };
}
std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::ConvertableByteArray &cba);