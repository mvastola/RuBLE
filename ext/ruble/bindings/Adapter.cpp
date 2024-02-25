#include <ranges>

#include "types/declarations.hpp"
#include "bindings/Adapter.hpp"
#include "bindings/Peripheral.hpp"
#include "management/Registry.hpp"
#include "management/RegistryFactory.hpp"
#include "management/RubyQueue.hpp"
#include "utils/containers.hpp"

namespace RuBLE {
    using Rice::String;
    Adapter::Adapter(const SimpleBLE::Adapter &adapter) :
            _self(Adapter_DO(*this)),
            _adapter(std::make_shared<SimpleBLE::Adapter>(adapter)),
            _addr(_adapter->address()),
            _on_scan_start(std::make_shared<Callback>(1)),
            _on_scan_stop(std::make_shared<Callback>(1)),
            _on_scan_update(std::make_shared<Callback>(2)),
            _on_scan_find(std::make_shared<Callback>(2)),
            _peripheral_registry(peripheralRegistryFactory->registry(this)) {
        get().set_callback_on_scan_start([this]() {
            RubyQueue::FnType fn = [this]() -> void { _on_scan_start->fire(self()); };
            if (_on_scan_start) rubyQueue->push(fn);
        });

        get().set_callback_on_scan_stop([this]() {
            RubyQueue::FnType fn = [this]() -> void { _on_scan_stop->fire(self()); };
            if (_on_scan_stop) rubyQueue->push(fn);
        });

        get().set_callback_on_scan_updated([this](const SimpleBLE::Peripheral &peripheral) {
            RubyQueue::FnType fn = [this, peripheral]() -> void {
                const std::shared_ptr<Peripheral> &wrapped = _peripheral_registry->fetch(peripheral);
                _on_scan_update->fire(wrapped->self(), self());
            };
            if (_on_scan_update) rubyQueue->push(fn);
        });

        get().set_callback_on_scan_found([this](const SimpleBLE::Peripheral &peripheral) {
            RubyQueue::FnType fn = [this, peripheral]() -> void {
                const std::shared_ptr<Peripheral> &wrapped = _peripheral_registry->fetch(peripheral);
                _on_scan_find->fire(wrapped->self(), self());
            };
            if (_on_scan_find) rubyQueue->push(fn);
        });
    }

    bool Adapter::initialized() const { return get().initialized(); }

    std::string Adapter::identifier() { return (_adapter) ? get().identifier() : ""; }


    BluetoothAddress Adapter::address() const {
        return _addr; /*get().address();*/
    }

    void Adapter::scan_start() { if (!scan_is_active()) get().scan_start(); }

    void Adapter::scan_stop() { if (scan_is_active()) get().scan_stop(); }

    void Adapter::scan_for(int timeout_ms) { get().scan_for(timeout_ms); }

    bool Adapter::scan_is_active() const {
        return const_cast<Adapter*>(this)->get().scan_is_active();
    }

    bool Adapter::operator==(const SimpleBLE::BluetoothAddress &addr) const { return !_addr.empty() && _addr == addr; }

    bool Adapter::operator==(const Adapter &ap) const { return *this == ap._addr; }

    std::vector<std::shared_ptr<Peripheral>> Adapter::scan_get_results() {
        auto unwrappedPeripherals = get().scan_get_results();
        return _peripheral_registry->map_to_objects(unwrappedPeripherals);
    }

    Rice::Array Adapter::scan_get_ruby_results() {
        auto unwrappedPeripherals = get().scan_get_results();
        return _peripheral_registry->map_to_ruby_objects(unwrappedPeripherals);
    }

    std::vector<std::shared_ptr<Peripheral>> Adapter::get_paired_peripherals() {
        auto unwrappedPeripherals = get().get_paired_peripherals();
        return _peripheral_registry->map_to_objects(unwrappedPeripherals);
    }


    Rice::Array Adapter::get_ruby_paired_peripherals() {
        auto unwrappedPeripherals = get().get_paired_peripherals();
        return _peripheral_registry->map_to_ruby_objects(unwrappedPeripherals);
    }

    SimpleBLE::Adapter &Adapter::get() {
        if (!_adapter) throw std::runtime_error("Tried fetching address for unconfigured Adapter instance");
        return *_adapter;
    }

    const SimpleBLE::Adapter &Adapter::get() const {
        if (!_adapter) throw std::runtime_error("Tried fetching address for unconfigured Adapter instance");
        return *_adapter;
    }

    Object Adapter::self() const { return _self; }

    bool Adapter::bluetooth_enabled() { return SimpleBLE::Adapter::bluetooth_enabled(); }

    std::vector<std::shared_ptr<Adapter>> Adapter::get_adapters() {
        std::vector<SimpleBLE::Adapter> unwrappedAdapters = SimpleBLE::Adapter::get_adapters();
        return adapterRegistry->map_to_objects(unwrappedAdapters);
    }

    Rice::Array Adapter::get_ruby_adapters() {
        std::vector<SimpleBLE::Adapter> unwrappedAdapters = SimpleBLE::Adapter::get_adapters();
        return adapterRegistry->map_to_ruby_objects(unwrappedAdapters);
    }

    void Adapter::on_scan_start(Object new_cb) { _on_scan_start->set(new_cb); }

    void Adapter::on_scan_stop(Object new_cb) { _on_scan_stop->set(new_cb); }

    void Adapter::on_scan_update(Object new_cb) { _on_scan_update->set(new_cb); }

    void Adapter::on_scan_find(Object new_cb) { _on_scan_find->set(new_cb); }

    std::unordered_set<std::string> Adapter::status_flags() const {
        using PossibleFlagMap = std::unordered_map<std::string, std::function<bool(const Adapter&)>>;
        static const PossibleFlagMap possible_flag_fns {
                { "initialized"s, &Adapter::initialized    },
                { "scanning"s,    &Adapter::scan_is_active },
        };
        static auto flag_check = [this](const PossibleFlagMap::value_type &pair) -> bool {
            return pair.second(*this);
        };

        ranges::viewable_range auto flags = possible_flag_fns | views::filter(flag_check) | views::keys;
        return { flags.begin(), flags.end() };
    }

    std::string Adapter::to_s() const {
        std::ostringstream oss;
        String superStr(rb_call_super(0, nullptr));
        if (superStr.test() && superStr.length() > 0) {
            std::string super(superStr.str());
            super.pop_back();
            oss << super;
        } else {
            oss << "#<" << Utils::human_type_name<decltype(*this)>();
            oss << ":" << Utils::to_hex_addr(this);
        }
        oss << " ";
        if (!initialized())  return oss.str() + "uninitialized>";
        oss << "@address=\"" << address() << "\" ";
        oss << Utils::join(status_flags(), "|");
        oss << ">";
        return oss.str();
    }

    void Adapter::ruby_mark() const {
//    std::cout << "ruby_mark-ing adapter proxy: " << address() << std::endl;
//    rb_gc_mark(self());
        Rice::ruby_mark<RuBLE::PeripheralRegistry>(_peripheral_registry.get());
//        rb_gc_mark(_peripheral_registry->self());
        if (_on_scan_start) Rice::ruby_mark<RuBLE::Callback>(_on_scan_start.get());
        if (_on_scan_stop) Rice::ruby_mark<RuBLE::Callback>(_on_scan_stop.get());
        if (_on_scan_update) Rice::ruby_mark<RuBLE::Callback>(_on_scan_update.get());
        if (_on_scan_find) Rice::ruby_mark<RuBLE::Callback>(_on_scan_find.get());
    }

    void Init_Adapter() {
        define_class_under<SimpleBLE::Adapter>(rb_mRuBLEUnderlying, "SimpleBLEAdapter");
        rb_cAdapter = define_class_under<Adapter>(rb_mRuBLE, "Adapter")
                .define_singleton_function("bluetooth_enabled?", &Adapter::bluetooth_enabled) // returns bool
                .define_singleton_function("get_adapters", &Adapter::get_ruby_adapters) // returns vector<Adapter>
                .define_method("initialized?", &Adapter::initialized)
                .define_method("identifier", &Adapter::identifier)
                .define_method("address", &Adapter::address) // returns BluetoothAddress (alias of std::string)
                .define_method("scan_start", &Adapter::scan_start)
                .define_method("scan_stop", &Adapter::scan_stop)
                .define_method("scan_for", &Adapter::scan_for) // takes (int timeout_ms)
                .define_method("scanning?", &Adapter::scan_is_active)
                .define_method("scan_get_results", &Adapter::scan_get_ruby_results) // returns vector<Peripheral>
                .define_method("get_paired_peripherals",
                               &Adapter::get_ruby_paired_peripherals) // returns vector<Peripheral>
                .define_method("on_scan_start", &Adapter::on_scan_start, Arg("cb").keepAlive() = Qnil)
                .define_method("on_scan_stop", &Adapter::on_scan_stop, Arg("cb").keepAlive() = Qnil)
                .define_method("on_scan_update", &Adapter::on_scan_update, Arg("cb").keepAlive() = Qnil)
                .define_method("on_scan_find", &Adapter::on_scan_find, Arg("cb").keepAlive() = Qnil)
                ;
//        define_class_under<std::shared_ptr<RuBLE::Adapter>>(rb_mRuBLE, "AdapterPtr");
    }
}


