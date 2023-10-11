#pragma once
#include <utility>
#include <compare>

#include "common.h"

namespace SimpleRbBLE {
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
        [[nodiscard]] constexpr const Owner *owner() const;
        constexpr Owner *owner();
        [[nodiscard]] constexpr const Characteristic *characteristic() const;
        constexpr Characteristic *characteristic();

        [[nodiscard]] constexpr const BluetoothUUID &uuid() const { return _uuid; }
        ConvertableByteArray read();
        void write(ConvertableByteArray data);

        [[nodiscard]] constexpr std::string to_s() const;
        [[nodiscard]] Rice::String inspect() const;

        constexpr auto operator<=>(const Descriptor &other) const;
    };
} // SimpleRbBLE


std::ostream &operator<<(std::ostream &os, const SimpleRbBLE::Descriptor &d);

