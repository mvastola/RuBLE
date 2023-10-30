#include "garbage_collection.hpp"

#include "management/Registry.hpp"
#include "management/RegistryFactory.hpp"
#include "bindings/Adapter.hpp"
#include "bindings/Peripheral.hpp"
#include "bindings/Service.hpp"
#include "bindings/Characteristic.hpp"
#include "containers/ByteArray.hpp"

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::AdapterRegistryFactory>(SimpleRbBLE::AdapterRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::ByteArray>(SimpleRbBLE::ByteArray *byteArray) {
        byteArray->ruby_mark();
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

    template<>
    void ruby_mark<SimpleRbBLE::AdapterRegistry>(SimpleRbBLE::AdapterRegistry *registry) {
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::PeripheralRegistry>(SimpleRbBLE::PeripheralRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<SimpleRbBLE::ServiceRegistry>(SimpleRbBLE::ServiceRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<SimpleRbBLE::CharacteristicRegistry>(SimpleRbBLE::CharacteristicRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<SimpleRbBLE::Service>(SimpleRbBLE::Service *service) { service->ruby_mark(); }

    template<>
    void ruby_mark<SimpleRbBLE::Adapter>(SimpleRbBLE::Adapter *adapter) { adapter->ruby_mark(); }


    template<>
    void ruby_mark<SimpleRbBLE::Characteristic>(SimpleRbBLE::Characteristic *characteristic) { characteristic->ruby_mark(); }

    template<>
    void ruby_mark<SimpleRbBLE::Peripheral>(SimpleRbBLE::Peripheral *peripheral) { peripheral->ruby_mark(); }
}

// TODO: do these change anything? Without these,
//  CLion seemed to be suggesting that many of the functions above were unused,
//  which would tend to cause hard-to-detect errors, so just to be safe,
//  assert that these templated versions of Rice::ruby_mark exist.
static_assert(&Rice::ruby_mark<SimpleRbBLE::Callback>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::Adapter>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::AdapterRegistry>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::AdapterRegistryFactory>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::Peripheral>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::PeripheralRegistry>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::PeripheralRegistryFactory>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::Service>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::ServiceRegistry>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::ServiceRegistryFactory>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::Characteristic>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::CharacteristicRegistry>);
static_assert(&Rice::ruby_mark<SimpleRbBLE::CharacteristicRegistryFactory>);

