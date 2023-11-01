#pragma once

#include "types/ruby.hpp"
#include "utils/ruby_context.hpp"
//#include "types/stl.hpp"
#include <ruby/thread.h>
#include <memory>
#include <functional>

namespace SimpleRbBLE {
    namespace Utils::Ruby {
        template <typename T, typename Fn = std::function<T()>>
        T ensure_ruby(const Fn &fn) {
            if (in_ruby) return fn();
            RbThreadCallFn<> invoke = [](void *fnPtr) -> void * {
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

        [[maybe_unused]] constexpr VALUE to_ruby_value(const VALUE &val) { return val; }

        [[maybe_unused]] constexpr VALUE to_ruby_value(const RiceObject auto &obj) { return obj.value(); }

        [[maybe_unused]] constexpr VALUE to_ruby_value(const HasRubyObject auto &obj) { return obj.self(); }
        constexpr VALUE to_ruby_value(const HasRubyObject auto *obj) { return to_ruby_value(*obj); }

        template <class T> requires HasRubyObject<typename T::element_type> &&
                                    std::same_as<std::shared_ptr<typename T::element_type>, T>
        constexpr VALUE to_ruby_value(const T &obj) { return to_ruby_value(*obj); }

        template <RiceObject T = Rice::Object>
        [[maybe_unused]] constexpr T to_ruby_object(const auto &val) { return T { to_ruby_value(val) }; }

#if false
        // TODO: test me!
        template <typename T, typename... Args>
        RubyThreadId ruby_thread_create(std::function<T(Args...)> &&fn, Args &&...args) {
            using FnT = std::function<void()>;
            FnT invoker = [&fn, &args...]() { std::invoke(fn, args...); };
            constexpr RbThreadCreateFn<void> wrapper = [](void *ptr) -> VALUE  {
                FnT invoker = *reinterpret_cast<FnT*>(ptr);
                invoker();
                return Qnil;
            };
            RubyThreadId tid = rb_thread_create(wrapper, &invoker);
            return tid;
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
#endif
    }
    namespace Ruby = Utils::Ruby;
    using Utils::Ruby::ensure_ruby;
}
