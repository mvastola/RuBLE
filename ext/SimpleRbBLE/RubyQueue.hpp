#pragma once

#include "Callback.hpp"
#include <deque>
#include <mutex>
#include <ruby/thread.h>
#include <future>
#include <semaphore>
#include <chrono>
#include <shared_mutex>
#include "utils.hpp"

namespace chrono = std::chrono;

namespace SimpleRbBLE {

    const constexpr static auto semaphore_max = std::numeric_limits<uint16_t>::max();
    // All ruby calls must take place in thread known to ruby
    // Since SimpleBLE creates its own thread from which callbacks are invoked,
    // we have that (non-ruby) thread add a function to RubyQueue,
    // which is monitored from a ruby thread and invoked immediately invoked with a FIFO ordering
    class RubyQueue {
    public:
        using FnType = std::function<void(void)>;
        using QueueItemType = FnType;
        using QueueType = std::deque<QueueItemType>;

        using RubyThreadId = VALUE;
        using CppThreadId = std::thread::id;
        using Semaphore = std::counting_semaphore<semaphore_max>;
    private:
        static std::shared_future<std::shared_ptr<RubyQueue>> _instance;
        std::shared_ptr<QueueType> _q = std::shared_ptr<QueueType>(new QueueType()); // NOLINT(*-make-shared);

        std::mutex _mtx;
        Semaphore _sem { 0 };
        std::binary_semaphore _startup_latch { 0 };

        mutable std::shared_mutex _run_mtx;
        mutable std::atomic_flag _running;

        std::atomic<CppThreadId> _thread_id;
        std::atomic<RubyThreadId> _rb_thread;
        std::atomic_flag _starting, _stopping;

        RubyQueue();


        VALUE run();
        void loop();
    public:
        ~RubyQueue() = default;
        RubyQueue(const RubyQueue&) = delete;
        RubyQueue(RubyQueue&&) = delete;
        RubyQueue &operator=(RubyQueue&&) = delete;
        RubyQueue &operator=(const RubyQueue&) = delete;
        // TODO: add a function that waits for queue to be settled

        static std::shared_ptr<RubyQueue> instance();
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

    // TODO: is doing it this way helping at all?
    inline std::shared_future<std::shared_ptr<RubyQueue>> RubyQueue::_instance = std::async(std::launch::deferred, []() {
        return std::shared_ptr<RubyQueue>(new RubyQueue());
    }).share();


//    using RubyQueue_DT = Data_Type<RubyQueue>;
//    using RubyQueue_DO = Data_Object<RubyQueue>;
//    static inline RubyQueue_DT rb_cRubyQueue;
}
