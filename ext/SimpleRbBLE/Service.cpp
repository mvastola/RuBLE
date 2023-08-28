#include "SimpleRbBLE.h"

Data_Type<Service> Init_SimpleRbBLEService(Module &rb_mSimpleRbBLE) {
    return define_class_under<Service>(rb_mSimpleRbBLE, "Service")
            .define_constructor(Constructor<Service>())
            .define_method("uuid", &Service::uuid) // returns BluetoothUUID
            .define_method("data", &Service::data) // returns ByteArray
            .define_method("characteristics", &Service::characteristics); // returns std::vector<Characteristic>
    // virtual ~Service() = default
}