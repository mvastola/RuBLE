#pragma once

#include <iostream>
#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>

using namespace Rice;
using namespace std::placeholders;
using SimpleBLE::Adapter,
        SimpleBLE::Peripheral,
        SimpleBLE::Service,
        SimpleBLE::ByteArray,
        SimpleBLE::BluetoothUUID,
        SimpleBLE::BluetoothAddress,
        SimpleBLE::Characteristic,
        SimpleBLE::Descriptor,
        SimpleBLE::BluetoothAddressType;

class AdapterProxy;
class CallbackHolder;
using AdapterProxyPtr = std::shared_ptr<AdapterProxy>;

using Descriptor_DT = Data_Type<Descriptor>;
using BluetoothAddressType_DT = Enum<BluetoothAddressType>;
using Adapter_DT = Data_Type<AdapterProxy>;
using Characteristic_DT = Data_Type<Characteristic>;
using Service_DT = Data_Type<Service>;
using Peripheral_DT = Data_Type<Peripheral>;
using CallbackHolder_DT = Data_Type<CallbackHolder>;

extern Module rb_mSimpleRbBLE;
extern Descriptor_DT rb_cDescriptor;
extern BluetoothAddressType_DT rb_cBluetoothAddressType;
extern Adapter_DT rb_cAdapter;
extern Characteristic_DT rb_cCharacteristic;
extern Service_DT rb_cService;
extern Peripheral_DT rb_cPeripheral;
extern CallbackHolder_DT rb_cCallbackHolder;

extern void Init_Descriptor();
extern void Init_BluetoothAddressType();
extern void Init_Characteristic();
extern void Init_Service();
extern void Init_Peripheral();
extern void Init_Adapter();
extern void Init_CallbackHolder();


