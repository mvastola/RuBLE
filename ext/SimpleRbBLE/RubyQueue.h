#pragma once

#include "common.h"
#include "Callback.h"
#include <semaphore>
#include <deque>
#include <ruby/thread.h>


namespace SimpleRbBLE {
    // All ruby calls must take place in thread known to ruby
    // Since SimpleBLE creates its own thread from which callbacks are invoked,
    // we have that (non-ruby) thread add a function to RubyQueue,
    // which is monitored from a ruby thread and invoked immediately invoked with a FIFO ordering
    class RubyQueue {
    public:
        using FnType = std::function<void(void)>;
        using QueueItemType = FnType;
        using OptionalQueueItemType = std::optional<FnType>;
        using QueueType = std::deque<QueueItemType>;
    private:
        static inline std::mutex _q_mutex;
        static inline std::mutex _run_mutex;
        static inline std::atomic_flag _stop = false;
        static inline std::atomic_flag _running = false;
        static inline std::optional<VALUE> _thread {};
        static inline std::counting_semaphore<semaphore_max> _sem {0};
        static std::shared_ptr<RubyQueue> _instance;
        std::shared_ptr<QueueType> _q { new QueueType() };

        RubyQueue();
        std::optional<RubyQueue::QueueItemType> pop();
        VALUE _run();

    public:
        ~RubyQueue();
        static std::shared_ptr<RubyQueue> instance();
        void push(QueueItemType fn);

        // Argument is unused, but required, because rb_set_end_proc
        // requires the method to accept this argument
        static void stop(VALUE = Qnil);
    };

    using RubyQueue_DT = Data_Type<RubyQueue>;
    using RubyQueue_DO = Data_Object<RubyQueue>;
    static inline RubyQueue_DT rb_cRubyQueue;
}