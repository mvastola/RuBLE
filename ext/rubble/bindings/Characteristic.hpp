#pragma once

#include "management/RubyQueue.hpp"
#include "containers/NamedBitSet.hpp"
#include "bindings/Descriptor.hpp"
#include "bindings/Service.hpp"
#include "containers/ByteArray.hpp"
#include "utils/hash.hpp"
#include "concerns/CharacteristicValueTracker.hpp"

namespace Rubble {
    enum class CharacteristicCapabilityType : std::size_t {
        READ = 0,
        WRITE_REQUEST,
        WRITE_COMMAND,
        NOTIFY,
        INDICATE
    };
    class Service;
    class Characteristic : public CharacteristicValueTracker {
    public:
        static constexpr std::string_view CAPABILITY_NAMES[] = {
                "read",
                "write_request",
                "write_command",
                "notify",
                "indicate"
        };
        using Capability = CharacteristicCapabilityType;
        using CapabilityFlags = Rubble::NamedBitSet<CAPABILITY_NAMES>;
        using DataObject = Data_Object<Characteristic>;
        using Owner = Service;
        using CallbackFnType [[maybe_unused]] = std::function<void(ByteArray payload)>;
        using DescriptorMap = std::map<BluetoothUUID, std::shared_ptr<Descriptor>>;
    protected:
        Owner *_owner;
        std::shared_ptr<SimpleBLE::Characteristic> _characteristic;
        BluetoothUUID _uuid;
        std::shared_ptr<Callback> _on_notify;
        std::shared_ptr<Callback> _on_indicate;
        mutable std::optional<CapabilityFlags> _capabilities;
        // NB: this is the only class where we're not using a Registry<>
        // This can be a case study in if we can remove a ton of the registry stuff
        // now that I figured out garbage collection
        mutable std::optional<DescriptorMap> _descriptors;

        VALUE _self = Qnil;

        void fire_on_notify(const ByteArray &data) const;
        void fire_on_indicate(const ByteArray &data) const;
    public:
        [[nodiscard]] Object self() const;
        Characteristic() = delete;
        Characteristic(const SimpleBLE::Characteristic&, Owner*);
        SimpleBLE::Characteristic &get();
        [[nodiscard]] const SimpleBLE::Characteristic &get() const;
        [[nodiscard]] constexpr const Owner *owner() const;
        constexpr Owner *owner();
        [[nodiscard]] constexpr const Service *service() const;
        constexpr Service *service();

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const;
//        constexpr ResourceUniqueIdentifier<Characteristic> full_resource_identifier() const;
//        constexpr BluetoothUUID this_resource_identifier() const;

        [[nodiscard]] const std::map<BluetoothUUID, std::shared_ptr<Descriptor>> &descriptors() const;
        [[nodiscard]] std::shared_ptr<Descriptor> operator[](const BluetoothUUID &descUuid) const;
        [[nodiscard]] constexpr const CapabilityFlags &capabilities() const;

        [[nodiscard]] std::vector<std::string> capability_names() const;
        [[nodiscard]] constexpr bool can_read() const;
        [[nodiscard]] constexpr bool can_write_request() const;
        [[nodiscard]] constexpr bool can_write_command() const;
        [[nodiscard]] constexpr bool can_notify() const override;
        [[nodiscard]] constexpr bool can_indicate() const;

        [[nodiscard]] ByteArray read(bool force = false);
        [[nodiscard]] ByteArray read(const BluetoothUUID &descriptor);
        void write(const BluetoothUUID &descriptor, const ByteArray &data);
        void write_request(const ByteArray &data);
        void write_command(const ByteArray &data);

        void set_on_notify(Object cb);
        void set_on_indicate(Object cb);
        void unsubscribe();

        [[nodiscard]] std::string to_s() const;
        [[nodiscard]] Rice::String inspect() const;
        // bool operator==(const Characteristic &ap) const;
        // bool operator==(const Characteristic &ap) const;
        void ruby_mark() const override;

        friend void Init_Characteristic();
        friend void Init_Registries();
        template<typename, class, class> friend class Registry;
        using ExposedReadFn = ByteArray(Characteristic::*)(bool);
    };

    constexpr const Characteristic::Owner *Characteristic::owner() const { return _owner; }

    constexpr Characteristic::Owner *Characteristic::owner() { return _owner; }

    constexpr const Service *Characteristic::service() const { return owner(); }

    constexpr Service *Characteristic::service() { return owner(); }

    constexpr const BluetoothUUID &Characteristic::uuid() const { return _uuid; }


//    constexpr BluetoothUUID Characteristic::this_resource_identifier() const { return _uuid; }

    constexpr const Characteristic::CapabilityFlags &Characteristic::capabilities() const {
        if (_capabilities) return *_capabilities;
        _capabilities = std::make_optional<CapabilityFlags>(_characteristic->capabilities());
        return *_capabilities;
    }

    constexpr bool Characteristic::can_read() const { return capabilities()["read"]; }

    constexpr bool Characteristic::can_write_request() const { return capabilities()["write_request"]; }

    constexpr bool Characteristic::can_write_command() const { return capabilities()["write_command"]; }

    constexpr bool Characteristic::can_notify() const { return capabilities()["notify"]; }

    constexpr bool Characteristic::can_indicate() const { return capabilities()["indicate"]; }

    using CharacteristicCapabilityType_DT = Data_Type<Characteristic::Capability>;
    using CharacteristicCapabilityType_DO [[maybe_unused]] = Data_Object<Characteristic::Capability>;

}

