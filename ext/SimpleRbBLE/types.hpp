#pragma once

#ifdef SIMPLERBBLE_DEBUG
#include <valgrind/valgrind.h>
#endif
#include <iostream>
#include <memory>
#include <concepts>

#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>
#include <simpleble/Characteristic.h>

// from https://stackoverflow.com/a/6713727
#define STRINGIFICATOR(X) #X

namespace Rice { ; }
namespace SimpleRbBLE {

#ifdef SIMPLERBBLE_DEBUG
        static constexpr const bool DEBUG = true;
#else
        static constexpr const bool DEBUG = false;
#endif
    namespace Rice = ::Rice;
    using namespace ::Rice;

    using SimpleBLE::BluetoothAddress,
            SimpleBLE::BluetoothAddressType,
            SimpleBLE::BluetoothUUID,
            SimpleBLE::ByteArray;
//    class RubyQueue;

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

