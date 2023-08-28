#include "SimpleRbBLE.h"

Data_Type<Adapter> Init_SimpleRbBLEAdapter(Module &rb_mSimpleRbBLE) {
    Data_Type<Adapter> rb_cAdapter = define_class_under<Adapter>(rb_mSimpleRbBLE, "Adapter")
            .define_constructor(Constructor<Adapter>(), Return().keepAlive())
            .define_singleton_function("bluetooth_enabled", &Adapter::bluetooth_enabled) // returns bool
            .define_singleton_function("get_adapters", &Adapter::get_adapters) // returns vector<Adapter>
            .define_method("initialized", &Adapter::initialized)
            .define_method("identifier", &Adapter::identifier)
            .define_method("address", &Adapter::address) // returns BluetoothAddress (alias of std::string)
            .define_method("scan_start", &Adapter::scan_start)
            .define_method("scan_stop", &Adapter::scan_stop)
            .define_method("scan_for", &Adapter::scan_for) // takes (int timeout_ms)
            .define_method("scan_is_active", &Adapter::scan_is_active)
            .define_method("scan_get_results", &Adapter::scan_get_results) // returns vector<Peripheral>
            .define_method("get_paired_peripherals", &Adapter::get_paired_peripherals); // returns vector<Peripheral>
    // FIXME: this is not working currently (segfaulting, IIRC). Need to figure out correct way to fire a ruby block
    // on a C callback. Example @ https://github.com/jasonroelofs/rice/blob/master/sample/callbacks/sample_callbacks.cpp,
    // but I could be missing something.
    rb_cAdapter.define_method("set_callback_on_scan_found", [&rb_cAdapter](Adapter *realAdapter, Object rbCb) -> void {
        std::cout << "Debug: in set_callback_on_scan_found" << std::endl;
//        rb_need_block();
//        VALUE rbCb = rb_block_proc();
//        Object cbrb_obj_dup()
//        rb_gc_mark(rbCb);
        Data_Object<Adapter> adapter(realAdapter, false);
        auto ch = new CallbackHolder();
        ch->set(rbCb);
//        rb_gc_mark(static_cast<VALUE>(cbHolder.value()));
        adapter.iv_set("@scan_found_callback", ch->callback_);


        return adapter->set_callback_on_scan_found([realAdapter,adapter](const Peripheral &per) -> auto {
            std::cout << "Debug: in scan found callback" << std::endl;
            std::cout << "Debug: rbAdapter " << realAdapter->address() << std::endl;
            Object ivCb = adapter.iv_get("@scan_found_callback");
            ivCb.call("call");
//            return Qnil;
        });

    });
    return rb_cAdapter;

    // TODO:
    //    virtual ~Adapter()
    //    void set_callback_on_scan_start(std::function<void()> on_scan_start)
    //    void set_callback_on_scan_stop(std::function<void()> on_scan_stop)
    //    void set_callback_on_scan_updated(std::function<void(Peripheral)> on_scan_updated)
    //    void set_callback_on_scan_found(std::function<void(Peripheral)> on_scan_found)
}
