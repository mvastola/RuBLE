#pragma once

#include <cstdint>
#include <utility>

namespace RubBLE::Utils::Ruby::Context {
    class InRuby;
    class InRubyGuard {
        InRuby *_inRuby = nullptr;

        [[maybe_unused]] constexpr InRubyGuard(InRuby &inRuby);
    public:
        constexpr InRubyGuard() = default;
        constexpr InRubyGuard(const InRubyGuard&) = delete;
        constexpr InRubyGuard &operator=(const InRubyGuard&) = delete;
        constexpr InRubyGuard(InRubyGuard&&) noexcept ;
        constexpr InRubyGuard &operator=(InRubyGuard&&) noexcept ;
        constexpr ~InRubyGuard();
        constexpr operator bool() const { return _inRuby != nullptr; }

        friend class InRuby;
    };

    class InRuby {
    private:
        uint64_t _use_count = 0;
    public:
        using Guard = InRubyGuard;
        constexpr InRuby() = default;
        InRuby(const InRuby &) = delete;
        InRuby(InRuby &&) = delete;
        InRuby &operator=(InRuby &&) = delete;
        InRuby &operator=(const InRuby &) = delete;

        // similar to lock_guard: this->state() will return true
        // as long as at least one result of the assert_in_ruby_guard fn is maintained
        constexpr Guard assert_in_ruby_guard() {
            return { *this };
        }
        [[nodiscard]] constexpr const auto &use_count() const { return _use_count; }
        [[nodiscard]] constexpr bool state() const { return use_count() > 0; }
        constexpr operator bool() const { return state(); }
        friend class InRubyGuard;
    };

    [[maybe_unused]] constexpr InRubyGuard::InRubyGuard(InRuby &inRuby) : _inRuby(&inRuby) {
        if (_inRuby != nullptr) ++_inRuby->_use_count;
    }
    constexpr InRubyGuard::~InRubyGuard() {
        if (_inRuby != nullptr) --_inRuby->_use_count;
    }

    constexpr InRubyGuard::InRubyGuard(InRubyGuard &&other) noexcept {
        *this = std::move(other);
    }

    constexpr InRubyGuard &InRubyGuard::operator=(InRubyGuard &&other) noexcept {
        InRuby *old = std::exchange(_inRuby, other._inRuby);
        if (_inRuby != nullptr) ++_inRuby->_use_count;
        if (old != nullptr) --old->_use_count;
        other._inRuby = nullptr;
        return *this;
    }
}

namespace RubBLE {
    namespace Utils {
        namespace RubyContext = Ruby::Context;
    }
    namespace RubyContext = Utils::RubyContext;
    using RubyContext::InRuby;
    extern thread_local InRuby in_ruby;
}
