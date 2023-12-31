#pragma once
#include <utility>
#include "types/declarations.hpp"

namespace RuBLE {
    template<typename Key>
    using HasherType = std::hash<Key>;

    template<typename Key>
    using HashFnType [[maybe_unused]] = std::invoke_result_t<std::hash<Key>()>;

    template<typename Key>
    using HashResultType = std::invoke_result<HashFnType<Key>, const Key &>::type;
    static_assert(std::is_same<std::size_t, HashResultType<std::string>>::value);
}

template<> struct std::hash<SimpleBLE::Adapter> {
    std::size_t operator()(const SimpleBLE::Adapter& a) const noexcept;
};

template<> struct std::hash<RuBLE::Adapter> {
    std::size_t operator()(const RuBLE::Adapter& a) const noexcept;
};

template<> struct std::hash<SimpleBLE::Peripheral> {
    std::size_t operator()(const SimpleBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<RuBLE::Peripheral> {
    std::size_t operator()(const RuBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleBLE::Service> {
    std::size_t operator()(const SimpleBLE::Service& s) const noexcept;
};

template<> struct std::hash<RuBLE::Service> {
    std::size_t operator()(const RuBLE::Service& s) const noexcept;
};

template<> struct std::hash<SimpleBLE::Characteristic> {
    std::size_t operator()(const SimpleBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<RuBLE::Characteristic> {
    std::size_t operator()(const RuBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<RuBLE::Descriptor> {
    std::size_t operator()(const RuBLE::Descriptor& s) const noexcept;
};

