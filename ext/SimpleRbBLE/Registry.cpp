#include "common.hpp"
#include "Adapter.hpp"
#include "Peripheral.hpp"
#include "Registry.hpp"
#include "helpers/RegistryFactory.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"

namespace SimpleRbBLE {
    AdapterRegistry_DT rb_cAdapterRegistry;
    PeripheralRegistry_DT rb_cPeripheralRegistry;
    ServiceRegistry_DT rb_cServiceRegistry;
    CharacteristicRegistry_DT rb_cCharacteristicRegistry;

    std::shared_ptr<AdapterRegistryFactory> adapterRegistryFactory;
    std::shared_ptr<AdapterRegistry> adapterRegistry;
    std::shared_ptr<PeripheralRegistryFactory> peripheralRegistryFactory;
    std::shared_ptr<ServiceRegistryFactory> serviceRegistryFactory;
    std::shared_ptr<CharacteristicRegistryFactory> characteristicRegistryFactory;

    void Init_Registries() {
        rb_cAdapterRegistry = define_class_under<AdapterRegistry>(rb_mSimpleRbBLE, "AdapterRegistry")
                .define_method("size", &AdapterRegistry::size)
                .define_method("to_s", &AdapterRegistry::to_s);
        define_map_under<AdapterRegistry::Collection>(rb_cAdapterRegistry, "Map");

        if (!adapterRegistryFactory) adapterRegistryFactory = AdapterRegistryFactory::instance();
        if (!adapterRegistry) adapterRegistry = adapterRegistryFactory->registry();
        rb_cAdapterRegistry.define_singleton_attr("registry", &*adapterRegistry, Rice::AttrAccess::Read);
        rb_cAdapterRegistry.iv_set("@registry", *adapterRegistry->_registry);

        rb_cPeripheralRegistry = define_class_under<PeripheralRegistry>(rb_mSimpleRbBLE, "PeripheralRegistry")
                .define_method("to_s", &PeripheralRegistry::to_s)
                .define_method("size", &PeripheralRegistry::size);

        rb_cAdapter
                .define_attr("peripheral_registry", &Adapter::_peripheral_registry, Rice::AttrAccess::Read);

        if (!peripheralRegistryFactory) peripheralRegistryFactory = PeripheralRegistryFactory::instance();

        rb_cServiceRegistry = define_class_under<ServiceRegistry>(rb_mSimpleRbBLE, "ServiceRegistry")
                .define_method("to_s", &ServiceRegistry::to_s)
                .define_method("size", &ServiceRegistry::size);

        define_map_under<Peripheral::ServiceMap>(rb_cPeripheral, "ServiceMap");
        rb_cPeripheral
            .define_method("services", &Peripheral::services)
            .define_method("[]", &Peripheral::operator[])
            .define_attr("service_registry", &Peripheral::_service_registry, Rice::AttrAccess::Read)
        ;
        if (!serviceRegistryFactory) serviceRegistryFactory = ServiceRegistryFactory::instance();

        rb_cCharacteristicRegistry = define_class_under<CharacteristicRegistry>(rb_mSimpleRbBLE, "CharacteristicRegistry")
                .define_method("to_s", &ServiceRegistry::to_s)
                .define_method("size", &ServiceRegistry::size);

        define_map_under<CharacteristicRegistry::Collection>(rb_cService, "CharacteristicMap");
        rb_cService
                .define_method("characteristics", &Service::characteristics)
                .define_method("[]", &Service::operator[])
                .define_attr("characteristic_registry", &Service::_characteristic_registry, Rice::AttrAccess::Read);

        if (!characteristicRegistryFactory) characteristicRegistryFactory = CharacteristicRegistryFactory ::instance();
        define_map_under<Characteristic::DescriptorMap>(rb_cCharacteristic, "DescriptorMap");
        rb_cCharacteristic
                .define_method("descriptors", &Characteristic::descriptors)
                .define_method("[]", &Characteristic::operator[])
                .define_attr("descriptors", &Characteristic::_descriptors, Rice::AttrAccess::Read);

    }

}

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::AdapterRegistry>(SimpleRbBLE::AdapterRegistry *registry) {
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::PeripheralRegistry>(SimpleRbBLE::PeripheralRegistry *registry) {
//        std::cout << "ruby_mark-ing peripheral registry" << std::endl;
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::ServiceRegistry>(SimpleRbBLE::ServiceRegistry *registry) {
//        std::cout << "ruby_mark-ing peripheral registry" << std::endl;
        registry->ruby_mark();
    }

    template<>
    void ruby_mark<SimpleRbBLE::CharacteristicRegistry>(SimpleRbBLE::CharacteristicRegistry *registry) {
//        std::cout << "ruby_mark-ing peripheral registry" << std::endl;
        registry->ruby_mark();
    }
}