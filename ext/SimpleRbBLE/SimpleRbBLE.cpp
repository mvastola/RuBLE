#include "SimpleRbBLE.h"

Enum<BluetoothAddressType> Init_BluetoothAddressType(Module &rb_mSimpleRbBLE) {
    return define_enum<BluetoothAddressType>("BluetoothAddressType", rb_mSimpleRbBLE)
            .define_value("PUBLIC", BluetoothAddressType::PUBLIC)
            .define_value("RANDOM", BluetoothAddressType::RANDOM)
            .define_value("UNSPECIFIED", BluetoothAddressType::UNSPECIFIED);

}

Data_Type<Descriptor> Init_SimpleRbBLEDescriptor(Module &rb_mSimpleRbBLE) {
    return define_class_under<Descriptor>(rb_mSimpleRbBLE, "Descriptor")
            .define_constructor(Constructor<Descriptor>())
            .define_method("uuid", &Descriptor::uuid); // returns BluetoothUUID
    // virtual ~Descriptor() = default
}



extern Data_Type<CallbackHolder> Init_SimpleRbBLECallbackHolder(Module &rb_mSimpleRbBLE);
extern Data_Type<Characteristic> Init_SimpleRbBLECharacteristic(Module &rb_mSimpleRbBLE);
extern Data_Type<Service> Init_SimpleRbBLEService(Module &rb_mSimpleRbBLE);
extern Data_Type<Peripheral> Init_SimpleRbBLEPeripheral(Module &rb_mSimpleRbBLE);
extern Data_Type<Adapter> Init_SimpleRbBLEAdapter(Module &rb_mSimpleRbBLE);

// See https://jasonroelofs.com/rice/4.x
extern "C"


void Init_SimpleRbBLE() {
    detail::Registries::instance.instances.isEnabled = false;
    rb_gc_disable();
    Module rb_mSimpleRbBLE = define_module("SimpleRbBLE");
    Enum<BluetoothAddressType> rb_cBluetoothAddressType = Init_BluetoothAddressType(rb_mSimpleRbBLE);
    Data_Type<CallbackHolder> rb_cCallbackHolder = Init_SimpleRbBLECallbackHolder(rb_mSimpleRbBLE);
    Data_Type<Descriptor> rb_cDescriptor = Init_SimpleRbBLEDescriptor(rb_mSimpleRbBLE);
    Data_Type<Characteristic> rb_cCharacteristic = Init_SimpleRbBLECharacteristic(rb_mSimpleRbBLE);
    Data_Type<Service> rb_cService = Init_SimpleRbBLEService(rb_mSimpleRbBLE);
    Data_Type<Peripheral> rb_cPeripheral = Init_SimpleRbBLEPeripheral(rb_mSimpleRbBLE);
    Data_Type<Adapter> rb_cAdapter = Init_SimpleRbBLEAdapter(rb_mSimpleRbBLE);
}
