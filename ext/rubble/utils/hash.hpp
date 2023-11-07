#pragma once
#include <utility>
#include "types/declarations.hpp"

namespace Rubble {
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

template<> struct std::hash<Rubble::Adapter> {
    std::size_t operator()(const Rubble::Adapter& a) const noexcept;
};

template<> struct std::hash<SimpleBLE::Peripheral> {
    std::size_t operator()(const SimpleBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<Rubble::Peripheral> {
    std::size_t operator()(const Rubble::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleBLE::Service> {
    std::size_t operator()(const SimpleBLE::Service& s) const noexcept;
};

template<> struct std::hash<Rubble::Service> {
    std::size_t operator()(const Rubble::Service& s) const noexcept;
};

template<> struct std::hash<SimpleBLE::Characteristic> {
    std::size_t operator()(const SimpleBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<Rubble::Characteristic> {
    std::size_t operator()(const Rubble::Characteristic& s) const noexcept;
};

template<> struct std::hash<Rubble::Descriptor> {
    std::size_t operator()(const Rubble::Descriptor& s) const noexcept;
};

