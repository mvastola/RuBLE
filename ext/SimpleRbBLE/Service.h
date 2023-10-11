#pragma once

#include "common.h"

namespace SimpleRbBLE {
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
        Object self() const;
        Service() = delete;
        Service(const SimpleBLE::Service&, Owner*); // NOLINT(*-explicit-constructor)

        SimpleBLE::Service &get();
        [[nodiscard]] const SimpleBLE::Service &get() const;
        [[nodiscard]] constexpr const Owner *owner() const;
        constexpr Owner *owner();
        [[nodiscard]] constexpr const Peripheral *peripheral() const;
        constexpr Peripheral *peripheral();

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const { return _uuid; }
        [[nodiscard]] ConvertableByteArray data() const;
        [[nodiscard]] const std::map<BluetoothUUID, std::shared_ptr<Characteristic>> &characteristics() const;
        std::shared_ptr<Characteristic> operator[](const BluetoothUUID &charUuid) const;

        ConvertableByteArray read(const BluetoothUUID &characteristic);
        ConvertableByteArray read(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor);
        void write(const BluetoothUUID &characteristic, const BluetoothUUID &descriptor, ConvertableByteArray data);
        void write_request(const BluetoothUUID &characteristic, ConvertableByteArray data);
        void write_command(const BluetoothUUID &characteristic, ConvertableByteArray data);
        void notify(const BluetoothUUID &characteristic, std::function<void(ConvertableByteArray payload)> callback);
        void indicate(const BluetoothUUID &characteristic, std::function<void(ConvertableByteArray payload)> callback);
        void unsubscribe(const BluetoothUUID &characteristic);


        [[nodiscard]] std::string to_s() const;
        // bool operator==(const Service &ap) const;
        // bool operator==(const Service &ap) const;

        friend void Init_Service();
        friend void Init_Registries();
        template<typename, class, class> friend class Registry;
        template<typename T> friend void Rice::ruby_mark(T*);
    };

    void Init_Service();
}
