#pragma once

#include <optional>
#include <memory>
#include <functional>

#include "AdapterProxy.h"


class AdapterProxyRegistry {
    using Collection = std::map<BluetoothAddress, AdapterProxyPtr>;
    static std::optional<AdapterProxyRegistry> _instance;

    Collection _registry;
    AdapterProxyRegistry();

public:
    static AdapterProxyRegistry &instance();

    AdapterProxyPtr fetch(const Adapter &adapter);
    AdapterProxyPtr operator[](const BluetoothAddress &addr);
};