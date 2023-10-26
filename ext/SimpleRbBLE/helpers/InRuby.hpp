#pragma once

#include <memory>

namespace SimpleRbBLE {
    class InRuby {
    public:
        using Guard = std::shared_ptr<bool>;
        using WeakGuard = Guard::weak_type;
    private:
        WeakGuard _state;
    public:
        InRuby() = default;
        InRuby(const InRuby&) = delete;
        InRuby(InRuby&&) = delete;
        InRuby &operator=(InRuby&&) = delete;
        InRuby &operator=(const InRuby&) = delete;

        // similar to lock_guard: this->state() will return true
        // as long as at least one result of the assert_guard fn is maintained
        inline Guard assert_guard() {
            Guard flag = _state.lock();
            if (!flag) _state = flag = std::make_shared<bool>(true);
            return flag;
        }

        [[nodiscard]] inline bool state() const { return !_state.expired(); }
        inline operator bool() const { return state(); } // NOLINT(*-explicit-constructor)
    };
    // we shouldn't have to worry about atomicity here since things are thread_local
    inline thread_local InRuby in_ruby;
}
