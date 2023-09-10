#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "modernize-avoid-bind"
#include "SimpleRbBLE.h"
#include "AdapterProxyRegistry.h"
#include <functional>
using namespace std::string_literals;

Adapter_DT rb_cAdapter;

void Init_Adapter() {
    
//    auto adapterFoundCallbackMap = define_map_under<std::map<
//            std::string,
//            std::shared_ptr<Adapter>>
//            >(rb_mSimpleRbBLE, "adapterFoundCallbackMap");
    rb_cAdapter = define_class_under<AdapterProxy>(rb_mSimpleRbBLE, "Adapter")
//            .define_constructor(Constructor<AdapterProxy>())
            .define_singleton_function("bluetooth_enabled", &AdapterProxy::bluetooth_enabled) // returns bool
            .define_singleton_function("get_adapters", &AdapterProxy::get_adapters, Return().takeOwnership()) // returns vector<Adapter>
            .define_method("initialized", &AdapterProxy::initialized)
            .define_method("identifier", &AdapterProxy::identifier)
            .define_method("address", &AdapterProxy::address) // returns BluetoothAddress (alias of std::string)
            .define_method("scan_start", &AdapterProxy::scan_start)
            .define_method("scan_stop", &AdapterProxy::scan_stop)
            .define_method("scan_for", &AdapterProxy::scan_for) // takes (int timeout_ms)
            .define_method("scan_is_active", &AdapterProxy::scan_is_active)
            .define_method("scan_get_results", &AdapterProxy::scan_get_results) // returns vector<Peripheral>
            .define_method("get_paired_peripherals", &AdapterProxy::get_paired_peripherals) // returns vector<Peripheral>
            .define_method("on_scan_start", &AdapterProxy::on_scan_start, Arg("cb").keepAlive())
            .define_method("on_scan_stop", &AdapterProxy::on_scan_stop, Arg("cb").keepAlive())
            .define_method("on_scan_update", &AdapterProxy::on_scan_update, Arg("cb").keepAlive())
            .define_method("on_scan_find", &AdapterProxy::on_scan_find, Arg("cb").keepAlive())
            .define_method("fire_scan_started", &AdapterProxy::fire_on_scan_started)
            .define_method("fire_scan_stopped", &AdapterProxy::fire_on_scan_stopped)
            .define_method("fire_scan_updated", &AdapterProxy::fire_on_scan_updated, Arg("peripheral").keepAlive())
            .define_method("fire_scan_found", &AdapterProxy::fire_on_scan_found, Arg("peripheral").keepAlive())
            ;


    // FIXME: Callbacks are not working currently (segfaulting, IIRC). Need to figure out correct way to implement
    // on a C callback. Example @ https://github.com/jasonroelofs/rice/blob/master/sample/callbacks/sample_callbacks.cpp,
    // but I could be missing something.
    //    void set_callback_on_scan_start(std::function<void()> on_scan_start)
    //    void set_callback_on_scan_stop(std::function<void()> on_scan_stop)
    //    void set_callback_on_scan_updated(std::function<void(Peripheral)> on_scan_updated)
    //    void set_callback_on_scan_found(std::function<void(Peripheral)> on_scan_found)
}

#pragma clang diagnostic pop