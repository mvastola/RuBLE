#pragma once

#include "types.hpp"

#include "NamedBitSet.hpp"
#include "ConvertableByteArray.hpp"
#include "metaprogramming.hpp"
#include "utils.hpp"

namespace SimpleRbBLE {
    template<Identifiers::OwnerTypedResource T>
    class ResourceUniqueIdentifier;
}

#include "Callback.hpp"
#include "RubyQueue.hpp"

#include "Adapter.hpp"
#include "NamedBitSet.hpp"
#include "NamedBitSet.ipp"

#include "Peripheral.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"
#include "Descriptor.hpp"

#include "RegistryFactory.hpp"
#include "Registry.hpp"

#include "ResourceUniqueIdentifier.hpp"

namespace SimpleRbBLE {

    class RubyQueue;

    extern std::shared_ptr<RubyQueue> rubyQueue;

    extern Module rb_mSimpleRbBLE;
    extern Module rb_mSimpleRbBLEUnderlying;
    extern Module rb_mUtils;
    extern BluetoothAddressType_DT rb_cBluetoothAddressType;
    extern Characteristic_DT rb_cCharacteristic;
    extern CharacteristicCapabilityType_DT rb_cCharacteristicCapabilityType;
    extern Descriptor_DT rb_cDescriptor;
    extern Service_DT rb_cService;
    extern ServiceRegistry_DT rb_cServiceRegistry;
    extern AdapterRegistry_DT rb_cAdapterRegistry;
    extern PeripheralRegistry_DT rb_cPeripheralRegistry;
    extern CharacteristicRegistry_DT rb_cCharacteristicRegistry;
    extern ConvertableByteArray_DT rb_cConvertableByteArray;

    extern void Init_Adapter();
    extern void Init_BluetoothAddressType();
    extern void Init_Characteristic();
    extern void Init_ConvertableByteArray();
    extern void Init_Descriptor();
    extern void Init_Peripheral();
    extern void Init_Registries();
    extern void Init_Service();
    extern void Init_Utils();
    extern void Init_DeInit();
    extern void DeInit(VALUE);

    extern void c_debug();

}

template<> struct std::hash<SimpleBLE::Adapter> {
    std::size_t operator()(const SimpleBLE::Adapter& a) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Adapter> {
    std::size_t operator()(const SimpleRbBLE::Adapter& a) const noexcept;
};

template<> struct std::hash<SimpleBLE::Peripheral> {
    std::size_t operator()(const SimpleBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Peripheral> {
    std::size_t operator()(const SimpleRbBLE::Peripheral& p) const noexcept;
};

template<> struct std::hash<SimpleBLE::Service> {
    std::size_t operator()(const SimpleBLE::Service& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Service> {
    std::size_t operator()(const SimpleRbBLE::Service& s) const noexcept;
};

template<> struct std::hash<SimpleBLE::Characteristic> {
    std::size_t operator()(const SimpleBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Characteristic> {
    std::size_t operator()(const SimpleRbBLE::Characteristic& s) const noexcept;
};

template<> struct std::hash<SimpleRbBLE::Descriptor> {
    std::size_t operator()(const SimpleRbBLE::Descriptor& s) const noexcept;
};

namespace Rice {
    template<> void ruby_mark<SimpleRbBLE::Adapter>(SimpleRbBLE::Adapter *);
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistry>(SimpleRbBLE::AdapterRegistry*);
    template<> void ruby_mark<SimpleRbBLE::AdapterRegistryFactory>(SimpleRbBLE::AdapterRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Callback>(SimpleRbBLE::Callback*);
    template<> void ruby_mark<SimpleRbBLE::Peripheral>(SimpleRbBLE::Peripheral*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistry>(SimpleRbBLE::PeripheralRegistry*);
    template<> void ruby_mark<SimpleRbBLE::PeripheralRegistryFactory>(SimpleRbBLE::PeripheralRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Service>(SimpleRbBLE::Service*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistry>(SimpleRbBLE::ServiceRegistry*);
    template<> void ruby_mark<SimpleRbBLE::ServiceRegistryFactory>(SimpleRbBLE::ServiceRegistryFactory*);
    template<> void ruby_mark<SimpleRbBLE::Characteristic>(SimpleRbBLE::Characteristic*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistry>(SimpleRbBLE::CharacteristicRegistry*);
    template<> void ruby_mark<SimpleRbBLE::CharacteristicRegistryFactory>(SimpleRbBLE::CharacteristicRegistryFactory*);

}

#include "NamedBitSet.ipp"
#include "ResourceUniqueIdentifier.hpp"
