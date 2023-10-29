#pragma once
#include <utility>
#include "types/declarations.hpp"

namespace SimpleRbBLE {
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

template<> struct std::hash<SimpleRbBLE::Adapter> {
    std::size_t operator()(const SimpleRbBLE::Adapter& a) const noexcept;
};

template<> struct std::hash<SimpleBLE::Peripheral> {
    std::size_t operator()(const SimpleBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Peripheral> {
    std::size_t operator()(const SimpleRbBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleBLE::Service> {
    std::size_t operator()(const SimpleBLE::Service& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Service> {
    std::size_t operator()(const SimpleRbBLE::Service& s) const noexcept;
};

template<> struct std::hash<SimpleBLE::Characteristic> {
    std::size_t operator()(const SimpleBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Characteristic> {
    std::size_t operator()(const SimpleRbBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Descriptor> {
    std::size_t operator()(const SimpleRbBLE::Descriptor& s) const noexcept;
};

