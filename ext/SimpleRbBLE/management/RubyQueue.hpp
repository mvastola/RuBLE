#pragma once

#include "containers/Callback.hpp"
#include "types/stl.hpp"
#include <deque>
#include <mutex>
#include <chrono>
#include <thread>
#include <shared_mutex>


namespace SimpleRbBLE {
    // All ruby calls must take place in thread known to ruby
    // Since SimpleBLE creates its own thread from which callbacks are invoked,
    // we have that (non-ruby) thread add a function to RubyQueue,
    // which is monitored from a ruby thread and invoked immediately invoked with a FIFO ordering
    class RubyQueue {
    public:
        using FnType = std::function<void(void)>;
        using QueueItemType = FnType;
        using QueueType [[maybe_unused]] = std::deque<QueueItemType>;

    private:
        static std::shared_ptr<RubyQueue> _instance;
        std::shared_ptr<QueueType> _q = std::shared_ptr<QueueType>(new QueueType());

        std::mutex _mtx;
        Semaphore _sem { 0 };
        std::binary_semaphore _startup_latch { 0 };

        mutable std::shared_mutex _run_mtx;
        mutable std::atomic_flag _running;

        std::atomic<CppThreadId> _thread_id;
        std::atomic<RubyThreadId> _rb_thread;
        std::atomic_flag _starting, _stopping;

        constexpr RubyQueue() = default;
        VALUE run();
        void loop();
    public:
        ~RubyQueue() = default;
        RubyQueue(const RubyQueue&) = delete;
        RubyQueue(RubyQueue&&) = delete;
        RubyQueue &operator=(RubyQueue&&) = delete;
        RubyQueue &operator=(const RubyQueue&) = delete;
        // TODO: add a function that waits for queue to be settled

        static const std::shared_ptr<RubyQueue> &instance();
        [[nodiscard]] Object rb_thread() const;

        void push(QueueItemType fn);

        void start();
        void ensure_started();
        [[nodiscard]] bool running() const;

        void stop();
        [[nodiscard]] bool stopped() const;
        static void stop_on_exit(VALUE); // Argument is unused, but rb_set_end_proc requires it
        void kill();
    };



//    using RubyQueue_DT = Data_Type<RubyQueue>;
//    using RubyQueue_DO = Data_Object<RubyQueue>;
//    static inline RubyQueue_DT rb_cRubyQueue;
}
