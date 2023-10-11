#include "common.h"
#include "Peripheral.h"
#include "Service.h"
#include "Characteristic.h"
#include "Descriptor.h"

#include <utility>
#include "RubyQueue.h"
#include "Callback.h"

namespace SimpleRbBLE {
    Characteristic::Characteristic(const SimpleBLE::Characteristic &characteristic,
                                   Owner *owner) :
            _owner(owner),
            _characteristic(std::make_shared<SimpleBLE::Characteristic>(characteristic)),
            _uuid(_characteristic->uuid()),
            _self(DataObject(*this)) {}

    Object Characteristic::self() const {
        return _self;
    }

    SimpleBLE::Characteristic &Characteristic::get() { return *_characteristic; }
    const SimpleBLE::Characteristic &Characteristic::get() const { return *_characteristic; }
    constexpr const Characteristic::Owner *Characteristic::owner() const { return _owner; }
    constexpr Characteristic::Owner *Characteristic::owner() { return _owner; }
    constexpr const Service *Characteristic::service() const { return owner(); }
    constexpr Service *Characteristic::service() { return owner(); }


    const std::map<BluetoothUUID, std::shared_ptr<Descriptor>> &Characteristic::descriptors() const {
        if (_descriptors) return *_descriptors;
        auto result = Descriptor::make_map(_characteristic->descriptors(), const_cast<Characteristic*>(this));
        _descriptors = std::move(result);
        return *_descriptors;
    }

    std::shared_ptr<Descriptor> Characteristic::operator[](const BluetoothUUID &descUuid) const {
        // only check SimpleBLE backend for descriptor if not already instantiated
        if (!_descriptors || !_descriptors->contains(descUuid)) const auto _result = descriptors();
        return _descriptors->at(descUuid);
        // TODO(?): catch (std::out_of_range &ex)
    }

    constexpr const Characteristic::CababilityFlags &Characteristic::capabilities() const {
        if (_capabilities) return *_capabilities;
        _capabilities = std::make_optional<CababilityFlags>(_characteristic->capabilities());
        return *_capabilities;
    }

    std::vector<std::string> Characteristic::capability_names() const { return capabilities().flag_name_strs(); }

    bool Characteristic::can_read() const { return capabilities()["read"]; }
    bool Characteristic::can_write_request() const { return capabilities()["write_request"]; }
    bool Characteristic::can_write_command() const { return capabilities()["write_command"]; }
    bool Characteristic::can_notify() const { return capabilities()["notify"]; }
    bool Characteristic::can_indicate() const { return capabilities()["indicate"]; }


    ConvertableByteArray Characteristic::read() {
        return service()->read(uuid());
    }
    ConvertableByteArray Characteristic::read(const BluetoothUUID &descriptor) {
        return service()->read(uuid(), descriptor);
    }
    void Characteristic::write(const BluetoothUUID &descriptor, ConvertableByteArray data) {
        service()->write(uuid(), descriptor, std::move(data));
    }
    void Characteristic::write_request(ConvertableByteArray data) {
        service()->write_request(uuid(), std::move(data));
    }
    void Characteristic::write_command(ConvertableByteArray data) {
        service()->write_command(uuid(), std::move(data));
    }
    void Characteristic::set_on_notify(Object cb) {
        if (!_on_notify) {
            _on_notify = std::make_shared<Callback>();
            service()->notify(uuid(), [this](const ConvertableByteArray& data) {
                RubyQueue::FnType fn = std::bind(&Characteristic::fire_on_notify, std::cref(*this), data);  // NOLINT(*-avoid-bind)
                rubyQueue->push(fn);
            });
        }
        _on_notify->set(std::move(cb));
    }

    void Characteristic::set_on_indicate(Object cb) {
        if (!_on_indicate) {
            _on_indicate = std::make_shared<Callback>();
            service()->indicate(uuid(), [this](const ConvertableByteArray& data) {
                RubyQueue::FnType fn = std::bind(&Characteristic::fire_on_indicate, std::cref(*this), data);  // NOLINT(*-avoid-bind)
                rubyQueue->push(fn);
            });
        }
        _on_indicate->set(std::move(cb));
    }

    void Characteristic::fire_on_notify(const ConvertableByteArray &data) const {
        if (_on_notify) _on_notify->fire(Rice::detail::To_Ruby<ByteArray>().convert(data));
    }

    void Characteristic::fire_on_indicate(const ConvertableByteArray &data) const {
        if (_on_indicate) _on_indicate->fire(Rice::detail::To_Ruby<ByteArray>().convert(data));
    }

    void Characteristic::unsubscribe() {
        service()->unsubscribe(uuid());
    }

    std::string Characteristic::to_s() const {
        std::ostringstream oss;
        String superStr(rb_call_super(0, nullptr));
        if (superStr.test() && superStr.length() > 0) {
            std::string super(superStr.str());
            super.pop_back();
            oss << super;
        } else {
            oss << "#<" << human_type_name<decltype(*this)>();
            oss << ":0x" << std::hex << reinterpret_cast<uint64_t>(this) << std::dec;
        }
        oss << " ";
        oss << "@uuid=" << uuid() << " ";
        oss << "@capabilities=" << capabilities();
//        oss << "#characteristics=" << characteristics().size();
        oss << ">";
        return oss.str();
    }

    Rice::String Characteristic::inspect() const {
        return to_s();
    }

    Characteristic_DT rb_cCharacteristic;
    CharacteristicCapabilityType_DT rb_cCharacteristicCapabilityType;
    void Init_Characteristic() {

        // FIXME: A bunch of these classes inherit from abstract base classes (e.g. BaseAdapter),
        // and are also subclassed in different implementations (I think).
        // Do we need the constructor and methods to return/receive pointers to work well
        // (so the right virtual functions are called)? Not sure if rice is handling that all,
        // or just not seeing any obvious bugs yet because the base classes are so minimal.
        rb_cCharacteristic = define_class_under<Characteristic>(rb_mSimpleRbBLE, "Characteristic");
        using CapabilityType = Characteristic::Capability;
        rb_cCharacteristicCapabilityType = define_enum<CapabilityType>("CapabilityType", rb_cCharacteristic)
                .define_value("READ", CapabilityType::READ)
                .define_value("WRITE_REQUEST", CapabilityType::WRITE_REQUEST)
                .define_value("WRITE_COMMAND", CapabilityType::WRITE_COMMAND)
                .define_value("NOTIFY", CapabilityType::NOTIFY)
                .define_value("INDICATE", CapabilityType::INDICATE);
        using CababilityFlags = Characteristic::CababilityFlags;
        //        auto rb_cCharacteristicCapabilityFlags = define_class_under<CababilityFlags>(rb_cCharacteristic, "CapabilityFlags")
//                .define_method("flag_names", &CababilityFlags::flag_names)
//                ;

        rb_cCharacteristic
                .define_method("uuid", &Characteristic::uuid) // returns BluetoothUUID
                .define_method("descriptors", &Characteristic::descriptors)
                .define_method("capabilities", &Characteristic::capability_names)
                .define_method("can_read?", &Characteristic::can_read)
                .define_method("can_write_request?", &Characteristic::can_write_request)
                .define_method("can_write_command?", &Characteristic::can_write_command)
                .define_method("can_notify?", &Characteristic::can_notify)
                .define_method("can_indicate?", &Characteristic::can_indicate)
                .define_method<Characteristic::ExposedReadFn>("read", &Characteristic::read)
                .define_method("write", &Characteristic::write)
                .define_method("write_request", &Characteristic::write_request)
                .define_method("write_command", &Characteristic::write_command)
                .define_method("set_on_notify", &Characteristic::set_on_notify)
                .define_method("set_on_indicate", &Characteristic::set_on_indicate)
                .define_method("unsubscribe", &Characteristic::unsubscribe)
                .define_method("inspect", &Characteristic::to_s);
    }
}

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::Characteristic>(SimpleRbBLE::Characteristic *characteristic) {
        rb_gc_mark(characteristic->self());
        if (characteristic->_on_indicate) Rice::ruby_mark(characteristic->_on_indicate.get());
        if (characteristic->_on_notify) Rice::ruby_mark(characteristic->_on_notify.get());
        if (characteristic->_descriptors) {
            for (const auto &[uuid, descriptor]: *characteristic->_descriptors) Rice::ruby_mark(descriptor.get());
        }
    }
}

std::size_t std::hash<SimpleBLE::Characteristic>::operator()(const SimpleBLE::Characteristic &p) const noexcept {
    auto uuid = const_cast<SimpleBLE::Characteristic &>(p).uuid();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<decltype(uuid)>{};
    return h1 ^ (hasher(uuid) << 1);
}

std::size_t std::hash<SimpleRbBLE::Characteristic>::operator()(const SimpleRbBLE::Characteristic &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Characteristic>{};
    return h1 ^ (hasher(p.get()) << 1);
}
