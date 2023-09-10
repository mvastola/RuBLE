#include "SimpleRbBLE.h"

Module rb_mSimpleRbBLE;
Descriptor_DT rb_cDescriptor;
BluetoothAddressType_DT rb_cBluetoothAddressType;


void Init_BluetoothAddressType() {
    rb_cBluetoothAddressType = define_enum<BluetoothAddressType>("BluetoothAddressType", rb_mSimpleRbBLE)
            .define_value("PUBLIC", BluetoothAddressType::PUBLIC)
            .define_value("RANDOM", BluetoothAddressType::RANDOM)
            .define_value("UNSPECIFIED", BluetoothAddressType::UNSPECIFIED);

}

void Init_Descriptor() {
    rb_cDescriptor = define_class_under<Descriptor>(rb_mSimpleRbBLE, "Descriptor")
            .define_constructor(Constructor<Descriptor>())
            .define_method("uuid", &Descriptor::uuid); // returns BluetoothUUID
    // virtual ~Descriptor() = default
}


// See https://jasonroelofs.com/rice/4.x
extern "C"

void Init_SimpleRbBLE() {
    detail::Registries::instance.instances.isEnabled = true;
    rb_mSimpleRbBLE = define_module("SimpleRbBLE");
    Init_BluetoothAddressType();
    Init_Descriptor();
    Init_Characteristic();
    Init_Service();
    Init_Peripheral();
    Init_Adapter();
}


