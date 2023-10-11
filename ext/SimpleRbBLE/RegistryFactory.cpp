#include "Registry.h"
#include "RegistryFactory.h"
#include "Adapter.h"
#include "Peripheral.h"
#include "Service.h"
#include "Characteristic.h"

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