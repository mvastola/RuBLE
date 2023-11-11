#pragma once

#include "types/declarations.hpp"
#include "types/ruby.hpp"
#include "utils/hash.hpp"

namespace RubBLE {
    class Peripheral;
//    template<Identifiers::OwnerTypedResource T>
//    class ResourceUniqueIdentifier;

    class Service {
    public:
        using Owner = Peripheral;
        using DataObject = Data_Object<Service>;
    protected:
        Owner *_owner;
        std::shared_ptr<SimpleBLE::Service> _service;
        std::shared_ptr<CharacteristicRegistry> _characteristic_registry;
        BluetoothUUID _uuid;
        VALUE _self = Qnil;

    public:
        [[nodiscard]] Object self() const;
        Service() = delete;
        Service(const SimpleBLE::Service&, Owner*);
        SimpleBLE::Service &get();
        [[nodiscard]] const SimpleBLE::Service &get() const;
        [[nodiscard]] constexpr const Owner *owner() const { return _owner; }
        constexpr Owner *owner() { return _owner; }
        [[nodiscard]] constexpr const Peripheral *peripheral() const { return owner(); }
        constexpr Peripheral *peripheral() { return owner(); }

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const { return _uuid; }
//        [[nodiscard]] constexpr ResourceUniqueIdentifier<Service> full_resource_identifier() const;
//        [[nodiscard]] constexpr BluetoothUUID this_resource_identifier() const { return uuid(); }


        [[nodiscard]] ByteArray data() const;
        [[nodiscard]] const std::map<BluetoothUUID, std::shared_ptr<Characteristic>> &characteristics() const;
        [[nodiscard]] std::shared_ptr<Characteristic> operator[](const BluetoothUUID &charUuid) const;

        [[nodiscard]] ByteArray read(const BluetoothUUID &characteristic);
        [[nodiscard]] ByteArray read(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor);
        void write(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor, const ByteArray &data);
        void write_request(const BluetoothUUID &characteristic, const ByteArray &data);
        void write_command(const BluetoothUUID &characteristic, const ByteArray &data);
        void notify(const BluetoothUUID &characteristic, std::function<void(ByteArray payload)> callback);
        void indicate(const BluetoothUUID &characteristic, std::function<void(ByteArray payload)> callback);
        void unsubscribe(const BluetoothUUID &characteristic);
        void ruby_mark() const;

        [[nodiscard]] std::string to_s() const;
        // bool operator==(const Service &ap) const;
        // bool operator==(const Service &ap) const;

        friend void Init_Service();
        friend void Init_Registries();
        template<typename, class, class> friend class Registry;
    };
}
