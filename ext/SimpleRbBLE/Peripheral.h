#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include "common.h"
#include "Callback.h"
#include "NamedBitSet.h"

namespace SimpleRbBLE {
    enum class PeripheralStatus : std::size_t {
        INITIALIZED = 0, CONNECTED, CONNECTABLE, PAIRED
    };
    class Peripheral {
    public:
        static constexpr std::string_view StatusFlagNames[] = {
                "initialized", "connected", "connectable", "paired"
        };
        using StatusFlagSet = SimpleRbBLE::NamedBitSet<StatusFlagNames>;
        using Status = SimpleRbBLE::PeripheralStatus;

        using DataObject = Data_Object<Peripheral>;
        using Owner = Adapter;
        using ServiceMap = std::map<BluetoothUUID, std::shared_ptr<Service>>;
        static bool unpair_all_on_exit;
    protected:
        Owner *_owner;
        std::shared_ptr<SimpleBLE::Peripheral> _peripheral;
        SimpleBLE::BluetoothAddress _addr;
        std::shared_ptr<Callback> _on_connected;
        std::shared_ptr<Callback> _on_disconnected;
        std::shared_ptr<ServiceRegistry> _service_registry;
        StatusFlagSet _service_cache_tag;
        VALUE _self = Qnil;

    public:
//        static std::shared_ptr<Peripheral> create(const SimpleBLE::Peripheral &peripheral);

        Object self() const;
        Peripheral() = delete;
        Peripheral(const SimpleBLE::Peripheral&, Owner*); // NOLINT(*-explicit-constructor)
        ~Peripheral() = default;

//    Peripheral(const Peripheral&) = delete;
//    Peripheral(Peripheral&&) = delete;
        bool initialized() const;

        const Owner *owner() const;
        Owner *owner();

        SimpleBLE::Peripheral &get();
        const SimpleBLE::Peripheral &get() const;

        std::string identifier() const;
        BluetoothAddress address() const;
        BluetoothAddressType address_type() const;
        std::map<uint16_t, ConvertableByteArray> manufacturer_data() const;
        int16_t rssi() const;
        int16_t tx_power() const;
        uint16_t mtu() const;
        bool is_connected() const;
        bool is_connectable() const;
        bool is_paired() const;
        StatusFlagSet status_flags() const;
        std::vector<std::string_view> statuses() const;
        void connect();
        void disconnect();
        void unpair();

        const std::map<BluetoothUUID, std::shared_ptr<Service>> &services() const;
        std::shared_ptr<Service> operator[](const BluetoothUUID &svcUuid) const;

        ConvertableByteArray read(BluetoothUUID const &service, BluetoothUUID const &characteristic);
        ConvertableByteArray read(BluetoothUUID const &service, BluetoothUUID const &characteristic,
             BluetoothUUID const &descriptor);
        void write(BluetoothUUID const &service, BluetoothUUID const &characteristic,
                   BluetoothUUID const &descriptor, ConvertableByteArray data);
        void write_request(BluetoothUUID const &service, BluetoothUUID const &characteristic,
                           ConvertableByteArray data);
        void write_command(BluetoothUUID const &service, BluetoothUUID const &characteristic,
                           ConvertableByteArray data);
        void notify(BluetoothUUID const &service, BluetoothUUID const &characteristic,
                    std::function<void(ConvertableByteArray payload)> callback);
        void indicate(BluetoothUUID const &service, BluetoothUUID const &characteristic,
                      std::function<void(ConvertableByteArray payload)> callback);
        void unsubscribe(BluetoothUUID const &service, BluetoothUUID const &characteristic);

        std::string to_s() const;
        // bool operator==(const Peripheral &ap) const;
        // bool operator==(const Peripheral &ap) const;

        void on_connect(Object on_connect);
        void on_disconnect(Object on_disconnect);

        template<typename, class, class> friend class Registry;
        template<typename T> friend void Rice::ruby_mark(T*);
        friend void Init_Registries();
    };

    static Peripheral_DT rb_cPeripheral;
}



#pragma clang diagnostic pop