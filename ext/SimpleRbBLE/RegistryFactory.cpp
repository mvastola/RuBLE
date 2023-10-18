#include "Registry.hpp"
#include "RegistryFactory.hpp"
#include "Adapter.hpp"
#include "Peripheral.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::AdapterRegistryFactory>(SimpleRbBLE::AdapterRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::PeripheralRegistryFactory>(SimpleRbBLE::PeripheralRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::ServiceRegistryFactory>(SimpleRbBLE::ServiceRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::CharacteristicRegistryFactory>(SimpleRbBLE::CharacteristicRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }
}