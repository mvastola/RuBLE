#include "hexadecimal.hpp"

namespace Rubble {
    namespace Utils {
        namespace Hexadecimal {
            std::string byte_to_hex(const std::byte &byte) {
                std::string hex_str(3, '\0');
                std::to_chars(&*hex_str.begin(), &*hex_str.end(), std::to_integer<uint8_t>(byte), 16);
                hex_str.erase(ranges::find(hex_str, '\0'), hex_str.end());
                hex_str.insert(0, "0", 0, hex_str.size() - 2);
                return hex_str;
            }
        } // Hexadecimal
    } // Utils
} // Rubble