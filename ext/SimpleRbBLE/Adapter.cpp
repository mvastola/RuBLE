#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "modernize-avoid-bind"
#include "SimpleRbBLE.h"

#include <functional>
using namespace std::string_literals;

using OnAdapterScanFoundCallbackMap = std::map<std::string, std::shared_ptr<Data_Object<CallbackHolder>>>;
Adapter_DT rb_cAdapter;

CallbackHolder adapter_on_scan_found;
auto get_instance() { return Rice::detail::Registries::instance; }

void set_adapter_callback_on_scan_found(Adapter&  adapter, Object value) {
//    static OnAdapterScanFoundCallbackMap on_adapter_scan_found_callbacks {};
//    Data_Object<Adapter> adapter(adapterPtr);
//    Adapter *chPtr = detail::From_Ruby<Adapter*>().convert(rbAdapter);
//    std::cout << "Debug: in set_callback_on_scan_found:" << ((long unsigned) static_cast<void*>(adapter.get())) << std::endl << std::flush;
    std::cout << "Debug: rbAdapter " << adapter.address() << std::endl << std::flush;
    adapter_on_scan_found.set(value);
    std::cout << "Adapter Callback Set for " << adapter.address() << std::endl << std::flush;
//    rb_need_block();
//    VALUE rbCb = rb_block_proc();

//    const SimpleBLE::BluetoothAddress addr = adapter->address();
//    on_adapter_scan_found_callbacks[addr] = std::make_shared<Data_Object<CallbackHolder>>(cb);
//    adapter.iv_set("@scan_found_callback", ch);
//    std::function<void (SimpleBLE::Peripheral)> c_callback = [&addr] (Peripheral p) -> void {
//        std::cout << "Debug: in scan found cb: " << "foo" << std::endl << std::flush;
//        std::cout << "Debug: rbAdapter " << adapter->address() << std::endl << std::flush;
//        on_adapter_scan_found_callbacks[addr]->fire();
//    };
//    adapter->set_callback_on_scan_found(c_callback);

}

void fire_adapter_callback_on_scan_found(Adapter &adapter, Rice::Object value) {

//    std::cout << "Debug: in fire_callback_on_scan_found: " << ((long unsigned) static_cast<void*>(adapter.get())) << std::endl << std::flush;
//    Data_Object<Adapter> adapter(realAdapter.get(), false);

    std::cout << "Debug: rbAdapter " << adapter.address() << std::endl << std::flush;
    adapter_on_scan_found.fire(value);
//    Data_Object<Adapter> adapter(realAdapter, false);

//    (*globalCallback)->fire();

    //    auto cb = Data_Object<CallbackHolder>(globalCallbackPtr.get(), false);
//    cb->fire();
//    Data_Object<Adapter> adapter(realAdapter, false);
//    rb_cCallbackHolder
//    Object callback = adapter->iv_get("@scan_found_callback");
//    if (callback.is_nil()) {
//        std::cout << "@scan_found_callback is nil. Skipping." << std::endl << std::flush;
//        return;
//    }
//    callback.call("call", peripheral);
//    std::cout << std::flush;
}

//void setup_adapter_callbacks(std::shared_ptr<Adapter> adapter) {
//    std::cout << "Debug: in setup_callbacks: " << ((long unsigned) static_cast<void*>(adapter.get())) << std::endl << std::flush;

//    auto ownedAdapter = detail::From_Ruby<Adapter>::convert()
//    adapter->set_callback_on_scan_found([adapter] (Peripheral p) -> void {
//        std::cout << "Debug: cb " << (*globalCallback).inspect() << std::endl << std::flush;

//        globalCallbackPtr->fire();
//        auto cb = Data_Object<CallbackHolder>(globalCallbackPtr.get(), false);
//        cb->fire();
//        globalCallback->fire();
//        (*globalCallback)->fire();
//        fire_adapter_callback_on_scan_found(adapter, p);
//    });


void Init_Adapter() {
    rb_cAdapter = define_class_under<Adapter>(rb_mSimpleRbBLE, "Adapter");
//    auto adapterFoundCallbackMap = define_map_under<std::map<
//            std::string,
//            std::shared_ptr<Adapter>>
//            >(rb_mSimpleRbBLE, "adapterFoundCallbackMap");
    rb_cAdapter
            .define_constructor(Constructor<Adapter>())
            .define_singleton_function("bluetooth_enabled", &Adapter::bluetooth_enabled) // returns bool
            .define_singleton_function("get_adapters", &Adapter::get_adapters, Return().takeOwnership()) // returns vector<Adapter>
            .define_method("initialized", &Adapter::initialized)
            .define_method("identifier", &Adapter::identifier)
            .define_method("address", &Adapter::address) // returns BluetoothAddress (alias of std::string)
            .define_method("scan_start", &Adapter::scan_start)
            .define_method("scan_stop", &Adapter::scan_stop)
            .define_method("scan_for", &Adapter::scan_for) // takes (int timeout_ms)
            .define_method("scan_is_active", &Adapter::scan_is_active)
            .define_method("scan_get_results", &Adapter::scan_get_results) // returns vector<Peripheral>
            .define_method("get_paired_peripherals", &Adapter::get_paired_peripherals) // returns vector<Peripheral>

    // FIXME: this is not working currently (segfaulting, IIRC). Need to figure out correct way to fire a ruby block
    // on a C callback. Example @ https://github.com/jasonroelofs/rice/blob/master/sample/callbacks/sample_callbacks.cpp,
    // but I could be missing something.
    .define_method("set_callback_on_scan_found", set_adapter_callback_on_scan_found)
//    .define_method("set_callback_on_scan_found", &set_adapter_callback_on_scan_found, Arg("cb").keepAlive());
    .define_method("fire_callback_on_scan_found", &fire_adapter_callback_on_scan_found);

    // TODO:
    //    virtual ~Adapter()
    //    void set_callback_on_scan_start(std::function<void()> on_scan_start)
    //    void set_callback_on_scan_stop(std::function<void()> on_scan_stop)
    //    void set_callback_on_scan_updated(std::function<void(Peripheral)> on_scan_updated)
    //    void set_callback_on_scan_found(std::function<void(Peripheral)> on_scan_found)
}

#pragma clang diagnostic pop