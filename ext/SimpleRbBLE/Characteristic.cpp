#include "common.hpp"
#include "Peripheral.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"
#include "Descriptor.hpp"

#include <utility>
#include "RubyQueue.hpp"
#include "Callback.hpp"

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


    std::vector<std::string> Characteristic::capability_names() const { return capabilities().flag_name_strs(); }

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
                if (DEBUG) std::cout << "Characteristic::fire_on_notify called with " << data.inspect() << std::endl;
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
                if (DEBUG) std::cout << "Characteristic::fire_on_indicate called with " << data.inspect() << std::endl;
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
        oss << basic_object_inspect_start(*this) << " ";
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
                .define_method("to_s", &Characteristic::to_s)
                .define_method("inspect", &Characteristic::inspect);
        define_class_under<std::shared_ptr<SimpleRbBLE::Characteristic>>(rb_mSimpleRbBLE, "CharacteristicPtr");

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
