#pragma once

#include "types.hpp"
#include "helpers/metaprogramming.hpp"
#include "helpers/InRuby.hpp"
#include <ruby/ruby.h>
#include <ruby/thread.h>
//#include <ruby/thread_native.h>
//#include <any>
//#include <version>
#include <ranges>
//#include <endian.h>
//#include <chrono>
//#include <variant>
//#include <future>
//#include <charconv>
#include <ranges>
//#include <format>

namespace ranges = std::ranges;
namespace views = std::views;
namespace chrono = std::chrono;
using namespace std::chrono_literals;
using namespace std::placeholders;

namespace SimpleRbBLE {
    namespace Rice = ::Rice;
    namespace Utils {
        template <typename T, typename Fn>
        T ensure_ruby(const Fn &fn);


        template<class KeyT, class ValueT,
                class FnT = std::function<std::remove_reference_t<KeyT>(const std::remove_cvref_t<ValueT> &)>,
                class ResultT = std::map<std::remove_reference_t<KeyT>, std::remove_reference_t<ValueT>>,
                ranges::viewable_range RangeT>
        requires requires(FnT &&fnT, RangeT &&rangeT) {
            { fnT(**ranges::begin(rangeT)) } -> std::convertible_to<std::remove_reference_t<KeyT>>;
        }

        ResultT map_by(RangeT &&r, FnT &&mapFn) {
            auto makePair = [&mapFn](ValueT &val) -> auto {
                KeyT key = mapFn(*val);
                return std::make_pair(std::move(key), val);
            };
            auto pairView = views::transform(r, makePair);
            return {pairView.begin(), pairView.end()};
        }

        // Not sure exactly which of these is doing the trick, but calling this when waiting
        // on an async operation tends to get ruby jump to its other threads
        // oddly, none of these commands, by themselves, were working for me
        void thread_sleep();

        template <OptionalDuration DurationT = std::nullopt_t>
        bool wait_until(const std::function<bool(void)> &fn, const DurationT &duration = std::nullopt) {
            using PredicateFn = std::function<bool()>;
            constexpr const bool has_timeout = !Nullopt<DurationT>;

            bool fn_result = false, timed_out = false;
            PredicateFn is_timed_out;

            if constexpr (has_timeout) {
                using clock = chrono::high_resolution_clock;
                using time_point = clock::time_point;
                time_point start = clock::now();
                time_point until = start + chrono::duration_cast<clock::duration>(duration);

                is_timed_out = [&timed_out, until] -> bool {
                    return until < clock::now();
                };
            } else {
                is_timed_out = [] { return false; };
            }

            auto check_if_done = [&is_timed_out, &timed_out, &fn, &fn_result] () -> bool {
                fn_result = fn();
                timed_out = is_timed_out();
                return fn_result || timed_out;
            };

            while(!check_if_done()) thread_sleep();
            return fn_result;
        }

//        template <Future FutureT, typename ...Args> // T should be std::shared_future<T> or std::future<T>
//        bool wait_for_future(FutureT future, const Args&... args) {
//            using PredicateFn = std::function<bool()>;
//            PredicateFn is_ready = [&future] { return future.wait_for(0s) == std::future_status::ready; };
//            return wait_until(is_ready, args...);
//        }
//
//        template <Future FutureT>
//        using await_future_result = typename future_traits<FutureT>::value_type;
//
//        template <Future FutureT, typename ...Args> requires (sizeof...(Args) == 1)
//        std::optional<WrapReferences<await_future_result<FutureT>>> await_future(FutureT future, const Args& ...args) {
//            using ResultType = WrapReferences<await_future_result<FutureT>>;
//            bool done = wait_for_future(future, args...);
//            return done ? std::ref(future.get()) : std::optional<ResultType>();
//        }
//
//        template <Future FutureT>
//        await_future_result<FutureT> await_future(FutureT future) {
//            await_future(future, std::nullopt);
//            return future.get();
//        }

//        TODO: See if we can figure out how to get this to work (see https://stackoverflow.com/q/28746744)
//        template <typename T = void>
//        VALUE ruby_thread_create(RbThreadCreateFn<T> fn, T *arg = nullptr) {
//            constexpr RbThreadCreateFn<void> wrapper = [&fn](void *ptr) -> VALUE  {
//                return fn(reinterpret_cast<T*>(ptr));
//            };
//            return rb_thread_create(wrapper, arg);
//        }
//
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
        constexpr const bool endianness_byteswap_needed = std::endian::native == std::endian::big;

        template<typename T>
        static constexpr T maybe_swap_endianness(const T &t) {
        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "Simplify"
            // TODO: there has to be a better way to silence CLion's
            //  "This will always evaluate to false" warning
            if constexpr (!endianness_byteswap_needed) return t;
        #pragma clang diagnostic pop
            // From https://codereview.stackexchange.com/a/237565
            auto in = std::as_bytes(std::span(&t, 1));
            T result;
            auto out = std::as_writable_bytes(std::span(&result, 1));
            std::copy(in.rbegin(), in.rend(), out.begin());
            return result;
        }

        template<typename T>
        static constexpr void maybe_swap_endianness_inplace(T &t) {
        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "Simplify"
            // TODO: there has to be a better way to silence CLion's
            //  "This will always evaluate to false" warning
            if constexpr (!endianness_byteswap_needed) return t;
        #pragma clang diagnostic pop
            // Based on https://codereview.stackexchange.com/a/237565
            auto span = std::as_writable_bytes(std::span(&t, 1));
            std::reverse(span.begin(), span.end());
        }
    }

    using namespace Utils;


    void Init_Utils();
}

