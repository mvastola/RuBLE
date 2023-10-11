#pragma once

#ifdef DEBUG
#include <valgrind/valgrind.h>
#endif
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <vector>
#include <typeinfo>
#include <type_traits>
#include <functional>
#include <ranges>
#include <filesystem>
#include <deque>
#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include <bitset>

using namespace std::placeholders;
using namespace std::string_literals;
using namespace std::literals::chrono_literals;
namespace chrono = std::chrono;
namespace fs = std::filesystem;
namespace ranges = std::ranges;
namespace views = ranges::views;

#include <rice/rice.hpp>
#include <rice/stl.hpp>
using namespace Rice;

#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>
#include <simpleble/Characteristic.h>

namespace SimpleRbBLE {
    using SimpleBLE::BluetoothAddress,
            SimpleBLE::BluetoothAddressType,
            SimpleBLE::BluetoothUUID,
            SimpleBLE::ByteArray;

    class RubyQueue;

    class Callback;

    template<typename Key, class ProxyClass, class Value>
    class Registry;

    template<class RegistryType>
    class RegistryFactory;

    using BluetoothAddressType_DT = Enum<SimpleBLE::BluetoothAddressType>;

    class ConvertableByteArray;
    using ConvertableByteArray_DT = Data_Type<ConvertableByteArray>;
    using ConvertableByteArray_DO = Data_Object<ConvertableByteArray>;

    class Adapter;
    using Adapter_DT = Data_Type<Adapter>;
    using Adapter_DO = Data_Object<Adapter>;
    using AdapterRegistry = Registry<SimpleBLE::BluetoothAddress, Adapter, SimpleBLE::Adapter>;
    using AdapterRegistry_DT = Data_Type<AdapterRegistry>;
    using AdapterRegistry_DO = Data_Object<AdapterRegistry>;
    using AdapterRegistryFactory = RegistryFactory<AdapterRegistry>;

    class Peripheral;
    using Peripheral_DT = Data_Type<Peripheral>;
    using Peripheral_DO = Data_Object<Peripheral>;
    using PeripheralRegistry = Registry<SimpleBLE::BluetoothAddress, Peripheral, SimpleBLE::Peripheral>;
    using PeripheralRegistry_DT = Data_Type<PeripheralRegistry>;
    using PeripheralRegistry_DO = Data_Object<PeripheralRegistry>;
    using PeripheralRegistryFactory = RegistryFactory<PeripheralRegistry>;

    class Service;
    using Service_DT = Data_Type<Service>;
    using Service_DO = Data_Object<Service>;
    using ServiceRegistry = Registry<SimpleBLE::BluetoothUUID, Service, SimpleBLE::Service>;
    using ServiceRegistry_DT = Data_Type<ServiceRegistry>;
    using ServiceRegistry_DO = Data_Object<ServiceRegistry>;
    using ServiceRegistryFactory = RegistryFactory<ServiceRegistry>;

    class Characteristic;
    using Characteristic_DT = Data_Type<Characteristic>;
    using Characteristic_DO = Data_Object<Characteristic>;
    using CharacteristicRegistry = Registry<SimpleBLE::BluetoothUUID, Characteristic, SimpleBLE::Characteristic>;
    using CharacteristicRegistry_DT = Data_Type<CharacteristicRegistry>;
    using CharacteristicRegistry_DO = Data_Object<CharacteristicRegistry>;
    using CharacteristicRegistryFactory = RegistryFactory<CharacteristicRegistry>;

    enum class CharacteristicCapabilityType : std::size_t;
    using CharacteristicCapabilityType_DT = Data_Type<CharacteristicCapabilityType>;
    using CharacteristicCapabilityType_DO = Data_Object<CharacteristicCapabilityType>;

    class Descriptor;
    using Descriptor_DT = Data_Type<Descriptor>;
    using Descriptor_DO = Data_Object<Descriptor>;

    template <typename Key>
    using HasherType = std::hash<Key>;

    template <typename Key>
    using HashFnType = std::invoke_result_t<std::hash<Key>()>;

    template <typename Key>
    using HashResultType = std::invoke_result<HashFnType<Key>, const Key&>::type;
    static_assert(std::is_same<std::size_t, HashResultType<std::string>>::value);

    template<typename T>
    concept BluetoothAddressable = requires(T t) {
        { t.address() } -> std::same_as<SimpleBLE::BluetoothAddress>;
    };

    template<typename T>
    concept UUIDable = requires(T t) {
        { t.uuid() } -> std::same_as<SimpleBLE::BluetoothUUID>;
    };

    template<typename T>
    concept CallbackSetuppable = requires(T t) {
        { t.setup_callbacks() };
    };

    template<class T, class R>
    concept RangeOf = requires(const R &r, T &t) {
        { *ranges::begin(r) } -> std::convertible_to<T>;
        { *ranges::end(r) } -> std::convertible_to<T>;
    };

    template<ranges::viewable_range RangeT>
    using RangeIteratorType = std::invoke_result_t<decltype(std::ranges::begin), RangeT&&>;
    template<ranges::viewable_range RangeT>
    using RangeElementType = std::invoke_result_t<decltype(RangeIteratorType<RangeT>::operator*), RangeIteratorType<RangeT>&&>;

    const constexpr static auto semaphore_max = std::numeric_limits<uint16_t>::max();
}

namespace Rice {
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistry>(SimpleRbBLE::AdapterRegistry*);
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistryFactory>(SimpleRbBLE::AdapterRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Peripheral>(SimpleRbBLE::Peripheral*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistry>(SimpleRbBLE::PeripheralRegistry*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistryFactory>(SimpleRbBLE::PeripheralRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Service>(SimpleRbBLE::Service*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistry>(SimpleRbBLE::ServiceRegistry*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistryFactory>(SimpleRbBLE::ServiceRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Characteristic>(SimpleRbBLE::Characteristic*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistry>(SimpleRbBLE::CharacteristicRegistry*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistryFactory>(SimpleRbBLE::CharacteristicRegistryFactory*);
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