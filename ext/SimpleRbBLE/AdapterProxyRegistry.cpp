#include "SimpleRbBLE.h"
#include "AdapterProxyRegistry.h"

using namespace std::string_literals;

std::optional<AdapterProxyRegistry> AdapterProxyRegistry::_instance {};

AdapterProxyRegistry &AdapterProxyRegistry::instance() {
    if(!_instance.has_value()) _instance = std::make_optional(AdapterProxyRegistry());
    return _instance.value();
}

AdapterProxyRegistry::AdapterProxyRegistry() : _registry() {}


AdapterProxyPtr AdapterProxyRegistry::operator[](const SimpleBLE::BluetoothAddress &addr) {
    return _registry.at(addr);
}

AdapterProxyPtr AdapterProxyRegistry::fetch(const Adapter &adapter) {
    std::shared_ptr<Adapter> adapter_ptr = std::make_shared<Adapter>(adapter);
    AdapterProxyPtr proxy_ptr = std::shared_ptr<AdapterProxy>(new AdapterProxy(adapter_ptr));

    auto [it, created] = _registry.try_emplace(proxy_ptr->address(), proxy_ptr);
    return it->second;
}
