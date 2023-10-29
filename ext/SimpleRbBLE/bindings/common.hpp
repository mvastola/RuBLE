#pragma once

#include "types/ruby.hpp"
#include "types/declarations.hpp"

namespace SimpleRbBLE {
    extern std::shared_ptr<RubyQueue> rubyQueue;

    extern Rice::Module rb_mSimpleRbBLE;
    extern Rice::Module rb_mSimpleRbBLEUnderlying;
    extern Rice::Module rb_mUtils;
    extern BluetoothAddressType_DT rb_cBluetoothAddressType;
    extern Characteristic_DT rb_cCharacteristic;
    extern CharacteristicCapabilityType_DT rb_cCharacteristicCapabilityType;
    [[maybe_unused]] extern Descriptor_DT rb_cDescriptor;
    extern Service_DT rb_cService;
    extern ServiceRegistry_DT rb_cServiceRegistry;
    extern AdapterRegistry_DT rb_cAdapterRegistry;
    extern PeripheralRegistry_DT rb_cPeripheralRegistry;
    extern CharacteristicRegistry_DT rb_cCharacteristicRegistry;
    extern ByteArray_DT rb_cByteArray;
    extern Peripheral_DT rb_cPeripheral;
    extern Adapter_DT rb_cAdapter;


    extern std::shared_ptr<AdapterRegistryFactory> adapterRegistryFactory;
    extern std::shared_ptr<AdapterRegistry> adapterRegistry;
    extern std::shared_ptr<PeripheralRegistryFactory> peripheralRegistryFactory;
    extern std::shared_ptr<ServiceRegistryFactory> serviceRegistryFactory;
    extern std::shared_ptr<CharacteristicRegistryFactory> characteristicRegistryFactory;


    extern void Init_Adapter();
    extern void Init_BluetoothAddressType();
    extern void Init_Characteristic();
    extern void Init_ByteArray();
    extern void Init_Descriptor();
    extern void Init_Peripheral();
    extern void Init_Registries();
    extern void Init_Service();
    extern void Init_Utils();
    extern void Init_DeInit();
    extern void DeInit(VALUE);
    extern void c_debug();
}

