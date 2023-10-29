#pragma once

#include <endian.h>
#include <span>
#include <algorithm>

namespace SimpleRbBLE::Utils {
    namespace Endianness {
        constexpr const bool endianness_byteswap_needed = std::endian::native == std::endian::big; // NOLINT(clion-simplify)

        template<typename T>
        static constexpr T maybe_swap_endianness(const T &t) {
            // TODO: there has to be a better way to silence CLion's
            //  "This will always evaluate to false" warning
            if constexpr (!endianness_byteswap_needed) return t; // NOLINT(clion-simplify)
            // From https://codereview.stackexchange.com/a/237565
            auto in = std::as_bytes(std::span(&t, 1));
            T result;
            auto out = std::as_writable_bytes(std::span(&result, 1));
            std::copy(in.rbegin(), in.rend(), out.begin());
            return result;
        }

        template<typename T>
        [[maybe_unused]] static constexpr void maybe_swap_endianness_inplace(T &t) {
            // TODO: there has to be a better way to silence CLion's
            //  "This will always evaluate to false" warning
            if constexpr (!endianness_byteswap_needed) return t; // NOLINT(clion-simplify)
            // Based on https://codereview.stackexchange.com/a/237565
            auto span = std::as_writable_bytes(std::span(&t, 1));
            std::reverse(span.begin(), span.end());
        }
    }
    using namespace Endianness;
}