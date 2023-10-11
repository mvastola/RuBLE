#pragma once

#include "types.h"
#include "utils.h"

namespace SimpleRbBLE {

    extern std::shared_ptr<RubyQueue> rubyQueue;

    extern Module rb_mSimpleRbBLE;
    extern Module rb_mSimpleRbBLEUnderlying;
    extern BluetoothAddressType_DT rb_cBluetoothAddressType;
    extern Characteristic_DT rb_cCharacteristic;
    extern CharacteristicCapabilityType_DT rb_cCharacteristicCapabilityType;
    extern Descriptor_DT rb_cDescriptor;
    extern Service_DT rb_cService;
    extern ServiceRegistry_DT rb_cServiceRegistry;
    extern AdapterRegistry_DT rb_cAdapterRegistry;
    extern PeripheralRegistry_DT rb_cPeripheralRegistry;
    extern CharacteristicRegistry_DT rb_cCharacteristicRegistry;
    extern ConvertableByteArray_DT rb_cConvertableByteArray;

    extern void Init_Adapter();
    extern void Init_BluetoothAddressType();
    extern void Init_Characteristic();
    extern void Init_ConvertableByteArray();
    extern void Init_Descriptor();
    extern void Init_Peripheral();
    extern void Init_Registries();
    extern void Init_Service();
    extern void Init_DeInit();
    extern void DeInit(VALUE);

    extern void c_debug();
}

