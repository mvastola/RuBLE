#pragma once

#include <functional>
#include <ruby/ruby.h>
#include <thread>
#include "types/helpers.hpp"
#include "types/stl.hpp"

namespace Rubble::Utils {
    namespace Async {
        using namespace std::chrono_literals;
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


//        template <typename From>
//        using WrapReferences = std::conditional_t<std::is_reference_v<From>, std::reference_wrapper<std::remove_reference_t<From>>, From>;
//
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
    }
    using namespace Async;
}
