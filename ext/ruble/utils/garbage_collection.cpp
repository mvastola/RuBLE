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
    void ruby_mark<RuBLE::AdapterRegistryFactory>(RuBLE::AdapterRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::ByteArray>(RuBLE::ByteArray *byteArray) {
        byteArray->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::PeripheralRegistryFactory>(RuBLE::PeripheralRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::ServiceRegistryFactory>(RuBLE::ServiceRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::CharacteristicRegistryFactory>(RuBLE::CharacteristicRegistryFactory *registryFactory) {
        registryFactory->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::AdapterRegistry>(RuBLE::AdapterRegistry *registry) {
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<RuBLE::PeripheralRegistry>(RuBLE::PeripheralRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RuBLE::ServiceRegistry>(RuBLE::ServiceRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RuBLE::CharacteristicRegistry>(RuBLE::CharacteristicRegistry *registry) { registry->ruby_mark(); }

    template<>
    void ruby_mark<RuBLE::Service>(RuBLE::Service *service) { service->ruby_mark(); }

    template<>
    void ruby_mark<RuBLE::Adapter>(RuBLE::Adapter *adapter) { adapter->ruby_mark(); }


    template<>
    void ruby_mark<RuBLE::Characteristic>(RuBLE::Characteristic *characteristic) { characteristic->ruby_mark(); }

    template<>
    void ruby_mark<RuBLE::Peripheral>(RuBLE::Peripheral *peripheral) { peripheral->ruby_mark(); }
}

// TODO: do these change anything? Without these,
//  CLion seemed to be suggesting that many of the functions above were unused,
//  which would tend to cause hard-to-detect errors, so just to be safe,
//  assert that these templated versions of Rice::ruby_mark exist.
static_assert(&Rice::ruby_mark<RuBLE::Callback>);
static_assert(&Rice::ruby_mark<RuBLE::Adapter>);
static_assert(&Rice::ruby_mark<RuBLE::AdapterRegistry>);
static_assert(&Rice::ruby_mark<RuBLE::AdapterRegistryFactory>);
static_assert(&Rice::ruby_mark<RuBLE::Peripheral>);
static_assert(&Rice::ruby_mark<RuBLE::PeripheralRegistry>);
static_assert(&Rice::ruby_mark<RuBLE::PeripheralRegistryFactory>);
static_assert(&Rice::ruby_mark<RuBLE::Service>);
static_assert(&Rice::ruby_mark<RuBLE::ServiceRegistry>);
static_assert(&Rice::ruby_mark<RuBLE::ServiceRegistryFactory>);
static_assert(&Rice::ruby_mark<RuBLE::Characteristic>);
static_assert(&Rice::ruby_mark<RuBLE::CharacteristicRegistry>);
static_assert(&Rice::ruby_mark<RuBLE::CharacteristicRegistryFactory>);

