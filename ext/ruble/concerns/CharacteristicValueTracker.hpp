#pragma once
#include "types/declarations.hpp"
#include <optional>

namespace RuBLE {
    // just felt cleaner to abstract out this logic. open to being persuaded otherwise
    class CharacteristicValueTracker {
    protected:
        std::optional<bool> _track_values;
        mutable std::shared_ptr<ByteArray> _value;

        constexpr CharacteristicValueTracker(std::optional<bool> enabled = std::nullopt) :
            _track_values(enabled) {}

        // returns true if value changed
        bool record_new_value(ByteArray new_data) const;

    public:
        virtual bool can_notify() const = 0;
        constexpr void set_value_tracking(bool new_value) {
            if (new_value && !can_notify()) {
                throw std::runtime_error("Characteristic does not have 'notify' capability.");
            } else if (!new_value) {
                _value = nullptr;
            }
            _track_values = new_value;
        }

        constexpr bool value_tracking() const {
            return _track_values.value_or(can_notify());
        }

        [[nodiscard]] constexpr const std::shared_ptr<ByteArray> &last_value() const {
            return _value;
        }

        virtual void ruby_mark() const;
    };
} // RuBLE

