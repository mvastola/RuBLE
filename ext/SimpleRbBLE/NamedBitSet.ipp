#pragma once

#include "types.hpp"
#include "NamedBitSet.hpp"

#include <string>
#include "utils.hpp"
using namespace std::string_literals;

namespace SimpleRbBLE {
    template<const auto &FIELD_NAMES>
    constexpr std::string NamedBitSet<FIELD_NAMES>::to_s() const { return join(flag_names(), "|"); }

    template<const auto &FIELD_NAMES>
    template<typename ObjT, FlagCheck::Map<ObjT> R>
    constexpr NamedBitSet<FIELD_NAMES>::NamedBitSet(R &r, ObjT &&obj) {
        for (const auto &[name, fn] : r) (*this)[name] = FlagCheck::test_flag<ObjT>(fn, obj);
    }

    template<const auto &FIELD_NAMES>
    constexpr NamedBitSet<FIELD_NAMES>::NamedBitSet(const NamedBitSet::BitSet &bitSet) : _bits(bitSet) {}

    template<const auto &FIELD_NAMES>
    constexpr NamedBitSet<FIELD_NAMES>::NamedBitSet(const auto &rangeList) { // NOLINT(*-explicit-constructor)
        for (const auto &sv: rangeList) (*this)[sv] = true;
    }

    template<const auto &FIELD_NAMES>
    constexpr NamedBitSet<FIELD_NAMES>::NamedBitSet(const std::initializer_list<const std::string_view> &list) {
        for (const auto &sv: list) (*this)[sv] = true;
    }

    template<const auto &FIELD_NAMES>
    constexpr std::vector<std::string> NamedBitSet<FIELD_NAMES>::flag_name_strs() const {
        const auto names = flag_names();
        return { names.begin(), names.end() };
    }

    template<const auto &FIELD_NAMES>
    constexpr std::vector<std::string_view> NamedBitSet<FIELD_NAMES>::flag_names() const {
        std::vector<std::string_view> result;
        for (const auto &[name, idx]: PAIRS) {
            if (_bits[idx]) result.push_back(name);
        }
        return result;
    }


    template<const auto &FIELD_NAMES>
    constexpr bool NamedBitSet<FIELD_NAMES>::operator[](const std::string_view &name) const {
        return _bits[index_of(name)];
    }

    template<const auto &FIELD_NAMES>
    constexpr NamedBitSet<FIELD_NAMES>::BitRef NamedBitSet<FIELD_NAMES>::operator[](const std::string_view &name) {
        return _bits[index_of(name)];
    }

    template<const auto &FIELD_NAMES>
    constexpr const NamedBitSet<FIELD_NAMES>::BitSet &NamedBitSet<FIELD_NAMES>::bits() const { return _bits; }

    template<const auto &FIELD_NAMES>
    constexpr std::size_t NamedBitSet<FIELD_NAMES>::index_of(const std::string_view &name) {
        for (const auto &[field, idx]: PAIRS) {
            if (field == name) return idx;
        }
        std::string errmsg = "Invalid field name "s + std::string(name.data()) + " in call to index_of";
        throw std::invalid_argument(errmsg);
    }
};