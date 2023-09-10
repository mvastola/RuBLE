#include "AdapterProxy.h"
#include "AdapterProxyRegistry.h"

AdapterProxy::AdapterProxy(const std::shared_ptr<Adapter> &adapter) : _adapter(adapter), _addr(_adapter->address()) {
    setup_callbacks();
}

bool AdapterProxy::initialized() const { return _adapter && _adapter->initialized(); }

void *AdapterProxy::underlying() { return _adapter ? _adapter->underlying() : nullptr; }

std::string AdapterProxy::identifier() { return _adapter->identifier(); }

BluetoothAddress AdapterProxy::address() const { return _addr; /*_adapter->address();*/ }

void AdapterProxy::scan_start() { _adapter->scan_start(); }

void AdapterProxy::scan_stop() { _adapter->scan_stop(); }

void AdapterProxy::scan_for(int timeout_ms) { _adapter->scan_for(timeout_ms); }

bool AdapterProxy::scan_is_active() { return _adapter->scan_is_active(); }

bool AdapterProxy::operator==(const BluetoothAddress &addr) const { return !_addr.empty() && _addr == addr; }

bool AdapterProxy::operator==(const AdapterProxy &ap) const { return *this == ap._addr; }

std::vector<Peripheral> AdapterProxy::scan_get_results() { return _adapter->scan_get_results(); }
std::vector<Peripheral> AdapterProxy::get_paired_peripherals() { return _adapter->get_paired_peripherals(); }

Adapter &AdapterProxy::get() {
    return *_adapter;
}

const Adapter &AdapterProxy::get() const {
    return *_adapter;
}

bool AdapterProxy::bluetooth_enabled() { return Adapter::bluetooth_enabled(); }

std::vector<AdapterProxyPtr> AdapterProxy::get_adapters() {
    std::vector<Adapter> unwrappedAdapters = Adapter::get_adapters();
    std::vector<AdapterProxyPtr> result {};
    std::ranges::transform(unwrappedAdapters, back_inserter(result), [](Adapter &adapter) -> AdapterProxyPtr {
        return AdapterProxyRegistry::instance().fetch(adapter);
    });
    return result;
}

void AdapterProxy::on_scan_start(const Object& new_cb) { _on_scan_start.set(new_cb); }
void AdapterProxy::on_scan_stop(const Object& new_cb) { _on_scan_stop.set(new_cb); }
void AdapterProxy::on_scan_update(const Object& new_cb) { _on_scan_update.set(new_cb); }
void AdapterProxy::on_scan_find(const Object& new_cb) { _on_scan_find.set(new_cb); }

void AdapterProxy::fire_on_scan_started() { _on_scan_start.fire(); }
void AdapterProxy::fire_on_scan_stopped() { _on_scan_stop.fire(); }
void AdapterProxy::fire_on_scan_updated(const Peripheral& peripheral) { _on_scan_update.fire(peripheral); }
void AdapterProxy::fire_on_scan_found(const Peripheral& peripheral) { _on_scan_find.fire(peripheral); }

void AdapterProxy::setup_callbacks() {
    _adapter->set_callback_on_scan_start([this] () { this->fire_on_scan_started(); });
    _adapter->set_callback_on_scan_stop([this] () { this->fire_on_scan_stopped(); });
    _adapter->set_callback_on_scan_updated([this] (const Peripheral &peripheral) {
        this->fire_on_scan_updated(peripheral);
    });
    _adapter->set_callback_on_scan_found([this] (const Peripheral &peripheral) {
        this->fire_on_scan_found(peripheral);
    });
}

