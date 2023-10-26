#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "modernize-avoid-bind"

#include "common.hpp"
using namespace std::chrono_literals;

namespace SimpleRbBLE {
    static_assert(!AdapterRegistry::is_owned);
    static_assert(PeripheralRegistry::is_owned);
    static_assert(ServiceRegistry::is_owned);
    static_assert(CharacteristicRegistry::is_owned);

    InRuby::Guard main_thread_in_ruby;
    std::shared_ptr<RubyQueue> rubyQueue;
    Module rb_mSimpleRbBLE;
    Module rb_mSimpleRbBLEUnderlying;
    BluetoothAddressType_DT rb_cBluetoothAddressType;

    bool Peripheral::unpair_all_on_exit = true;

    void Init_BluetoothAddressType() {
        rb_cBluetoothAddressType = define_enum<BluetoothAddressType>("BluetoothAddressType", rb_mSimpleRbBLE)
                .define_value("PUBLIC", BluetoothAddressType::PUBLIC)
                .define_value("RANDOM", BluetoothAddressType::RANDOM)
                .define_value("UNSPECIFIED", BluetoothAddressType::UNSPECIFIED);
    }

    void Init_DeInit() {
        rb_mSimpleRbBLE
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
#ifdef SIMPLERBBLE_DEBUG
        std::cerr << "Insert gbb breakpoint here" << std::endl;
#endif
    }

    template <class Exception> requires std::derived_from<Exception, std::exception>
    constexpr void handle_exception(const Exception& ex) {
        try {
            std::cerr << "Ruby Backtrace: " << std::endl;
            rb_backtrace();
            std::cerr << std::endl << std::endl;
        } catch (const std::exception &ex2) {}
        try {
            std::cerr << "C++ Backtrace: " << std::endl;
            Exceptions::throw_with_trace(ex);
        } catch (const std::exception &ex2) {
          throw Exception(ex);
        }
    }
    void Init_ExceptionHandling() {
//        register_handler<Rice::Exception>(SimpleRbBLE::handle_exception<Rice::Exception>);
        static std::terminate_handler old_terminate_handler = std::get_terminate();
        std::set_terminate([] {
            try {
                std::cerr << boost::stacktrace::stacktrace();
            } catch (...) {}
            try {
                rb_backtrace();
//                ensure_ruby<void*>([]() { rb_backtrace(); return nullptr; });
            } catch (...) {}
            if (old_terminate_handler != nullptr) old_terminate_handler();
//            std::abort();
        });
    }
}

using namespace SimpleRbBLE;
using namespace SimpleRbBLE::Exceptions;

static_assert(Identifiers::PartiallyIdentifiableResource<Adapter>);
static_assert(std::same_as<SimpleRbBLE::Identifiers::IDTypes::Full<SimpleRbBLE::Adapter, true>::tuple, std::tuple<std::string>>);

// See https://jasonroelofs.com/rice/4.x
extern "C" [[maybe_unused]] void Init_SimpleRbBLE() {
    SimpleRbBLE::main_thread_in_ruby = SimpleRbBLE::in_ruby.assert_guard();
    Init_ExceptionHandling();

#ifdef SIMPLERBBLE_DEBUG
    VALGRIND_PRINTF_BACKTRACE("Enabling leak checking now.\n");
    VALGRIND_CLO_CHANGE("--leak-check=full");
#endif
    Rice::detail::Registries::instance.instances.isEnabled = true;
    rb_mSimpleRbBLE = define_module("SimpleRbBLE");
    rubyQueue = RubyQueue::instance();

#ifdef SIMPLERBBLE_DEBUG
    std::cout << "RubyQueue: " << to_hex_addr(rubyQueue.get()) << std::endl;
    rb_mSimpleRbBLE.define_singleton_function("c_debug", &c_debug); // to make it easier to break in gdb from ruby
#endif
    rb_mSimpleRbBLEUnderlying = define_module_under(rb_mSimpleRbBLE, "Underlying");
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
