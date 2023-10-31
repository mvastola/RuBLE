#pragma once

#include <memory>
#include <functional>

namespace SimpleRbBLE {
    namespace Utils::Ruby {
        class InRuby {
        public:
            using Guard = std::shared_ptr<bool>;
            using WeakGuard = Guard::weak_type;
        private:
            WeakGuard _state;
        public:
            InRuby() = default;
            InRuby(const InRuby &) = delete;
            InRuby(InRuby &&) = delete;
            InRuby &operator=(InRuby &&) = delete;
            InRuby &operator=(const InRuby &) = delete;

            // similar to lock_guard: this->state() will return true
            // as long as at least one result of the assert_guard fn is maintained
            Guard assert_guard();

            [[nodiscard]] bool state() const;

            operator bool() const;
        };
    }

    using Utils::Ruby::InRuby;
    extern thread_local InRuby in_ruby;

    namespace Utils::Ruby {
        template <typename T, typename Fn = std::function<T()>>
        T ensure_ruby(const Fn &fn) {
            if (in_ruby) return fn();
            void *(*invoke)(void*) = [](void *fnPtr) -> void * {
                auto in_ruby_guard = in_ruby.assert_guard();
                auto &fnRef = *reinterpret_cast<Fn *>(fnPtr);
                return new T(std::move(fnRef()));
            };
            void *resultPtrVoid = rb_thread_call_with_gvl(invoke, const_cast<std::remove_const_t<Fn>*>(&fn));
            T *resultPtr = reinterpret_cast<T*>(resultPtrVoid);
            T &&result = std::move(*resultPtr);
            delete resultPtr;
            return std::move(result);
        }


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
    // we shouldn't have to worry about atomicity here since things are thread_local
    using Utils::Ruby::ensure_ruby;
}
