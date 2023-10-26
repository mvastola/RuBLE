#include "common.hpp"
#include "helpers/NamedBitSet.hpp"
#include "Peripheral.hpp"
#include "RubyQueue.hpp"
#include "helpers/Callback.hpp"
#include "Service.hpp"
#include "Registry.hpp"
#include "helpers/RegistryFactory.hpp"

namespace SimpleRbBLE {
    Peripheral::Peripheral(const SimpleBLE::Peripheral &peripheral,
                           Owner *owner) :
            _owner(owner),
            _peripheral(std::make_shared<SimpleBLE::Peripheral>(peripheral)),
            _addr(_peripheral->address()),
            _addr_type(_peripheral->address_type()),
            _on_connected(std::make_shared<Callback>(1)),
            _on_disconnected(std::make_shared<Callback>(1)),
            _self(Peripheral_DO(*this)),
            _service_registry(serviceRegistryFactory->registry(this)) {
        _peripheral->set_callback_on_connected([this]() {
            RubyQueue::FnType fn = [this]() -> void { _on_connected->fire(self()); };
            if (_on_connected) rubyQueue->push(fn);
        });
        _peripheral->set_callback_on_disconnected([this]() {
            RubyQueue::FnType fn = [this]() -> void { _on_disconnected->fire(self()); };
            if (_on_disconnected) rubyQueue->push(fn);
        });
    }


    bool Peripheral::initialized() const { return _peripheral->initialized(); }

    std::string Peripheral::identifier() const { return _peripheral->identifier(); }


    std::map<uint16_t, ByteArray> Peripheral::manufacturer_data() const {
        auto data = _peripheral->manufacturer_data();
        return { data.begin(), data.end() };
    }

    Object Peripheral::self() const {
        return _self;
    }

    const Peripheral::Owner *Peripheral::owner() const { return _owner; }
    Peripheral::Owner *Peripheral::owner() { return _owner; }
    SimpleBLE::Peripheral &Peripheral::get() { return *_peripheral; }
    const SimpleBLE::Peripheral &Peripheral::get() const { return *_peripheral; }

    bool Peripheral::is_connected() const { return _peripheral->is_connected(); }

    bool Peripheral::is_connectable() const { return _peripheral->is_connectable(); }

    bool Peripheral::is_paired() const { return _peripheral->is_paired(); }

    Peripheral::StatusFlagSet Peripheral::status_flags() const {
        using StatusFlagTestMap = std::unordered_map<std::string, std::function<bool(const Peripheral&)>>;
        static const StatusFlagTestMap status_flag_testers {
                {"initialized"s, &Peripheral::initialized},
                {"paired"s,      &Peripheral::is_paired},
                {"connected"s,   &Peripheral::is_connected},
                {"connectable"s, &Peripheral::is_connectable}
        };
        return { status_flag_testers, *this };
    }

    std::vector<std::string_view> Peripheral::statuses() const {
        return status_flags().flag_names();
    }

    int16_t Peripheral::rssi() const { return _peripheral->rssi(); }

    int16_t Peripheral::tx_power() const { return _peripheral->tx_power(); }

    uint16_t Peripheral::mtu() const { return _peripheral->mtu(); }

    void Peripheral::connect() { _peripheral->connect(); }

    void Peripheral::disconnect() { _peripheral->disconnect(); }

    void Peripheral::unpair() { _peripheral->unpair(); }

    const std::map<BluetoothUUID, std::shared_ptr<Service>> &Peripheral::services() const {
        if (!rb_during_gc()) {
            // we can't create new services during GC
            for (const auto &c : _peripheral->services()) _service_registry->fetch(c);
        }
        return _service_registry->data();
//        std::vector<SimpleBLE::Service> unwrappedServices = _peripheral->services();
//        auto objects = _service_registry->map_to_objects(unwrappedServices);
//        std::function<BluetoothUUID(const Service&)> mapFn = &Service::uuid;
//        std::map<BluetoothUUID, std::shared_ptr<Service>> result =
//                map_by<BluetoothUUID, std::shared_ptr<Service>>(objects, mapFn);
//        return result;
    }

    std::shared_ptr<Service> Peripheral::operator[](const BluetoothUUID &svcUuid) const {
        // only check SimpleBLE backend for descriptor if not already instantiated
        if (_service_registry || !_service_registry->contains(svcUuid)) const auto _result = services();
        return _service_registry->at(svcUuid);
        // TODO(?): catch (std::out_of_range &ex)
    }

    ByteArray Peripheral::read(const BluetoothUUID &service, const BluetoothUUID &characteristic) {
        return _peripheral->read(service, characteristic);
    }

    void Peripheral::write_request(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                                   ByteArray data) {
        return _peripheral->write_request(service, characteristic, std::move(data));
    }

    void Peripheral::write_command(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                                   ByteArray data) {
        return _peripheral->write_command(service, characteristic, std::move(data));
    }

    void Peripheral::notify(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                            std::function<void(ByteArray)> callback) {
        // FIXME: make sure we don't have a GC problem with this callback
        return _peripheral->notify(service, characteristic, callback);
    }

    void Peripheral::indicate(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                              std::function<void(ByteArray)> callback) {
        // FIXME: make sure we don't have a GC problem with this callback
        return _peripheral->indicate(service, characteristic, callback);
    }

    void Peripheral::unsubscribe(const BluetoothUUID &service, const BluetoothUUID &characteristic) {
        return _peripheral->unsubscribe(service, characteristic);
    }

    ByteArray Peripheral::read(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                               const BluetoothUUID &descriptor) {
        return _peripheral->read(service, characteristic, descriptor);
    }

    void Peripheral::write(const BluetoothUUID &service, const BluetoothUUID &characteristic,
                           const BluetoothUUID &descriptor, ByteArray data) {
        return _peripheral->write(service, characteristic, descriptor, std::move(data));
    }

//bool Peripheral::operator==(const Peripheral &p) const {
//    return _peripheral && _peripheral->underlying() == p.underlying();
//}

//bool Peripheral::operator==(const Peripheral &p) const { return _peripheral == p._peripheral; }

    void Peripheral::on_connect(Object on_connect) {
        _on_connected->set(on_connect);
    }

    void Peripheral::on_disconnect(Object on_disconnect) {
        _on_disconnected->set(on_disconnect);
    }

    std::string Peripheral::to_s() const {
        std::ostringstream oss;
        oss << basic_object_inspect_start(*this) << " ";
        if (!initialized())  return oss.str() + "uninitialized>";
        oss << "@address[" << enum_val_as_string(address_type()) << "]=\"" << address() << "\" ";
        oss << "@identifier=\"" << identifier() << "\" ";
        oss << "@rssi=" << rssi() << " ";
        oss << "@tx_power=" << tx_power() << " ";
        oss << "@mtu=" << mtu() << " ";
        oss << "svc_count=" << _peripheral->services().size() << " ";
        oss << join(statuses(), "|");
//        std::map<uint16_t, SimpleBLE::ByteArray> manufacturer_data() const;
        oss << ">";
        return oss.str();
    }



    void Init_Peripheral() {
        define_class_under<SimpleBLE::Peripheral>(rb_mSimpleRbBLEUnderlying, "Peripheral");
        rb_cPeripheral = define_class_under<Peripheral>(rb_mSimpleRbBLE, "Peripheral")
//                .define_constructor(Constructor<Peripheral>())
                .define_method("inspect", &Peripheral::to_s)
                .define_method("initialized?", &Peripheral::initialized)
                .define_method("identifier", &Peripheral::identifier)
                .define_method("address", &Peripheral::address) // returns BluetoothAddress (alias for string)
                .define_method("address_type", &Peripheral::address_type)
                .define_method("rssi", &Peripheral::rssi)
                .define_method("tx_power", &Peripheral::tx_power)
                .define_method("mtu", &Peripheral::mtu)
                .define_method("connect", &Peripheral::connect)
                .define_method("disconnect", &Peripheral::disconnect)
                .define_method("connected?", &Peripheral::is_connected)
                .define_method("connectable?", &Peripheral::is_connectable)
                .define_method("paired?", &Peripheral::is_paired)
                .define_method("unpair", &Peripheral::unpair)
                        // TODO: this isn't returning any useful data. Should we keep it? (Or is there a way to parse?)
                .define_method("manufacturer_data", &Peripheral::manufacturer_data)
                .define_method("on_connect", &Peripheral::on_connect, Arg("cb").keepAlive())
                .define_method("on_disconnect", &Peripheral::on_disconnect, Arg("cb").keepAlive())
                .define_singleton_attr("unpair_all_on_exit", &Peripheral::unpair_all_on_exit, Rice::AttrAccess::ReadWrite)
                ;
        define_class_under<std::shared_ptr<SimpleRbBLE::Peripheral>>(rb_mSimpleRbBLE, "PeripheralPtr");
    }
}

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::Peripheral>(SimpleRbBLE::Peripheral *peripheral) {
        rb_gc_mark(peripheral->self());
        if (peripheral->_on_connected) Rice::ruby_mark<SimpleRbBLE::Callback>(peripheral->_on_connected.get());
        if (peripheral->_on_disconnected) Rice::ruby_mark<SimpleRbBLE::Callback>(peripheral->_on_disconnected.get());
        Rice::ruby_mark(peripheral->_service_registry.get());

//        if (peripheral->_on_connected) rb_gc_mark(peripheral->_on_connected->self());
//        if (peripheral->_on_disconnected) rb_gc_mark(peripheral->_on_disconnected->self());
    }
}

std::size_t std::hash<SimpleBLE::Peripheral>::operator()(const SimpleBLE::Peripheral &p) const noexcept {
    SimpleBLE::BluetoothAddress addr = const_cast<SimpleBLE::Peripheral &>(p).address();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::BluetoothAddress>{};
    return h1 ^ (hasher(addr) << 1);
}

std::size_t std::hash<SimpleRbBLE::Peripheral>::operator()(const SimpleRbBLE::Peripheral &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Peripheral>{};
    return h1 ^ (hasher(p.get()) << 1);
}