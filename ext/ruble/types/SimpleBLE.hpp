#pragma once

#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>
#include <simpleble/Characteristic.h>

namespace RuBLE {
    using BluetoothAddress = SimpleBLE::BluetoothAddress;
    using BluetoothAddressType = SimpleBLE::BluetoothAddressType;
    using BluetoothUUID = SimpleBLE::BluetoothUUID;

    template<typename T>
    concept BluetoothAddressable = requires(T t) {
        { t.address() } -> std::same_as<SimpleBLE::BluetoothAddress>;
    };

    template<typename T>
    concept UUIDable = requires(T t) {
        { t.uuid() } -> std::same_as<SimpleBLE::BluetoothUUID>;
    };
}