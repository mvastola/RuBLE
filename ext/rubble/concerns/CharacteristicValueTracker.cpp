#include "CharacteristicValueTracker.hpp"

namespace RubBLE {
    bool CharacteristicValueTracker::record_new_value(ByteArray new_data) const {
        if (!value_tracking()) return true; // always assume value changed if no tracking
        std::shared_ptr<ByteArray> old_value =
                std::exchange(_value, std::make_shared<ByteArray>(std::move(new_data)));

        // if no previous value, returns whether or not new value is empty
        if (!old_value) return _value->length() > 0;
        // otherwise, return true if old and new values differ
        return *old_value != *_value;
    }

    void CharacteristicValueTracker::ruby_mark() const {
        if (_value) _value->ruby_mark();
    }
}