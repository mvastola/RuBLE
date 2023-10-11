#pragma once

#include "common.h"
#include "RubyQueue.h"
#include "NamedBitSet.h"
#include "Descriptor.h"
#include "Service.h"
#include "ConvertableByteArray.h"

namespace SimpleRbBLE {
    enum class CharacteristicCapabilityType : std::size_t {
        READ = 0,
        WRITE_REQUEST,
        WRITE_COMMAND,
        NOTIFY,
        INDICATE
    };
    class Characteristic {
    public:
        static constexpr std::string_view CAPABILITY_NAMES[] = {
                "read",
                "write_request",
                "write_command",
                "notify",
                "indicate"
        };
        using Capability = CharacteristicCapabilityType;
        using CababilityFlags = SimpleRbBLE::NamedBitSet<CAPABILITY_NAMES>;
        using DataObject = Data_Object<Characteristic>;
        using Owner = Service;
        using CallbackFnType = std::function<void(ConvertableByteArray payload)>;
        using DescriptorMap = std::map<BluetoothUUID, std::shared_ptr<Descriptor>>;
    protected:
        Owner *_owner;
        std::shared_ptr<SimpleBLE::Characteristic> _characteristic;
        BluetoothUUID _uuid;
        std::shared_ptr<Callback> _on_notify;
        std::shared_ptr<Callback> _on_indicate;
        mutable std::optional<CababilityFlags> _capabilities;
        // NB: this is the only class where we're not using a Registry<>
        // This can be a case study in if we can remove a ton of the registry stuff
        // now that I figured out garbage collection
        mutable std::optional<DescriptorMap> _descriptors;

        VALUE _self = Qnil;
    public:
        [[nodiscard]] Object self() const;
        Characteristic() = delete;
        Characteristic(const SimpleBLE::Characteristic&, Owner*); // NOLINT(*-explicit-constructor)

        SimpleBLE::Characteristic &get();
        [[nodiscard]] const SimpleBLE::Characteristic &get() const;
        [[nodiscard]] constexpr const Owner *owner() const;
        constexpr Owner *owner();
        [[nodiscard]] constexpr const Service *service() const;
        constexpr Service *service();

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const { return _uuid; }
        [[nodiscard]] const std::map<BluetoothUUID, std::shared_ptr<Descriptor>> &descriptors() const;
        [[nodiscard]] std::shared_ptr<Descriptor> operator[](const BluetoothUUID &descUuid) const;
        [[nodiscard]] constexpr const CababilityFlags &capabilities() const;
        [[nodiscard]] std::vector<std::string> capability_names() const;
        [[nodiscard]] bool can_read() const;
        [[nodiscard]] bool can_write_request() const;
        [[nodiscard]] bool can_write_command() const;
        [[nodiscard]] bool can_notify() const;
        [[nodiscard]] bool can_indicate() const;


        ConvertableByteArray read();
        ConvertableByteArray read(const BluetoothUUID &descriptor);
        void write(const BluetoothUUID &descriptor, ConvertableByteArray data);
        void write_request(ConvertableByteArray data);
        void write_command(ConvertableByteArray data);

        void set_on_notify(Object cb);
        void set_on_indicate(Object cb);
        void fire_on_notify(const ConvertableByteArray &data) const;
        void fire_on_indicate(const ConvertableByteArray &data) const;
        void unsubscribe();

        [[nodiscard]] std::string to_s() const;
        [[nodiscard]] Rice::String inspect() const;
        // bool operator==(const Characteristic &ap) const;
        // bool operator==(const Characteristic &ap) const;

        friend void Init_Characteristic();
        friend void Init_Registries();
        template<typename, class, class> friend class Registry;
        template<typename T> friend void Rice::ruby_mark(T*);

        using ExposedReadFn = ConvertableByteArray(Characteristic::*)();
    };

    using CharacteristicCapabilityType_DT = Data_Type<Characteristic::Capability>;
    using CharacteristicCapabilityType_DO = Data_Object<Characteristic::Capability>;

    void Init_Characteristic();
}
