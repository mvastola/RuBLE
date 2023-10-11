#include "Service.h"

#include <utility>
#include "RubyQueue.h"
#include "Callback.h"
#include "Characteristic.h"
#include "Registry.h"
#include "RegistryFactory.h"
#include "Peripheral.h"

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

    constexpr const Service::Owner *Service::owner() const { return _owner; }
    constexpr Service::Owner *Service::owner() { return _owner; }
    constexpr const Peripheral *Service::peripheral() const { return owner(); }
    constexpr Peripheral *Service::peripheral() { return owner(); }


    ConvertableByteArray Service::data() const { return _service->data(); }

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


    ConvertableByteArray Service::read(const BluetoothUUID &characteristic) {
        return peripheral()->read(uuid(), characteristic);
    }

    ConvertableByteArray
    Service::read(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor) {
        return peripheral()->read(uuid(), characteristic, descriptor);
    }

    void
    Service::write(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor,
                   ConvertableByteArray data) {
        peripheral()->write(uuid(), characteristic, descriptor, std::move(data));
    }

    void
    Service::write_request(const BluetoothUUID &characteristic, ConvertableByteArray data) {
        peripheral()->write_request(uuid(), characteristic, std::move(data));
    }

    void
    Service::write_command(const BluetoothUUID &characteristic, ConvertableByteArray data) {
        peripheral()->write_command(uuid(), characteristic, std::move(data));
    }

    void Service::notify(const BluetoothUUID &characteristic,
                         std::function<void(ConvertableByteArray)> callback) {
        peripheral()->notify(uuid(), characteristic, std::move(callback));
    }

    void Service::indicate(const BluetoothUUID &characteristic,
                           std::function<void(ConvertableByteArray)> callback) {
        peripheral()->indicate(uuid(), characteristic, std::move(callback));
    }

    void Service::unsubscribe(const BluetoothUUID &characteristic) {
        peripheral()->unsubscribe(uuid(), characteristic);
    }

    std::string Service::to_s() const {
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
        oss << "@uuid=\"" << uuid() << "\" ";
        oss << "@data=\"" << data() << "\" ";
//        oss << "#characteristics=" << characteristics().size();
        oss << ">";
        return oss.str();
    }

    Service_DT rb_cService;

    void Init_Service() {
        rb_cService = define_class_under<Service>(rb_mSimpleRbBLE, "Service")
                .define_method("uuid", &Service::uuid) // returns BluetoothUUID
                .define_method("data", &Service::data) // returns ByteArray
                .define_method("characteristics", &Service::characteristics) // returns std::vector<Characteristic>
                .define_method("inspect", &Service::to_s);
    }
}

namespace Rice {
    template<>
    void ruby_mark<SimpleRbBLE::Service>(SimpleRbBLE::Service *service) {
        rb_gc_mark(service->self());
        for (const auto &[uuid, characteristic]: service->characteristics())
            Rice::ruby_mark(characteristic.get());
    }
}

std::size_t std::hash<SimpleBLE::Service>::operator()(const SimpleBLE::Service &p) const noexcept {
    auto uuid = const_cast<SimpleBLE::Service &>(p).uuid();
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<decltype(uuid)>{};
    return h1 ^ (hasher(uuid) << 1);
}

std::size_t std::hash<SimpleRbBLE::Service>::operator()(const SimpleRbBLE::Service &p) const noexcept {
    std::size_t h1 = typeid(p).hash_code();
    static auto hasher = std::hash<SimpleBLE::Service>{};
    return h1 ^ (hasher(p.get()) << 1);
}
