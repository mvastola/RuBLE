#include "hash.hpp"
#include "bindings/Adapter.hpp"
#include "bindings/Characteristic.hpp"
#include "bindings/Peripheral.hpp"

std::size_t std::hash<SimpleBLE::Adapter>::operator()(const SimpleBLE::Adapter &a) const noexcept {
    SimpleBLE::BluetoothAddress addr = const_cast<SimpleBLE::Adapter &>(a).address();
    std::size_t h1 = typeid(a).hash_code();
    static auto hasher = std::hash<SimpleBLE::BluetoothAddress>{};
    return h1 ^ (hasher(addr) << 1);
}

std::size_t std::hash<RuBLE::Adapter>::operator()(const RuBLE::Adapter &a) const noexcept {
    std::size_t h1 = typeid(a).hash_code();
    static auto hasher = std::hash<SimpleBLE::Adapter>{};
    return h1 ^ (hasher(a.get()) << 1);
}

std::size_t std::hash<SimpleBLE::Characteristic>::operator()(const SimpleBLE::Characteristic &p) const noexcept {
    auto uuid = const_cast<SimpleBLE::Characteristic &>(p).uuid();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<decltype(uuid)>{};
    return h1 ^ (hasher(uuid) << 1);
}

std::size_t std::hash<RuBLE::Characteristic>::operator()(const RuBLE::Characteristic &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Characteristic>{};
    return h1 ^ (hasher(p.get()) << 1);
}

std::size_t std::hash<RuBLE::Descriptor>::operator()(const RuBLE::Descriptor& d) const noexcept {
    std::size_t h1 = typeid(d).hash_code();
    static auto hasher = std::hash<std::remove_cvref_t<decltype(d.uuid())>>{};
    return h1 ^ (hasher(d.uuid()) << 1);
}


std::size_t std::hash<SimpleBLE::Service>::operator()(const SimpleBLE::Service &p) const noexcept {
    auto uuid = const_cast<SimpleBLE::Service &>(p).uuid();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<decltype(uuid)>{};
    return h1 ^ (hasher(uuid) << 1);
}

std::size_t std::hash<RuBLE::Service>::operator()(const RuBLE::Service &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Service>{};
    return h1 ^ (hasher(p.get()) << 1);
}

size_t std::hash<SimpleBLE::Peripheral>::operator()(const SimpleBLE::Peripheral &p) const noexcept {
    SimpleBLE::BluetoothAddress addr = const_cast<SimpleBLE::Peripheral &>(p).address();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::BluetoothAddress>{};
    return h1 ^ (hasher(addr) << 1);
}

std::size_t std::hash<RuBLE::Peripheral>::operator()(const RuBLE::Peripheral &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Peripheral>{};
    return h1 ^ (hasher(p.get()) << 1);
}

template<> struct std::hash<SimpleBLE::Adapter>;
template<> struct std::hash<RuBLE::Adapter>;
template<> struct std::hash<SimpleBLE::Peripheral>;
template<> struct std::hash<RuBLE::Peripheral>;
template<> struct std::hash<SimpleBLE::Service>;
template<> struct std::hash<RuBLE::Service>;
template<> struct std::hash<SimpleBLE::Characteristic>;
template<> struct std::hash<RuBLE::Characteristic>;
template<> struct std::hash<RuBLE::Descriptor>;
// TODO: Use this in the registry map, but don't put in std::hash
//template<typename Key, class ProxyClass, class Value>
//struct std::hash<RuBLE::Registry<Key, ProxyClass, Value>> {
//    std::size_t operator()(const RuBLE::Registry<Key, ProxyClass, Value>& r) const noexcept {
//        std::size_t h1 = typeid(r).hash_code();
//        using RegistryOwner = RuBLE::Registry<Key, ProxyClass, Value>::Owner;
//        static auto hasher = std::hash<SimpleBLE::BluetoothAddress>{};
//        return h1 ^ (hasher(r._registry) << 1);
//    }
//};