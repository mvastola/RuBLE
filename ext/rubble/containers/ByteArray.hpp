#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "types/helpers.hpp"
#include "utils/containers.hpp"
#include "utils/hexadecimal.hpp"
#include "utils/human_type_names.hpp"
#include "utils/endianness.hpp"
#include "concerns/Rubyable.hpp"
#include <utility>
#include <span>
#include <array>
#include <algorithm>
#include <compare>

namespace RubBLE {
    template<typename T>
    concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;

    // TODO: Need to test this out
    class ByteArray : public Rubyable<ByteArray> {
    public:
        using DataObject = ByteArray_DO;
    private:
        SimpleBLE::ByteArray _data;

    public:
        using char_type = SimpleBLE::ByteArray::value_type;
        using iterator_diff_type = typename std::iterator_traits<SimpleBLE::ByteArray::const_iterator>::difference_type;

        static constexpr const std::size_t char_size = sizeof(char_type);

        template<typename T>
        static constexpr const std::size_t chars_per_val = Utils::divmod_ceil<sizeof(T), char_size>;

        using default_integral_type = uint64_t;

        [[nodiscard]] constexpr ByteArray();
        constexpr ByteArray(SimpleBLE::ByteArray byteArray);

        ByteArray(Rice::Object obj);
        template<UnsignedIntegral T>
        ByteArray(const T &data) : ByteArray() {
            // TODO: modernize by using std::span
            _data.assign(chars_per_val<T>, '\0');
            UnsignedIntegral auto &dstVal = *reinterpret_cast<T *>(_data.data());
            dstVal = Utils::maybe_swap_endianness(data);
        }

        constexpr ByteArray(const char_type &chr);
        constexpr ByteArray(const bool &b);

        static ByteArray from_ruby(Rice::Object obj);

        [[nodiscard]] constexpr std::size_t length() const { return _data.size(); }
        [[nodiscard]] constexpr std::size_t byte_count() const { return length() * char_size; }

        template<UnsignedIntegral T>
        constexpr explicit operator T() const { return to_i<T>(); }

        template<UnsignedIntegral T = default_integral_type>
        constexpr T operator|(const T &other) const { return to_i<T>() | other; }

        template<UnsignedIntegral T = default_integral_type>
        constexpr T operator&(const T &other) const { return to_i<T>() & other; }

        template<UnsignedIntegral T = default_integral_type>
        constexpr T operator^(const T &other) const { return to_i<T>() ^ other; }

        template<UnsignedIntegral T = default_integral_type>
        constexpr T operator%(const T &other) const { return to_i<T>() % other; }

        constexpr bool operator!() const { return to_i<>() != 0; }

        constexpr auto operator~() const { return ~to_i<>(); }

        template<typename T = default_integral_type>
        constexpr int operator<=>(const T &other) const {
            std::strong_ordering result = to_i<T>() <=> other;
            if (result == std::strong_ordering::less) return -1;
            if (result == std::strong_ordering::greater) return 1;
            return 0;
        }

        constexpr std::strong_ordering operator<=>(const ByteArray &other) const {
            return _data <=> other._data;
        }
        constexpr bool operator==(const ByteArray &other) const {
            auto comparison = *this <=> other;
            return comparison == std::strong_ordering::equivalent || comparison == std::strong_ordering::equal;
        }

        constexpr bool operator!=(const ByteArray &other) const { return !(*this == other); }

        [[nodiscard]] constexpr auto as_bytes() const {
            return std::as_bytes(std::span(_data.data(), length()));
        }

        [[maybe_unused]] [[nodiscard]] constexpr auto as_bytes() {
            return std::as_writable_bytes(std::span(_data.data(), length()));
        }

        [[nodiscard]] constexpr std::string to_string() const { return _data; }
        [[nodiscard]] std::string to_s() const;
        [[nodiscard]] std::string inspect() const;

        [[nodiscard]] constexpr const SimpleBLE::ByteArray &data() const { return _data; }


        [[nodiscard]] constexpr const std::byte &operator[](iterator_diff_type pos) const {
            auto span = std::as_bytes(std::span(_data.data(), length()));
            return *std::next(span.cbegin(), pos);
        }

        constexpr operator const SimpleBLE::ByteArray &() const { return _data; }
        // TODO: test me!
        template<UnsignedIntegral T = default_integral_type>
        [[nodiscard]] T to_i() const {
            if (sizeof(T) < char_size * length()) {
                static constexpr const auto errfmt = "Integer type %s holds only %ld bytes, "\
                    "while %s contains %ld bytes of data. Result will be truncated.";
                rb_warn(errfmt, std::string(Utils::human_type_name<T>()).c_str(), sizeof(T),
                        std::string(Utils::human_type_name(*this)).c_str(), char_size * length());
            }
            T result = 0;
            // this makes sure we don't read past the end of _data
            auto in = as_bytes();
            auto out = std::as_writable_bytes(std::span(&result, 1));
            iterator_diff_type byte_size = std::min(byte_count(), sizeof(T));
            if constexpr (Utils::endianness_byteswap_needed) {
                // TODO: make sure this works right
                std::copy(in.rbegin(), std::next(in.rbegin(), byte_size), out.begin());
            } else {
                std::copy(in.begin(), std::next(in.begin(), byte_size), out.begin());
            }

            return result;
        }

        void ruby_mark() const;
    };

    constexpr ByteArray::ByteArray() : _data(), Rubyable<ByteArray>() {}

    constexpr ByteArray::ByteArray(SimpleBLE::ByteArray byteArray) :
            _data(std::move(byteArray)), Rubyable<ByteArray>() {}

    constexpr ByteArray::ByteArray(const ByteArray::char_type &chr) : ByteArray(std::string(chr, 1)) {}

    constexpr ByteArray::ByteArray(const bool &b) : ByteArray(b ? '1' : '0') {}


    void Init_ByteArray();
}

//namespace Rice::detail {
//    template<>
//    class From_Ruby<RubBLE::ByteArray> {
//    public:
//        static constexpr const auto supported_ruby_types =
//                std::to_array({T_BIGNUM, T_FIXNUM, T_SYMBOL, T_STRING, T_NIL, T_TRUE, T_FALSE});
//        static bool can_convert_from_ruby(VALUE obj);
//        bool is_convertible(VALUE value) { // NOLINT(*-convert-member-functions-to-static)
//            return can_convert_from_ruby(value);
//        }
//        RubBLE::ByteArray convert(VALUE value) { // NOLINT(*-convert-member-functions-to-static)
//            return RubBLE::ByteArray::from_ruby(value);
//        }
//    };
//}

constexpr std::ostream &operator<<(std::ostream &os, const RubBLE::ByteArray &cba) {
    return os << cba.data();
}
