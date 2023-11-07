#pragma once

#include <endian.h>
#include <span>
#include <algorithm>

namespace Rubble::Utils {
    // TODO: I'm not 100% on if this code is correct. In any case it might be simpler to use the boost library
    //   (https://www.boost.org/doc/libs/1_83_0/libs/endian/doc/html/endian.html#conversion) also, is this really even
    //   needed? I'm thinking maybe we should take this out unless it becomes a problem/request.
    //   I haven't come across endianness being handled elsewhere in the ruby or SimpleBLE source, so (assuming I haven't
    //   missed anything) maybe there's a standard for bluetooth devices, or they're all proprietary in terms of encoding,
    //   or maybe ruby doesn't even play well with endianness changes anywhere
    namespace Endianness {
        constexpr const bool endianness_byteswap_needed = std::endian::native == std::endian::big; // NOLINT(clion-simplify)

        template<typename T>
        static constexpr T maybe_swap_endianness(const T &t) {
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
            if constexpr (!endianness_byteswap_needed) return t; // NOLINT(clion-simplify)
            // Based on https://codereview.stackexchange.com/a/237565
            auto span = std::as_writable_bytes(std::span(&t, 1));
            std::reverse(span.begin(), span.end());
        }
    }
    using namespace Endianness;
}