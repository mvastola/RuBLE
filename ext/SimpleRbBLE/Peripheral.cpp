#include "SimpleRbBLE.h"

Data_Type<Peripheral> Init_SimpleRbBLEPeripheral(Module &rb_mSimpleRbBLE) {
    using PeripheralReadMethod1 = ByteArray(Peripheral::*)(BluetoothUUID const &, BluetoothUUID const &);
    using PeripheralReadMethod2 = ByteArray(Peripheral::*)(BluetoothUUID const &, BluetoothUUID const &,
                                                           BluetoothUUID const &);

    // TODO: why is CLion indenting like this? (Need to fix code style settings)
    return define_class_under<Peripheral>(rb_mSimpleRbBLE, "Peripheral")
            .define_constructor(Constructor<Peripheral>())
            .define_method("initialized", &Peripheral::initialized)
            .define_method("identifier", &Peripheral::identifier)
            .define_method("address", &Peripheral::address) // returns BluetoothAddress (alias for string)
            .define_method("address_type", &Peripheral::address_type)
            .define_method("rssi", &Peripheral::rssi)
            .define_method("tx_power", &Peripheral::tx_power)
            .define_method("mtu", &Peripheral::mtu)
            .define_method("connect", &Peripheral::connect)
            .define_method("disconnect", &Peripheral::disconnect)
            .define_method("is_connected", &Peripheral::is_connected)
            .define_method("is_connectable", &Peripheral::is_connectable)
            .define_method("is_paired", &Peripheral::is_paired)
            .define_method("unpair", &Peripheral::unpair)
            .define_method("services", &Peripheral::services)
                    // TODO: this isn't returning any useful data. Should we keep it? (Or is there a way to parse?)
            .define_method("manufacturer_data", &Peripheral::manufacturer_data)
            .define_method("write_request", &Peripheral::write_request)
            .define_method<PeripheralReadMethod1>("read", &Peripheral::read)
            .define_method<PeripheralReadMethod2>("read", &Peripheral::read)
            .define_method("write_command", &Peripheral::write_command)
            .define_method("unsubscribe", &Peripheral::unsubscribe)
            .define_method("write", &Peripheral::write);

    // TODO:
    // void notify(BluetoothUUID const &service, BluetoothUUID const &characteristic, std::function<void(ByteArray payload)> callback)
    // void indicate(BluetoothUUID const &service, BluetoothUUID const &characteristic, std::function<void(ByteArray payload)> callback)
    // void set_callback_on_connected(std::function<void()> on_connected)
    // void set_callback_on_disconnected(std::function<void()> on_disconnected) */
    // virtual ~Peripheral() = default
    // void *underlying() const (probably unneeded)

}
