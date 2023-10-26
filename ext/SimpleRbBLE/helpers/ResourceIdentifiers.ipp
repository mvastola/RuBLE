#include "common.hpp"
#include "Adapter.hpp"

namespace SimpleRbBLE {

    constexpr BluetoothAddress Adapter::this_resource_identifier() const { return _addr; }

    constexpr ResourceUniqueIdentifier<Adapter> Adapter::full_resource_identifier() const { return this; }
    constexpr ResourceUniqueIdentifier<Service> Service::full_resource_identifier() const { return this; }

    constexpr ResourceUniqueIdentifier<Peripheral> Peripheral::full_resource_identifier() const {
        return ResourceUniqueIdentifier<Peripheral>(this);  // NOLINT(*-return-braced-init-list)
    }
    constexpr ResourceUniqueIdentifier<Descriptor> Descriptor::full_resource_identifier() const { return this; }
    constexpr ResourceUniqueIdentifier<Characteristic> Characteristic::full_resource_identifier() const { return this; }

    constexpr BluetoothUUID Descriptor::this_resource_identifier() const { return _uuid; }

    constexpr std::string Peripheral::this_resource_identifier() const {
        //todo: use the string version of the type instead
        //        BluetoothAddressType_DT::from_enum(address_type(), rb_cBluetoothAddressType);
        std::string result("[");
        result.append(STRINGIFICATOR(address_type()));
        result.append("]");
        result.append(address());
        return result;
    }

#ifdef SIMPLERBBLE_DEBUG
    static_assert(SimpleRbBLE::Identifiers::OwnedResource<Descriptor>);
    static_assert(SimpleRbBLE::Identifiers::OwnerTypedResource<Peripheral>);
    static_assert(SimpleRbBLE::Identifiers::PartiallyIdentifiableResource<Peripheral>);
    static_assert(SimpleRbBLE::Identifiers::OwnedResource<Peripheral>);
    static_assert(!SimpleRbBLE::Identifiers::IDTypes::is_toplevel<Peripheral>::value);
    static_assert(ResourceUniqueIdentifier<Adapter>::N == 1);
    static_assert(ResourceUniqueIdentifier<Peripheral>::N == 2);
    static_assert(ResourceUniqueIdentifier<Service>::N == 3);
    static_assert(SimpleRbBLE::Identifiers::OwnerTypedResource<SimpleRbBLE::Characteristic>);
    static_assert(ResourceUniqueIdentifier<SimpleRbBLE::Characteristic>::N == 4);
#endif
};

//#include "ResourceUniqueIdentifier.hpp"
//#include "ResourceUniqueIdentifier.ipp"
