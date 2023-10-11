#include "Descriptor.h"
#include "Characteristic.h"

namespace SimpleRbBLE {
    Descriptor_DT rb_cDescriptor;

    Descriptor::Descriptor(BluetoothUUID uuid, Descriptor::Owner *owner) :
        _owner(owner), _uuid(std::move(uuid)),  _self(DataObject(*this))  {}

    Descriptor::Descriptor(const SimpleBLE::Descriptor &descriptor, Descriptor::Owner *owner) :
            Descriptor(const_cast<SimpleBLE::Descriptor&>(descriptor).uuid(), owner) {}

    Object Descriptor::self() const { return _self; }

    constexpr const Descriptor::Owner *Descriptor::owner() const { return _owner; }

    constexpr Descriptor::Owner *Descriptor::owner() { return _owner; }

    constexpr const Characteristic *Descriptor::characteristic() const { return _owner; }

    constexpr Characteristic *Descriptor::characteristic() { return _owner; }

    ConvertableByteArray Descriptor::read() { return characteristic()->read(uuid()); }

    void Descriptor::write(ConvertableByteArray data) { characteristic()->write(uuid(), std::move(data)); }


    constexpr std::string Descriptor::to_s() const {
        return "#<SimpleRbBLE::Descriptor @uuid="s + uuid() + ">";
    }

    Rice::String Descriptor::inspect() const {
        return to_s();
    }

    constexpr auto Descriptor::operator<=>(const Descriptor &other) const {
        return uuid() <=> other.uuid();
    }


    void Init_Descriptor() {
        rb_cDescriptor = define_class_under<Descriptor>(rb_mSimpleRbBLE, "Descriptor")
                .define_method("uuid", &Descriptor::uuid) // returns BluetoothUUID
                .define_method("inspect", &Descriptor::inspect)
                .define_method("read", &Descriptor::read)
                .define_method("write", &Descriptor::write)

                ;
    }

}

std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::Descriptor &d) { return os << d.to_s(); }

std::size_t std::hash<SimpleRbBLE::Descriptor>::operator()(const SimpleRbBLE::Descriptor& d) const noexcept {
    std::size_t h1 = typeid(d).hash_code();
    static auto hasher = std::hash<std::remove_cvref_t<decltype(d.uuid())>>{};
    return h1 ^ (hasher(d.uuid()) << 1);
}