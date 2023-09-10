#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include "SimpleRbBLE.h"
#include <functional>
#include <algorithm>
#include <ranges>
using namespace std::string_literals;

class AdapterProxy;

class AdapterProxy {
public:
    using NoArgCallback = std::function<void()>;
    using UnaryPeripheralCallback = std::function<void(Peripheral)>;

private:
    std::shared_ptr<Adapter> _adapter = nullptr;
    BluetoothAddress _addr;
    CallbackHolder _on_scan_start {};
    CallbackHolder _on_scan_stop {};
    CallbackHolder _on_scan_update {};
    CallbackHolder _on_scan_find {};

    explicit AdapterProxy(const std::shared_ptr<Adapter> &adapter);
    void setup_callbacks();
public:

    static bool bluetooth_enabled();
    static std::vector<AdapterProxyPtr> get_adapters();

    virtual ~AdapterProxy() = default;
    
    bool initialized() const;
    Adapter &get();
    const Adapter &get() const;

    void* underlying();

    std::string identifier();
    BluetoothAddress address() const;

    void scan_start();
    void scan_stop();
    void scan_for(int timeout_ms);
    bool scan_is_active();

    bool operator==(const BluetoothAddress &addr) const;
    bool operator==(const AdapterProxy &ap) const;

    std::vector<Peripheral> scan_get_results();

    void on_scan_start(const Object& on_scan_start);
    void on_scan_stop(const Object& on_scan_stop);
    void on_scan_update(const Object& on_scan_updated);
    void on_scan_find(const Object& on_scan_found);
    std::vector<Peripheral> get_paired_peripherals();

//private:
    void fire_on_scan_started();
    void fire_on_scan_stopped();
    void fire_on_scan_updated(const Peripheral& peripheral);
    void fire_on_scan_found(const Peripheral& peripheral);

    friend class AdapterProxyRegistry;
};

template<>
struct std::hash<AdapterProxy>
{
    std::size_t operator()(const AdapterProxy& ap) const noexcept
    {
        return std::hash<std::string>{}(ap.address());
    }
};


#pragma clang diagnostic pop