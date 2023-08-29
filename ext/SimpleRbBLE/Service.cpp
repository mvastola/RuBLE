#include "SimpleRbBLE.h"

Service_DT rb_cService;

void Init_Service() {
    rb_cService = define_class_under<Service>(rb_mSimpleRbBLE, "Service")
            .define_constructor(Constructor<Service>())
            .define_method("uuid", &Service::uuid) // returns BluetoothUUID
            .define_method("data", &Service::data) // returns ByteArray
            .define_method("characteristics", &Service::characteristics); // returns std::vector<Characteristic>
    // virtual ~Service() = default
}