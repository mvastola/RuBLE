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
    void ruby_mark<RubBLE::AdapterRegistryFactory>(RubBLE::AdapterRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::ByteArray>(RubBLE::ByteArray *byteArray) {
        byteArray->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::PeripheralRegistryFactory>(RubBLE::PeripheralRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::ServiceRegistryFactory>(RubBLE::ServiceRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::CharacteristicRegistryFactory>(RubBLE::CharacteristicRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::AdapterRegistry>(RubBLE::AdapterRegistry *registry) {
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<RubBLE::PeripheralRegistry>(RubBLE::PeripheralRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RubBLE::ServiceRegistry>(RubBLE::ServiceRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RubBLE::CharacteristicRegistry>(RubBLE::CharacteristicRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RubBLE::Service>(RubBLE::Service *service) { service->ruby_mark(); }

    template<>
    void ruby_mark<RubBLE::Adapter>(RubBLE::Adapter *adapter) { adapter->ruby_mark(); }


    template<>
    void ruby_mark<RubBLE::Characteristic>(RubBLE::Characteristic *characteristic) { characteristic->ruby_mark(); }

    template<>
    void ruby_mark<RubBLE::Peripheral>(RubBLE::Peripheral *peripheral) { peripheral->ruby_mark(); }
}

// TODO: do these change anything? Without these,
//  CLion seemed to be suggesting that many of the functions above were unused,
//  which would tend to cause hard-to-detect errors, so just to be safe,
//  assert that these templated versions of Rice::ruby_mark exist.
static_assert(&Rice::ruby_mark<RubBLE::Callback>);
static_assert(&Rice::ruby_mark<RubBLE::Adapter>);
static_assert(&Rice::ruby_mark<RubBLE::AdapterRegistry>);
static_assert(&Rice::ruby_mark<RubBLE::AdapterRegistryFactory>);
static_assert(&Rice::ruby_mark<RubBLE::Peripheral>);
static_assert(&Rice::ruby_mark<RubBLE::PeripheralRegistry>);
static_assert(&Rice::ruby_mark<RubBLE::PeripheralRegistryFactory>);
static_assert(&Rice::ruby_mark<RubBLE::Service>);
static_assert(&Rice::ruby_mark<RubBLE::ServiceRegistry>);
static_assert(&Rice::ruby_mark<RubBLE::ServiceRegistryFactory>);
static_assert(&Rice::ruby_mark<RubBLE::Characteristic>);
static_assert(&Rice::ruby_mark<RubBLE::CharacteristicRegistry>);
static_assert(&Rice::ruby_mark<RubBLE::CharacteristicRegistryFactory>);

