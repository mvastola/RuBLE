#include "NamedBitSet.h"

#ifdef DEBUG
namespace SimpleRbBLE {
    static constexpr std::string_view characteristic_capabilities[] = {"read", "write_request", "write_command", "notify", "indicate"};

    using NBS = SimpleRbBLE::NamedBitSet<characteristic_capabilities>;
    using Map = FlagCheck::MapType<int>;

    void NamedBitSetDemo() {

        Map flag_fn_map {
                { "read", [](const int &i) -> bool { return i % 2 == 0; } },
                { "indicate", [] {return true;} }
        };

        std::cout << "Fields: ";
        for (const auto &f : NBS::FIELDS ) std::cout << f << ", ";
        std::cout << std::endl;
        std::cout << "Indices: ";
        for (const auto &[k,v] : NBS::PAIRS ) std::cout << k << ": " << v << ", ";
        std::cout << std::endl;

        auto nums = {1,2};
        for (const auto &i : nums) {
//            std::cout << "Value: " << i << std::endl;
            auto result = NBS(flag_fn_map, i);
            std::cout << "Value: " << i << " -- " << result << std::endl;
        }
    }
}

#endif
