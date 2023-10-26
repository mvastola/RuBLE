#pragma once

#include "common.hpp"
#include <utility>
#include <compare>


namespace SimpleRbBLE {
    class Characteristic;
    class Descriptor {
    public:
        using DataObject = Descriptor_DO;
        using Owner = Characteristic;
    protected:
        BluetoothUUID _uuid;
        Owner *_owner = nullptr;
        VALUE _self = Qnil;
    public:
        static std::map<BluetoothUUID, std::shared_ptr<Descriptor>> make_map(const auto &range, Descriptor::Owner *owner) {
            std::map<BluetoothUUID, std::shared_ptr<Descriptor>> result;
            for (const auto &descriptor : range) {
                auto descPtr = std::make_shared<Descriptor>(descriptor, owner);
                result.emplace(descPtr->uuid(), std::move(descPtr));
            }
            return result;
        }

        Descriptor() = delete;
        Descriptor(BluetoothUUID uuid, Owner *owner);
        Descriptor(const SimpleBLE::Descriptor &descriptor, Owner *owner);

        [[nodiscard]] Object self() const;

        [[nodiscard]] const Owner *owner() const { return _owner; }
        constexpr Owner *owner() { return _owner; }

        [[nodiscard]] const Characteristic *characteristic() const { return _owner; }

        constexpr Characteristic *characteristic() { return _owner; }

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const { return _uuid; }

        [[nodiscard]] constexpr ResourceUniqueIdentifier<Descriptor> full_resource_identifier() const;
        [[nodiscard]] constexpr BluetoothUUID this_resource_identifier() const;

        ByteArray read();
        void write(ByteArray data);

        [[nodiscard]] constexpr std::string to_s() const {
            return basic_object_inspect_start(*this) + " " + uuid() + ">";
        }
        [[nodiscard]] Rice::String inspect() const;

        [[nodiscard]] constexpr auto operator<=>(const Descriptor &other) const;

    };


    constexpr auto Descriptor::operator<=>(const Descriptor &other) const {
        return uuid() <=> other.uuid();
    }

} // SimpleRbBLE


std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::Descriptor &d);



