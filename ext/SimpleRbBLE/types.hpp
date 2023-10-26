#pragma once

#include RUBY_EXTCONF_H
#ifdef HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
#endif
#include <iostream>
#include <memory>
#include <concepts>
#include <span>
#include <cstddef>

#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>
#include <simpleble/Characteristic.h>

// from https://stackoverflow.com/a/6713727
#define STRINGIFICATOR(X) #X

namespace Rice {}
namespace SimpleRbBLE {
#ifdef SIMPLERBBLE_DEBUG
        static constexpr const bool DEBUG = true;
#else
        static constexpr const bool DEBUG = false;
#endif
    class InRuby;
    extern thread_local InRuby in_ruby;

    namespace Rice = ::Rice;
    using namespace ::Rice;

    using BluetoothAddress = SimpleBLE::BluetoothAddress;
    using BluetoothAddressType = SimpleBLE::BluetoothAddressType;
    using BluetoothUUID = SimpleBLE::BluetoothUUID;

    template <typename T = void>
    using RbThreadCreateFn = VALUE(*)(T*);
    template <typename T = void>
    using UnboundRbThreadCreateFn = VALUE(*)(RbThreadCreateFn<T>, void*);
    template <typename Ret = void, typename T = void>
    using RbThreadCallFn = Ret *(*)(T*);
    template <typename T = void>
    using RbUbfFn = void (*)(T*);

    template<const auto &FIELD_NAMES> class NamedBitSet;

    class Callback;
    template<typename Key, class ProxyClass, class Value>
    class Registry;

    template<class RegistryType>
    class RegistryFactory;

    using BluetoothAddressType_DT = Enum<SimpleBLE::BluetoothAddressType>;

    class ByteArray;
    using ByteArray_DT = Data_Type<ByteArray>;
    using ByteArray_DO = Data_Object<ByteArray>;

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

    using str_char_type = std::string::value_type;

    template<typename T>
    concept BluetoothAddressable = requires(T t) {
        { t.address() } -> std::same_as<SimpleBLE::BluetoothAddress>;
    };

    template<typename T>
    concept UUIDable = requires(T t) {
        { t.uuid() } -> std::same_as<SimpleBLE::BluetoothUUID>;
    };

    template<typename T>
    concept HasRubyObject = requires(T &t) {
        { t.self() } -> std::constructible_from<Object>;
    };


    template<typename T>
    concept CanSpanBytes = requires (const T &t) { std::as_bytes(t); };

    template<typename T>
    concept Span = std::same_as<std::remove_cvref_t<T>, std::span<typename T::element_type, T::extent>>;

    template<typename T>
    concept Byte = std::same_as<std::remove_cvref_t<T>, std::byte> ||
            std::same_as<std::remove_cvref_t<T>, std::underlying_type_t<std::byte>>;

    // because exact, constexpr division with round up
    template <std::integral auto A, std::integral auto B,
            std::integral Res = std::common_type_t<decltype(A), decltype(B)>>
    constexpr const Res divmod_ceil = (static_cast<Res>(A) / static_cast<Res>(B)) +
            (static_cast<Res>(A) % static_cast<Res>(B) == 0) ? 0 : 1;
}

namespace Rice {
    template<class T>
    void ruby_mark(T *);
}

