#include "bindings/Descriptor.hpp"
#include "bindings/Characteristic.hpp"

namespace RuBLE {
    Descriptor::Descriptor(BluetoothUUID uuid, Descriptor::Owner *owner) :
        _owner(owner), _uuid(std::move(uuid)),  _self(DataObject(*this))  {}

    Descriptor::Descriptor(const SimpleBLE::Descriptor &descriptor, Descriptor::Owner *owner) :
            Descriptor(const_cast<SimpleBLE::Descriptor&>(descriptor).uuid(), owner) {}

    Object Descriptor::self() const { return _self; }

    [[nodiscard]] ByteArray Descriptor::read() { return characteristic()->read(uuid()); }

    void Descriptor::write(const ByteArray &data) { characteristic()->write(uuid(), data); }


    Rice::String Descriptor::inspect() const {
        return to_s();
    }



    void Init_Descriptor() {
        rb_cDescriptor = define_class_under<Descriptor>(rb_mRuBLE, "Descriptor")
                .define_method("uuid", &Descriptor::uuid) // returns BluetoothUUID
                .define_method("inspect", &Descriptor::inspect)
                .define_method("read", &Descriptor::read)
                .define_method("write", &Descriptor::write)
                ;
    }

}

