#include "types/declarations.hpp"
#include "bindings/Peripheral.hpp"
#include "bindings/Service.hpp"
#include "bindings/Characteristic.hpp"
#include "bindings/Descriptor.hpp"

#include <utility>
#include "management/RubyQueue.hpp"
#include "containers/Callback.hpp"

namespace RubBLE {
    Characteristic::Characteristic(const SimpleBLE::Characteristic &characteristic,
                                   Owner *owner) :
            _owner(owner),
            _characteristic(std::make_shared<SimpleBLE::Characteristic>(characteristic)),
            _uuid(_characteristic->uuid()),
            _self(DataObject(*this)) {

        if (capabilities()["notify"]) {
            service()->notify(uuid(), [this](const ByteArray& data) {
                RubyQueue::FnType fn = std::bind(&Characteristic::fire_on_notify, std::cref(*this), data);  // NOLINT(*-avoid-bind)
                rubyQueue->push(fn);
            });
        }

        if (capabilities()["indicate"]) {
            service()->indicate(uuid(), [this](const ByteArray &data) {
                RubyQueue::FnType fn = std::bind(&Characteristic::fire_on_indicate, std::cref(*this), data);  // NOLINT(*-avoid-bind)
                rubyQueue->push(fn);
            });
        }
    }

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

    ByteArray Characteristic::read(bool force) {
        // if tracking is enabled, assume we'll get the value via callback
        if (!force && value_tracking() && last_value()) return *last_value();

        ByteArray value = service()->read(uuid());
        record_new_value(value);
        return std::move(value);
    }

    ByteArray Characteristic::read(const BluetoothUUID &descriptor) {
        return service()->read(uuid(), descriptor);
    }
    void Characteristic::write(const BluetoothUUID &descriptor, const ByteArray &data) {
        service()->write(uuid(), descriptor, data);
    }
    void Characteristic::write_request(const ByteArray &data) {
        service()->write_request(uuid(), data);
    }
    void Characteristic::write_command(const ByteArray &data) {
        service()->write_command(uuid(), data);
    }

    void Characteristic::set_on_notify(Object cb) {
        if (!_on_notify) _on_notify = std::make_shared<Callback>(2);
        _on_notify->set(std::move(cb));
    }

    void Characteristic::set_on_indicate(Object cb) {
        if (!_on_indicate) _on_indicate = std::make_shared<Callback>(2);
        _on_indicate->set(std::move(cb));
    }

    void Characteristic::fire_on_notify(const ByteArray &data) const {
        if (DEBUG) std::cout << "Characteristic::fire_on_notify called with " << data.inspect() << std::endl;
        if (!record_new_value(data)) return; // if value tracking is enabled, skip callback unless value changed
        if (_on_notify) _on_notify->fire(Rice::detail::To_Ruby<SimpleBLE::ByteArray>().convert(data), self());
    }

    void Characteristic::fire_on_indicate(const ByteArray &data) const {
        if (DEBUG) std::cout << "Characteristic::fire_on_indicate called with " << data.inspect() << std::endl;
        if (_on_indicate) _on_indicate->fire(Rice::detail::To_Ruby<SimpleBLE::ByteArray>().convert(data), self());
    }

    void Characteristic::unsubscribe() {
        service()->unsubscribe(uuid());
    }

    std::string Characteristic::to_s() const {
        std::ostringstream oss;
        oss << Utils::basic_object_inspect_start(*this) << " ";
        oss << "@uuid=" << uuid() << " ";
        oss << "@capabilities=" << capabilities();
//        oss << "#characteristics=" << characteristics().size();
        oss << ">";
        return oss.str();
    }

    Rice::String Characteristic::inspect() const {
        return to_s();
    }


    void Characteristic::ruby_mark() const {
        rb_gc_mark(self());
        CharacteristicValueTracker::ruby_mark();
        if (_on_indicate) Rice::ruby_mark(_on_indicate.get());
        if (_on_notify) Rice::ruby_mark(_on_notify.get());
        if (_descriptors) {
            for (const auto &[uuid, descriptor]: *_descriptors) Rice::ruby_mark(descriptor.get());
        }
    }

    void Init_Characteristic() {
        // FIXME: A bunch of these classes inherit from abstract base classes (e.g. BaseAdapter),
        // and are also subclassed in different implementations (I think).
        // Do we need the constructor and methods to return/receive pointers to work well
        // (so the right virtual functions are called)? Not sure if rice is handling that all,
        // or just not seeing any obvious bugs yet because the base classes are so minimal.
        rb_cCharacteristic = define_class_under<Characteristic>(rb_mRubBLE, "Characteristic");
        using CapabilityType = Characteristic::Capability;
        rb_cCharacteristicCapabilityType = define_enum<CapabilityType>("CapabilityType", rb_cCharacteristic)
                .define_value("READ", CapabilityType::READ)
                .define_value("WRITE_REQUEST", CapabilityType::WRITE_REQUEST)
                .define_value("WRITE_COMMAND", CapabilityType::WRITE_COMMAND)
                .define_value("NOTIFY", CapabilityType::NOTIFY)
                .define_value("INDICATE", CapabilityType::INDICATE);
        using CababilityFlags [[maybe_unused]] = Characteristic::CapabilityFlags;
        //        auto rb_cCharacteristicCapabilityFlags = define_class_under<CapabilityFlags>(rb_cCharacteristic, "CapabilityFlags")
//                .define_method("flag_names", &CapabilityFlags::flag_names)
//                ;

        rb_cCharacteristic
                .define_method("uuid", &Characteristic::uuid) // returns BluetoothUUID
                .define_method("capabilities", &Characteristic::capability_names)
                .define_method("can_read?", &Characteristic::can_read)
                .define_method("can_write_request?", &Characteristic::can_write_request)
                .define_method("can_write_command?", &Characteristic::can_write_command)
                .define_method("can_notify?", &Characteristic::can_notify)
                .define_method("can_indicate?", &Characteristic::can_indicate)
                .define_method<Characteristic::ExposedReadFn>("read", &Characteristic::read, Arg("force") = false)
                .define_method("write", &Characteristic::write)
                .define_method("write_request", &Characteristic::write_request)
                .define_method("write_command", &Characteristic::write_command)
                .define_method("set_on_notify", &Characteristic::set_on_notify)
                .define_method("set_on_indicate", &Characteristic::set_on_indicate)
//                .define_method("unsubscribe", &Characteristic::unsubscribe)
                .define_method("to_s", &Characteristic::to_s)
                .define_method("inspect", &Characteristic::inspect)
                .define_method("value_tracking", &Characteristic::value_tracking)
                .define_method("value_tracking?", &Characteristic::value_tracking)
                .define_method("value_tracking=", &Characteristic::set_value_tracking)
                .define_method("last_value", &Characteristic::last_value)
                ;
        define_class_under<std::shared_ptr<RubBLE::Characteristic>>(rb_mRubBLE, "CharacteristicPtr");

    }
}
