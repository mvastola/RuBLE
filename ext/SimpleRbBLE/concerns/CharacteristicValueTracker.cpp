#include "CharacteristicValueTracker.hpp"

namespace SimpleRbBLE {
    bool CharacteristicValueTracker::record_new_value(ByteArray new_data) const {
        if (!value_tracking()) return true; // always assume value changed if no tracking
        std::optional<ByteArray> old_value = _value;
        _value = std::move(new_data);
        // if no previous value, returns whether or not new value is empty
        if (!old_value.has_value()) return _value.value().length() > 0;
        // otherwise, return true if old and new values differ
        return old_value.value() != _value.value();
    }

    void CharacteristicValueTracker::ruby_mark() const {
        if (_value.has_value()) _value.value().ruby_mark();
    }
}