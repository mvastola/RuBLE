#include "types/declarations.hpp"
#include "bindings/Adapter.hpp"
#include "bindings/Peripheral.hpp"
#include "management/Registry.hpp"
#include "management/RegistryFactory.hpp"
#include "bindings/Service.hpp"
#include "bindings/Characteristic.hpp"

namespace RuBLE {

    void Init_Registries() {
        rb_cAdapterRegistry = define_class_under<AdapterRegistry>(rb_mRuBLE, "AdapterRegistry")
                .define_method("size", &AdapterRegistry::size)
                .define_method("to_s", &AdapterRegistry::to_s);
        define_class_under<AdapterRegistry::Collection>(rb_mRuBLE, "AdapterRegistryMap");

        if (!adapterRegistryFactory) adapterRegistryFactory = AdapterRegistryFactory::instance();
        if (!adapterRegistry) adapterRegistry = adapterRegistryFactory->registry();
        rb_cAdapterRegistry.define_singleton_attr("registry", &*adapterRegistry, Rice::AttrAccess::Read);
        rb_cAdapterRegistry.iv_set("@registry", *adapterRegistry->_registry);

        rb_cPeripheralRegistry = define_class_under<PeripheralRegistry>(rb_mRuBLE, "PeripheralRegistry")
                .define_method("to_s", &PeripheralRegistry::to_s)
                .define_method("size", &PeripheralRegistry::size);

        rb_cAdapter
                .define_attr("peripheral_registry", &Adapter::_peripheral_registry, Rice::AttrAccess::Read);

        if (!peripheralRegistryFactory) peripheralRegistryFactory = PeripheralRegistryFactory::instance();

        rb_cServiceRegistry = define_class_under<ServiceRegistry>(rb_mRuBLE, "ServiceRegistry")
                .define_method("to_s", &ServiceRegistry::to_s)
                .define_method("size", &ServiceRegistry::size);

        define_class_under<std::shared_ptr<ServiceMap>>(rb_mRuBLE, "ServiceMap");
        rb_cPeripheral
            .define_method("services", &Peripheral::services, Rice::Return().keepAlive())
            .define_method("[]", &Peripheral::operator[])
            .define_attr("service_registry", &Peripheral::_service_registry, Rice::AttrAccess::Read)
        ;

        if (!serviceRegistryFactory) serviceRegistryFactory = ServiceRegistryFactory::instance();

        rb_cCharacteristicRegistry = define_class_under<CharacteristicRegistry>(rb_mRuBLE, "CharacteristicRegistry")
                .define_method("to_s", &ServiceRegistry::to_s)
                .define_method("size", &ServiceRegistry::size);

        define_class_under<ServiceRegistry::Collection>(rb_mRuBLE, "CharacteristicMap");
        rb_cService
                .define_method("characteristics", &Service::characteristics_rb)
                .define_method("[]", &Service::operator[])
                .define_attr("characteristic_registry", &Service::_characteristic_registry, Rice::AttrAccess::Read);

        if (!characteristicRegistryFactory) characteristicRegistryFactory = CharacteristicRegistryFactory::instance();
        define_class_under<CharacteristicRegistry::Collection>(rb_mRuBLE, "DescriptorMap")
                .define_method("descriptors", &Characteristic::descriptors_rb)
                .define_method("[]", &Characteristic::operator[]);
//        rb_cCharacteristic
//                .define_attr("descriptors", &Characteristic::_descriptors, Rice::AttrAccess::Read);
    }
}