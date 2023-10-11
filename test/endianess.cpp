#include <iostream>
#include <iomanip>
#include <cstdint>
#include <span>
#include <algorithm>
template <typename T>
        constexpr T swap_endianness(const T &val) {
            auto in = std::as_bytes(std::span(&val, 1));
            T result;
            auto out = std::as_writable_bytes(std::span(&result, 1));
            std::copy(in.rbegin(), in.rend(), out.begin());
            return result;
        }
int main() {
    
	uint8_t a[4] = { 1, 2, 3, 4 };
	uint16_t *b = (uint16_t*)&a;

    for (std::size_t i = 0; i < 2; ++i) {
        //nums16[i] = *reinterpret_cast<uint16_t*>(&nums[i]);
				//nums16[i] = swap_endianness(nums16[i]);
        std::cout << std::hex << std::setw(0);
        std::cout << "(" << i << ") " << std::hex << std::to_string(a[2*i]) << " / ";
				std::cout << std::to_string(a[2*i+1]);
        std::cout << " => 0x" << std::setw(2) << std::hex << swap_endianness(b[i]) << std::endl;
    }

}

/*
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <span>
#include <algorithm>
template <typename T>
        constexpr T swap_endianness(const T &val) {
            auto in = std::as_bytes(std::span(&val, 1));
            T result;
            auto out = std::as_writable_bytes(std::span(&result, 1));
            std::copy(in.rbegin(), in.rend(), out.begin());
            return result;
        }
int main() {
    
	uint8_t a[2] = { 0, 255 };
	uint16_t *b = (uint16_t*)&a;
        std::cout << std::dec << std::setw(0);
        std::cout << "(" << 0 << ") " << (char)a[0] << "/" << (c;
        std::cout << " => " << std::setw(2) << std::hex << nums16[i] << std::endl;
    }

}
*/
