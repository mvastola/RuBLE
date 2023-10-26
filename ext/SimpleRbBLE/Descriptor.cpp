#include "Descriptor.hpp"
#include "Characteristic.hpp"

namespace SimpleRbBLE {
    [[maybe_unused]] Descriptor_DT rb_cDescriptor;

    Descriptor::Descriptor(BluetoothUUID uuid, Descriptor::Owner *owner) :
        _owner(owner), _uuid(std::move(uuid)),  _self(DataObject(*this))  {}

    Descriptor::Descriptor(const SimpleBLE::Descriptor &descriptor, Descriptor::Owner *owner) :
            Descriptor(const_cast<SimpleBLE::Descriptor&>(descriptor).uuid(), owner) {}

    Object Descriptor::self() const { return _self; }

    [[nodiscard]] ByteArray Descriptor::read() { return characteristic()->read(uuid()); }

    void Descriptor::write(ByteArray data) { characteristic()->write(uuid(), std::move(data)); }


    Rice::String Descriptor::inspect() const {
        return to_s();
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