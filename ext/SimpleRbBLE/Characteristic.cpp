#include "SimpleRbBLE.h"

Characteristic_DT rb_cCharacteristic;

void Init_Characteristic() {
    // FIXME: A bunch of these classes inherit from abstract base classes (e.g. BaseAdapter),
    // and are also subclassed in different implementations (I think).
    // Do we need the constructor and methods to return/receive pointers to work well
    // (so the right virtual functions are called)? Not sure if rice is handling that all,
    // or just not seeing any obvious bugs yet because the base classes are so minimal.
    rb_cCharacteristic = define_class_under<Characteristic>(rb_mSimpleRbBLE, "Characteristic")
            .define_constructor(Constructor<Characteristic>())
            .define_method("uuid", &Characteristic::uuid) // returns BluetoothUUID
            .define_method("descriptors", &Characteristic::descriptors) // returns std::vector<Descriptor>
            .define_method("capabilities", &Characteristic::capabilities) // returns std::vector<std::string>
            .define_method("can_read", &Characteristic::can_read) // returns bool
            .define_method("can_write_request", &Characteristic::can_write_request) // returns bool
            .define_method("can_write_command", &Characteristic::can_write_command) // returns bool
            .define_method("can_notify", &Characteristic::can_notify) // returns bool
            .define_method("can_indicate", &Characteristic::can_indicate); // returns bool
    // virtual ~Characteristic() = default

}