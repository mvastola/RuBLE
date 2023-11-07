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
    void ruby_mark<Rubble::AdapterRegistryFactory>(Rubble::AdapterRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::ByteArray>(Rubble::ByteArray *byteArray) {
        byteArray->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::PeripheralRegistryFactory>(Rubble::PeripheralRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::ServiceRegistryFactory>(Rubble::ServiceRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::CharacteristicRegistryFactory>(Rubble::CharacteristicRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::AdapterRegistry>(Rubble::AdapterRegistry *registry) {
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<Rubble::PeripheralRegistry>(Rubble::PeripheralRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<Rubble::ServiceRegistry>(Rubble::ServiceRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<Rubble::CharacteristicRegistry>(Rubble::CharacteristicRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<Rubble::Service>(Rubble::Service *service) { service->ruby_mark(); }

    template<>
    void ruby_mark<Rubble::Adapter>(Rubble::Adapter *adapter) { adapter->ruby_mark(); }


    template<>
    void ruby_mark<Rubble::Characteristic>(Rubble::Characteristic *characteristic) { characteristic->ruby_mark(); }

    template<>
    void ruby_mark<Rubble::Peripheral>(Rubble::Peripheral *peripheral) { peripheral->ruby_mark(); }
}

// TODO: do these change anything? Without these,
//  CLion seemed to be suggesting that many of the functions above were unused,
//  which would tend to cause hard-to-detect errors, so just to be safe,
//  assert that these templated versions of Rice::ruby_mark exist.
static_assert(&Rice::ruby_mark<Rubble::Callback>);
static_assert(&Rice::ruby_mark<Rubble::Adapter>);
static_assert(&Rice::ruby_mark<Rubble::AdapterRegistry>);
static_assert(&Rice::ruby_mark<Rubble::AdapterRegistryFactory>);
static_assert(&Rice::ruby_mark<Rubble::Peripheral>);
static_assert(&Rice::ruby_mark<Rubble::PeripheralRegistry>);
static_assert(&Rice::ruby_mark<Rubble::PeripheralRegistryFactory>);
static_assert(&Rice::ruby_mark<Rubble::Service>);
static_assert(&Rice::ruby_mark<Rubble::ServiceRegistry>);
static_assert(&Rice::ruby_mark<Rubble::ServiceRegistryFactory>);
static_assert(&Rice::ruby_mark<Rubble::Characteristic>);
static_assert(&Rice::ruby_mark<Rubble::CharacteristicRegistry>);
static_assert(&Rice::ruby_mark<Rubble::CharacteristicRegistryFactory>);

