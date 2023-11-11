#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "types/SimpleBLE.hpp"
#include "utils/human_type_names.hpp"
#include "utils/hexadecimal.hpp"
#include <boost/describe.hpp>
#include <boost/describe/enum.hpp>
#include <boost/describe/enum_to_string.hpp>
#include <boost/describe/enumerators.hpp>
#include <boost/describe/modifier_description.hpp>

namespace SimpleBLE {
    BOOST_DESCRIBE_ENUM(
        BluetoothAddressType,
        PUBLIC,
        RANDOM,
        UNSPECIFIED
    )
}
using boost::describe::boost_enum_descriptor_fn;

namespace RuBLE::Utils {
    namespace Inspection {
        std::string ruby_inspect(Object obj);

        std::string ruby_quote_string(const std::string &str);

        // From https://www.boost.org/doc/libs/1_83_0/libs/describe/doc/html/describe.html#example_printing_enums_rt
        template<class Enum_T> requires std::is_enum_v<Enum_T>
        struct enumerator_trait { const Enum_T value; const std::string name; };

        template<class Enum_T> requires std::is_enum_v<Enum_T>
        constexpr auto enumerator_traits_as_array() {
            constexpr auto impl = []<template<class... T> class L, class... T>( L<T...> ) ->
                    std::array<enumerator_trait<Enum_T>, sizeof...(T)> {
                return { { { T::value, T::name }... } };
            };
            return impl(boost::describe::describe_enumerators<Enum_T>());
        }

        template<typename Enum_T> requires std::is_enum_v<Enum_T>
        Rice::Enum<Enum_T> define_described_enum(char const* name, Rice::Module &module) {
            using Enum_DT = Rice::Enum<Enum_T>;
            Enum_DT rb_type = define_enum<Enum_T>(name, module);

            const auto traits = enumerator_traits_as_array<Enum_T>();
            for (const enumerator_trait<Enum_T> &item : traits) rb_type.define_value(item.name, item.value);
            return rb_type;
        }

        template<typename Enum_T> requires std::is_enum_v<Enum_T>
        Rice::Enum<Enum_T> define_described_enum(Rice::Module &module) {
            return define_described_enum(human_type_name<Enum_T>(), module);
        }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        std::string enum_rb_type_name(const Enum_T &enumVal) {
            Object rbEnumItem = Rice::detail::To_Ruby<Enum_T>().convert(enumVal);
            return rbEnumItem.class_name().str();
        }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        std::string enum_val_as_string(const Enum_T &enumVal) {
            constexpr const char *fallback = "(unrecognized)";
            return boost::describe::enum_to_string<Enum_T>(enumVal, fallback);
        }

        template<typename Enum_T>
        requires std::is_enum_v<Enum_T>
        [[maybe_unused]] std::string enum_val_inspect(const Enum_T &enumVal) {
            return enum_rb_type_name(enumVal) + "::" + enum_val_as_string(enumVal);
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
} // RuBLE::Utils

template<typename Key, class ProxyClass, class Value>
constexpr std::ostream &operator<<(std::ostream &os, const RuBLE::Registry<Key, ProxyClass, Value> &reg) {
    return os << reg.to_s();
}

template<const auto &FIELDS>
constexpr std::ostream &operator<<(std::ostream &os, const RuBLE::NamedBitSet<FIELDS> &nbs) { return os << nbs.to_s(); }
