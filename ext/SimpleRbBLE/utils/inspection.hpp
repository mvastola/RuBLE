#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "utils/human_type_names.hpp"
#include "utils/hexadecimal.hpp"

namespace SimpleRbBLE::Utils {
    namespace Inspection {
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

        [[maybe_unused]] std::string enum_val_inspect(const Enum_T &enumVal) {
            return enum_type_name(enumVal) + "::" + enum_val_as_string(enumVal);
        }

        // split out the closing '>' into its own function to make enhancing this method easier
        [[maybe_unused]] std::string basic_object_inspect_start(const Rice::Object &o);

//        template <HasRubyObject T>
//        std::string basic_object_inspect_start(T &t) {
//            Object obj = t.self();
//            return basic_object_inspect_start(obj);
//        }

        template<class T>
        // requires (!HasRubyObject<T>)
        constexpr std::string basic_object_inspect_start(T &t) {
            std::ostringstream oss;
            oss << "#<" << human_type_name<T>() << ":" << to_hex_addr<true, T>(t);
            return oss.str();
        }

        [[maybe_unused]] constexpr std::string basic_object_inspect(auto &o) {
            return basic_object_inspect_start(o) + ">";
        }
    } // Inspection
    using namespace Inspection;
} // SimpleRbBLE::Utils




template<typename Key, class ProxyClass, class Value>
constexpr std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::Registry<Key, ProxyClass, Value> &reg) {
    return os << reg.to_s();
}

template<const auto &FIELDS>
constexpr std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::NamedBitSet<FIELDS> &nbs) { return os << nbs.to_s(); }
