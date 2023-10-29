#pragma once

#include "config.h"
#ifdef HAVE_VALGRIND
#include <valgrind/valgrind.h>
#endif
//#include <iostream>
//#include <memory>
//#include <concepts>
//#include <span>
//#include <cstddef>

// from https://stackoverflow.com/a/6713727
// #define STRINGIFICATOR(X) #X
#include "types/SimpleBLE.hpp"

namespace Rice {
    class String;
    class Array;
    class Object;
    template<typename T>
    class Data_Type;
    template<typename T>
    class Data_Object;
    template<typename Enum_T>
    class Enum;
}

namespace SimpleRbBLE {
#ifdef SIMPLERBBLE_DEBUG
    static constexpr const bool DEBUG = true;
#else
    static constexpr const bool DEBUG = false;
#endif

    template<const auto &FIELD_NAMES>
    class NamedBitSet;

    using str_char_type = std::string::value_type;

    class RubyQueue;

    class Callback;
    template<typename Key, class ProxyClass, class Value>
    class Registry;

    template<class RegistryType>
    class RegistryFactory;

    using BluetoothAddressType_DT = Rice::Enum<SimpleBLE::BluetoothAddressType>;

    class ByteArray;
    using ByteArray_DT = Rice::Data_Type<ByteArray>;
    using ByteArray_DO = Rice::Data_Object<ByteArray>;

    class Adapter;
    using Adapter_DT = Rice::Data_Type<Adapter>;
    using Adapter_DO = Rice::Data_Object<Adapter>;
    using AdapterRegistry = Registry<SimpleBLE::BluetoothAddress, Adapter, SimpleBLE::Adapter>;
    using AdapterRegistry_DT = Rice::Data_Type<AdapterRegistry>;
    using AdapterRegistry_DO [[maybe_unused]] = Rice::Data_Object<AdapterRegistry>;
    using AdapterRegistryFactory = RegistryFactory<AdapterRegistry>;

    class Peripheral;
    using Peripheral_DT = Rice::Data_Type<Peripheral>;
    using Peripheral_DO = Rice::Data_Object<Peripheral>;
    using PeripheralRegistry = Registry<SimpleBLE::BluetoothAddress, Peripheral, SimpleBLE::Peripheral>;
    using PeripheralRegistry_DT = Rice::Data_Type<PeripheralRegistry>;
    using PeripheralRegistry_DO [[maybe_unused]] = Rice::Data_Object<PeripheralRegistry>;
    using PeripheralRegistryFactory = RegistryFactory<PeripheralRegistry>;

    class Service;
    using Service_DT = Rice::Data_Type<Service>;
    using Service_DO [[maybe_unused]] = Rice::Data_Object<Service>;
    using ServiceRegistry = Registry<SimpleBLE::BluetoothUUID, Service, SimpleBLE::Service>;
    using ServiceRegistry_DT = Rice::Data_Type<ServiceRegistry>;
    using ServiceRegistry_DO [[maybe_unused]] = Rice::Data_Object<ServiceRegistry>;
    using ServiceRegistryFactory = RegistryFactory<ServiceRegistry>;

    class Characteristic;
    using Characteristic_DT = Rice::Data_Type<Characteristic>;
    using Characteristic_DO [[maybe_unused]] = Rice::Data_Object<Characteristic>;
    using CharacteristicRegistry = Registry<SimpleBLE::BluetoothUUID, Characteristic, SimpleBLE::Characteristic>;
    using CharacteristicRegistry_DT = Rice::Data_Type<CharacteristicRegistry>;
    using CharacteristicRegistry_DO [[maybe_unused]] = Rice::Data_Object<CharacteristicRegistry>;
    using CharacteristicRegistryFactory = RegistryFactory<CharacteristicRegistry>;

    enum class CharacteristicCapabilityType : std::size_t;
    using CharacteristicCapabilityType_DT = Rice::Data_Type<CharacteristicCapabilityType>;
    using CharacteristicCapabilityType_DO [[maybe_unused]] = Rice::Data_Object<CharacteristicCapabilityType>;

    class Descriptor;
    using Descriptor_DT = Rice::Data_Type<Descriptor>;
    using Descriptor_DO = Rice::Data_Object<Descriptor>;

}

