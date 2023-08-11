#include <rice/rice.hpp>
#include <rice/stl.hpp>
#include <simpleble/SimpleBLE.h>
#include <simpleble/Adapter.h>
#include <vector>
#include <functional>

using namespace Rice;
using namespace std::placeholders;
using SimpleBLE::Adapter,
        SimpleBLE::Peripheral,
        SimpleBLE::Service,
        SimpleBLE::ByteArray,
        SimpleBLE::BluetoothUUID,
        SimpleBLE::Characteristic,
        SimpleBLE::Descriptor,
        SimpleBLE::BluetoothAddressType;


// See https://jasonroelofs.com/rice/4.x
extern "C"
// Just to silence some warnings for now:
// (Can remove the unused variables as long as we ensure that doesn't cause the code to get optimized out)
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedLocalVariable"
void Init_SimpleRbBLE(void) {
    Module rb_mSimpleRbBLE = define_module("SimpleRbBLE");

    Enum<BluetoothAddressType> rb_cBluetoothAddressType =
            define_enum<BluetoothAddressType>("BluetoothAddressType", rb_mSimpleRbBLE)
                    .define_value("PUBLIC", BluetoothAddressType::PUBLIC)
                    .define_value("RANDOM", BluetoothAddressType::RANDOM)
                    .define_value("UNSPECIFIED", BluetoothAddressType::UNSPECIFIED);

    Data_Type<Descriptor> rb_cDescriptor = define_class_under<Descriptor>(rb_mSimpleRbBLE, "Descriptor")
            .define_constructor(Constructor<Descriptor>())
            .define_method("uuid", &Descriptor::uuid); // returns BluetoothUUID
    // virtual ~Descriptor() = default
    
    // FIXME: A bunch of these classes inherit from abstract base classes (e.g. BaseAdapter), 
    // and are also subclassed in different implementations (I think). 
    // Do we need the constructor and methods to return/receive pointers to work well 
    // (so the right virtual functions are called)? Not sure if rice is handling that all, 
    // or just not seeing any obvious bugs yet because the base classes are so minimal.
    Data_Type<Characteristic> rb_cCharacteristic = define_class_under<Characteristic>(rb_mSimpleRbBLE,
                                                                                      "Characteristic")
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

    Data_Type<Service> rb_cService = define_class_under<Service>(rb_mSimpleRbBLE, "Service")
            .define_constructor(Constructor<Service>())
            .define_method("uuid", &Service::uuid) // returns BluetoothUUID
            .define_method("data", &Service::data) // returns ByteArray
            .define_method("characteristics", &Service::characteristics); // returns std::vector<Characteristic>
    // virtual ~Service() = default

    // TODO: why is CLion indenting like this? (Need to fix code style settings)
    Data_Type<Peripheral>
            rb_cPeripheral = define_class_under<Peripheral>(rb_mSimpleRbBLE, "Peripheral")
                                     .define_constructor(Constructor<Peripheral>())
                                     .define_method("initialized", &Peripheral::initialized)
                                     .define_method("identifier", &Peripheral::identifier) // returns std::string
                                     .define_method("address", &Peripheral::address) // returns BluetoothAddress (alias for string)
                                     .define_method("address_type",
                                                    &Peripheral::address_type) // returns BluetoothAddressType
                                     .define_method("rssi", &Peripheral::rssi) // returns int16_t
                                     .define_method("tx_power", &Peripheral::tx_power) // returns int16_t
                                     .define_method("mtu", &Peripheral::mtu) // returns uint16_t
                                     .define_method("connect", &Peripheral::connect)
                                     .define_method("disconnect", &Peripheral::disconnect)
                                     .define_method("is_connected", &Peripheral::is_connected) // returns bool
                                     .define_method("is_connectable", &Peripheral::is_connectable) // returns bool
                                     .define_method("is_paired", &Peripheral::is_paired) // returns bool
                                     .define_method("unpair", &Peripheral::unpair)
                                     .define_method("services", &Peripheral::services) // returns std::vector<Service>
                                     // TODO: this isn't returning any useful data. Should we keep it?
                                     .define_method("manufacturer_data",
                                                    &Peripheral::manufacturer_data) // returns std::map<uint16_t, ByteArray>
                                     .define_method("write_request",
                                                    &Peripheral::write_request) // takes (BluetoothUUID const &service, BluetoothUUID const &characteristic, ByteArray const &data))
                                     .define_method <
    ByteArray(Peripheral::*)(BluetoothUUID const &, BluetoothUUID const &)>
    ("read", &Peripheral::read)
            .define_method < ByteArray(Peripheral::*)(
            BluetoothUUID const &, BluetoothUUID const &, BluetoothUUID const &)>("read", &Peripheral::read)
            .define_method("write_command",
                           &Peripheral::write_command) // takes (BluetoothUUID const &service, BluetoothUUID const &characteristic, ByteArray const &data)
            .define_method("unsubscribe",
                           &Peripheral::unsubscribe) // takes (BluetoothUUID const &service, BluetoothUUID const &characteristic)
            .define_method("write",
                           &Peripheral::write); // takes (BluetoothUUID const &service, BluetoothUUID const &characteristic, BluetoothUUID const &descriptor, ByteArray const &data)


    // TODO:
    // void notify(BluetoothUUID const &service, BluetoothUUID const &characteristic, std::function<void(ByteArray payload)> callback)
    // void indicate(BluetoothUUID const &service, BluetoothUUID const &characteristic, std::function<void(ByteArray payload)> callback)
    // void set_callback_on_connected(std::function<void()> on_connected)
    // void set_callback_on_disconnected(std::function<void()> on_disconnected) */
    // virtual ~Peripheral() = default
    // void *underlying() const (probably unneeded)

    Data_Type<Adapter> rb_cAdapter = define_class_under<Adapter>(rb_mSimpleRbBLE, "Adapter")
            .define_constructor(Constructor<Adapter>())
            .define_singleton_function("bluetooth_enabled", &Adapter::bluetooth_enabled) // returns bool
            .define_singleton_function("get_adapters", &Adapter::get_adapters) // returns vector<Adapter>
            .define_method("initialized", &Adapter::initialized)
            .define_method("identifier", &Adapter::identifier) // returns std::string
            .define_method("address", &Adapter::address) // returns BluetoothAddress (alias of std::string)
            .define_method("scan_start", &Adapter::scan_start)
            .define_method("scan_stop", &Adapter::scan_stop)
            .define_method("scan_for", &Adapter::scan_for) // takes (int timeout_ms)
            .define_method("scan_is_active", &Adapter::scan_is_active)
            .define_method("scan_get_results", &Adapter::scan_get_results) // returns std::vector<Peripheral>
            .define_method("get_paired_peripherals",
                           &Adapter::get_paired_peripherals); // returns std::vector<Peripheral>
    // FIXME: this is not working currently (segfaulting, IIRC). Need to figure out correct way to fire a ruby block 
    // on a C callback. Example @ https://github.com/jasonroelofs/rice/blob/master/sample/callbacks/sample_callbacks.cpp,
    // but I could be missing something.
    rb_cAdapter.define_method("set_callback_on_scan_found",[](Adapter *adapter, Rice::Object rbCb) -> auto {
        return adapter->set_callback_on_scan_found([rbCb](auto per) -> void { rbCb.call("call", per); });
    });

    // TODO:
    //    virtual ~Adapter()
    //    void set_callback_on_scan_start(std::function<void()> on_scan_start)
    //    void set_callback_on_scan_stop(std::function<void()> on_scan_stop)
    //    void set_callback_on_scan_updated(std::function<void(Peripheral)> on_scan_updated)
    //    void set_callback_on_scan_found(std::function<void(Peripheral)> on_scan_found)

}

#pragma clang diagnostic pop

