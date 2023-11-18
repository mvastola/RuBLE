#include "types/declarations.hpp"
#include "utils/exception_handling.hpp"

using namespace std::chrono_literals;

namespace RuBLE {
    static_assert(!AdapterRegistry::is_owned);
    static_assert(PeripheralRegistry::is_owned);
    static_assert(ServiceRegistry::is_owned);
    static_assert(CharacteristicRegistry::is_owned);

    [[maybe_unused]] InRuby::Guard main_thread_in_ruby;


    void Init_BluetoothAddressType() {
        rb_cBluetoothAddressType = Utils::define_described_enum<BluetoothAddressType>("BluetoothAddressType", rb_mRuBLE);
    }

    void Init_DeInit() {
        rb_mRuBLE
            .define_singleton_function("shutdown", &DeInit); // to make it easier to break in gdb from ruby
        rb_set_end_proc(DeInit, Qnil);
    }

    void DeInit(VALUE = Qnil) {
        static std::atomic_flag called = false;
        // if called is already true, this is a no-op
        if (called.test_and_set()) return;


        // todo: confirm this only applies to peripherals we ourselves have paired
        if (adapterRegistry && Peripheral::unpair_all_on_exit) {
            adapterRegistry->for_each([](std::shared_ptr<Adapter> &adapter) {
                if (adapter->scan_is_active()) adapter->scan_stop();
                adapter->_peripheral_registry->for_each([](auto &peripheral) {
                    if (peripheral->is_paired()) peripheral->unpair();
                    if (peripheral->is_connected()) {
                        std::cout << "Peripheral " << peripheral << " is connected. Disconnecting on shutdown." << std::endl;
                        peripheral->disconnect();
                    }
                });
            });
        }

        // Wait to ensure rubyQueue has stopped (if it hasn't yet)
        RubyQueue::stop_on_exit(Qnil);
//        rubyQueue->wait_for_stop();

        // TODO(?): clear factories so ruby objects deallocate in this instance
        //  (we might not be segfaulting anymore, making this unneeded)
    }

    [[maybe_unused]] void c_debug() {
#ifdef RUBLE_DEBUG
        std::cerr << "Insert gbb breakpoint here" << std::endl;
#endif
    }

    void Init_ExceptionHandling() {
        ExceptionHandling::wrap_termination_handler();
//        register_handler<Rice::Exception>(RuBLE::handle_exception<Rice::Exception>);
    }

    void Init_Utils() {
        rb_mUtils = Rice::define_module_under(rb_mRuBLE, "Utils");

        if constexpr (!DEBUG) return;
        rb_mUtils.define_module_function("throw_cxx_exception", ExceptionHandling::throw_exception);
    }

    //static_assert(Identifiers::PartiallyIdentifiableResource<Adapter>);
    //static_assert(std::same_as<RuBLE::Identifiers::IDTypes::Full<RuBLE::Adapter, true>::tuple, std::tuple<std::string>>);

    // See https://jasonroelofs.com/rice/4.x
    void Init_RuBLE() {
        RuBLE::main_thread_in_ruby = RuBLE::in_ruby.assert_in_ruby_guard();
        assert(RuBLE::main_thread_in_ruby);
        Init_ExceptionHandling();

    #ifdef HAVE_VALGRIND
        VALGRIND_PRINTF_BACKTRACE("Enabling leak checking now.\n");
        VALGRIND_CLO_CHANGE("--leak-check=full");
    #endif
        Rice::detail::Registries::instance.instances.isEnabled = true;
        rb_mRuBLE = Rice::define_module("RuBLE");
//        rb_mRuBLE.
        rubyQueue = RubyQueue::instance();

    #ifdef RUBLE_DEBUG
        std::cout << "RubyQueue: " << Utils::to_hex_addr(rubyQueue.get()) << std::endl;
        rb_mRuBLE.define_singleton_function("c_debug", &c_debug); // to make it easier to break in gdb from ruby
    #endif
        rb_mRuBLEUnderlying = Rice::define_module_under(rb_mRuBLE, "Underlying");
        Init_Utils();
        Init_BluetoothAddressType();
        Init_ByteArray();
        Init_Descriptor();
        Init_Characteristic();
        Init_Service();
        Init_Peripheral();
        Init_Adapter();
        Init_Registries();
        Init_DeInit();

        rubyQueue->start();
        rubyQueue->ensure_started();
    }
}

extern "C" [[maybe_unused]] void Init_RuBLE() {
    RuBLE::Init_RuBLE();
}

extern "C" [[maybe_unused]] void Init_ruble() {
    RuBLE::Init_RuBLE();
}