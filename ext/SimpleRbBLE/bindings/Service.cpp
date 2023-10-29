#include "bindings/Service.hpp"

#include <utility>
#include "bindings/Characteristic.hpp"
#include "management/Registry.hpp"
#include "management/RegistryFactory.hpp"
#include "bindings/Peripheral.hpp"
#include "types/declarations.hpp"

namespace SimpleRbBLE {
    Service::Service(const SimpleBLE::Service &service, Owner *owner) :
            _owner(owner),
            _service(std::make_shared<SimpleBLE::Service>(service)),
            _uuid(_service->uuid()),
            _characteristic_registry(characteristicRegistryFactory->registry(this)),
            _self(DataObject(*this)) {}

    Object Service::self() const {
        return _self;
    }

    SimpleBLE::Service &Service::get() { return *_service; }

    const SimpleBLE::Service &Service::get() const { return *_service; }

    ByteArray Service::data() const { return _service->data(); }

    const std::map<BluetoothUUID, std::shared_ptr<Characteristic>> &Service::characteristics() const {
        if (!rb_during_gc()) {
            // we can't create new characteristics during GC
            for (const auto &c : _service->characteristics()) _characteristic_registry->fetch(c);
        }
        return _characteristic_registry->data();
//        std::vector<SimpleBLE::Characteristic> unwrappedCharacteristics = _service->characteristics();
//        auto objects = _characteristic_registry->map_to_objects(unwrappedCharacteristics);
//
//        std::function<BluetoothUUID(const Characteristic&)> mapFn = &Characteristic::uuid;
//        auto result = map_by<BluetoothUUID, std::shared_ptr<Characteristic>>(objects, mapFn);
//        return std::move(result);
    }

    std::shared_ptr<Characteristic> Service::operator[](const BluetoothUUID &charUuid) const {
        // only check SimpleBLE backend for characteristic if not already instantiated
        if (!_characteristic_registry->contains(charUuid)) const auto _result = characteristics();
        return _characteristic_registry->at(charUuid);
    }


    ByteArray Service::read(const BluetoothUUID &characteristic) {
        return peripheral()->read(uuid(), characteristic);
    }

    ByteArray
    Service::read(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor) {
        return peripheral()->read(uuid(), characteristic, descriptor);
    }

    void
    Service::write(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor,
                   const ByteArray &data) {
        peripheral()->write(uuid(), characteristic, descriptor, data);
    }

    void
    Service::write_request(const BluetoothUUID &characteristic, const ByteArray &data) {
        peripheral()->write_request(uuid(), characteristic, data);
    }

    void
    Service::write_command(const BluetoothUUID &characteristic, const ByteArray &data) {
        peripheral()->write_command(uuid(), characteristic, data);
    }

    void Service::notify(const BluetoothUUID &characteristic,
                         std::function<void(ByteArray)> callback) {
        peripheral()->notify(uuid(), characteristic, std::move(callback));
    }

    void Service::indicate(const BluetoothUUID &characteristic,
                           std::function<void(ByteArray)> callback) {
        peripheral()->indicate(uuid(), characteristic, std::move(callback));
    }

    void Service::unsubscribe(const BluetoothUUID &characteristic) {
        peripheral()->unsubscribe(uuid(), characteristic);
    }

    std::string Service::to_s() const {
        std::ostringstream oss;
        oss << Utils::basic_object_inspect_start(*this) << " ";
        oss << "@uuid=\"" << uuid() << "\" ";
        oss << "@data=\"" << data() << "\" ";
//        oss << "#characteristics=" << characteristics().size();
        oss << ">";
        return oss.str();
    }

    void Service::ruby_mark() const {
        rb_gc_mark(self());
        for (const auto &[uuid, characteristic]: characteristics())
            Rice::ruby_mark(characteristic.get());
    }

    void Init_Service() {
        rb_cService = define_class_under<Service>(rb_mSimpleRbBLE, "Service")
                .define_method("uuid", &Service::uuid) // returns BluetoothUUID
                .define_method("data", &Service::data) // returns SimpleBLE::ByteArray
                .define_method("characteristics", &Service::characteristics) // returns std::vector<Characteristic>
                .define_method("inspect", &Service::to_s);
        define_class_under<std::shared_ptr<SimpleRbBLE::Service>>(rb_mSimpleRbBLE, "ServicePtr");
    }
}
