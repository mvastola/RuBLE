#pragma once

#include "types/ruby.hpp"
#include <ruby/thread.h>
#include <memory>
#include <functional>

namespace SimpleRbBLE {
    namespace Utils::Ruby {
        class InRuby;
        class InRubyGuard {
            InRuby *_inRuby = nullptr;
            constexpr InRubyGuard(InRuby &inRuby);
        public:
            constexpr InRubyGuard() = default;
            constexpr InRubyGuard(const InRubyGuard&) = delete;
            constexpr InRubyGuard &operator=(const InRubyGuard&) = delete;
            constexpr InRubyGuard(InRubyGuard&&);
            constexpr InRubyGuard &operator=(InRubyGuard&&);
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
                return InRubyGuard(*this);
            }
            constexpr const auto &use_count() const { return _use_count; }
            constexpr bool state() const { return use_count() > 0; }
            constexpr operator bool() const { return state(); }
            friend class InRubyGuard;
        };

        constexpr InRubyGuard::InRubyGuard(InRuby &inRuby) : _inRuby(&inRuby) {
            if (_inRuby != nullptr) ++_inRuby->_use_count;
        }
        constexpr InRubyGuard::~InRubyGuard() {
            if (_inRuby != nullptr) --_inRuby->_use_count;
        }

        constexpr InRubyGuard::InRubyGuard(InRubyGuard &&other) {
            *this = std::move(other);
        }

        constexpr InRubyGuard &InRubyGuard::operator=(InRubyGuard &&other) {
            InRuby *old = std::exchange(_inRuby, other._inRuby);
            if (_inRuby != nullptr) ++_inRuby->_use_count;
            if (old != nullptr) --old->_use_count;
            other._inRuby = nullptr;
            return *this;
        }
    }

    using Utils::Ruby::InRuby;
    extern thread_local InRuby in_ruby;

    namespace Utils::Ruby {
        template <typename T, typename Fn = std::function<T()>>
        T ensure_ruby(const Fn &fn) {
            if (in_ruby) return fn();
            void *(*invoke)(void*) = [](void *fnPtr) -> void * {
                volatile auto in_ruby_guard = in_ruby.assert_in_ruby_guard();
                auto &fnRef = *reinterpret_cast<Fn *>(fnPtr);
                return new T(std::move(fnRef()));
            };
            void *resultPtrVoid = rb_thread_call_with_gvl(invoke, const_cast<std::remove_const_t<Fn>*>(&fn));
            T *resultPtr = reinterpret_cast<T*>(resultPtrVoid);
            T &&result = std::move(*resultPtr);
            delete resultPtr;
            return std::move(result);
        }
        template <typename T, typename... Args>
        [[maybe_unused]] T ensuring_ruby(std::function<T(Args...)> &&fn, Args &&...args) {
            auto wrapper = [&fn, &args...]() -> T { return std::invoke(fn, args...); };
            return ensure_ruby<T>(wrapper);
        }

        constexpr VALUE to_ruby_value(const VALUE &val) { return val; }
        constexpr VALUE to_ruby_value(const RiceObject auto &obj) { return obj.value(); }
        constexpr VALUE to_ruby_value(const HasRubyObject auto &obj) { return obj.self(); }
        constexpr VALUE to_ruby_value(const HasRubyObject auto *obj) { return to_ruby_value(*obj); }

        template <class T> requires HasRubyObject<typename T::element_type> &&
                std::same_as<std::shared_ptr<typename T::element_type>, T>
        constexpr VALUE to_ruby_value(const T &obj) { return to_ruby_value(*obj); }

        template <RiceObject T = Rice::Object>
        constexpr T to_ruby_object(const auto &val) { return T { to_ruby_value(val) }; }

//        TODO: See if we can figure out how to get this to work (see https://stackoverflow.com/q/28746744)
//        template <typename T = void>
//        VALUE ruby_thread_create(RbThreadCreateFn<T> fn, T *arg = nullptr) {
//            constexpr RbThreadCreateFn<void> wrapper = [&fn](void *ptr) -> VALUE  {
//                return fn(reinterpret_cast<T*>(ptr));
//            };
//            return rb_thread_create(wrapper, arg);
//        }
//
//
//        template <typename T = void, typename Ret = void, typename UbfArgT = void>
//        Ret *ruby_thread_call_without_gvl(RbThreadCallFn<Ret, T> fn,
//                                          T *arg = nullptr,
//                                          RbUbfFn<UbfArgT> &ubf = nullptr,
//                                          UbfArgT *ubfArg = nullptr) {
//            RbThreadCreateFn<void> wrapper = [&fn](void *ptr) -> void * {
//                return fn(reinterpret_cast<T*>(ptr));
//            };
//            RbUbfFn<void> ubfWrapper = [&ubf](void *ptr) -> void {
//                return ubf(reinterpret_cast<T*>(ptr));
//            };
//            void *ret = rb_thread_call_without_gvl(wrapper, arg, ubf, ubfWrapper);
//            return reinterpret_cast<Ret*>(ret);
//        }

    }
    namespace Ruby = Utils::Ruby;
    using Utils::Ruby::ensure_ruby;
}
