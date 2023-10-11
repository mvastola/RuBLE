#include "RubyQueue.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"

namespace SimpleRbBLE {
    std::shared_ptr<RubyQueue> RubyQueue::_instance { nullptr };

    RubyQueue::RubyQueue() {
        _thread = rb_thread_create([](void *) -> VALUE { return instance()->_run(); }, nullptr);
    }

    RubyQueue::~RubyQueue() {
        _running.clear();
        _running.notify_all();
    }

    std::shared_ptr<RubyQueue> RubyQueue::instance() {
        if (!_instance) {
            std::lock_guard<std::mutex> lock(_run_mutex);
            if (_instance) return _instance;
            _instance = std::shared_ptr<RubyQueue>(new RubyQueue);
        }
        return _instance;
    }

    void RubyQueue::stop(VALUE) {
        _stop.test_and_set();
        _sem.release(); // so RubyQueue::pop stops waiting for semaphore
    }


    void RubyQueue::push(RubyQueue::QueueItemType fn) {
        if (_stop.test()) {
#ifdef DEBUG
            std::cerr << "Warning: attempted to push to SimpleRbBLE callback queue, ";
            std::cerr << "but queue has been stopped." << std::endl;
#endif
            return;
        }
        std::lock_guard<std::mutex> lock(_q_mutex);
        _q->emplace_back(std::move(fn));
        _sem.release();
    }

    std::optional<RubyQueue::QueueItemType> RubyQueue::pop() {
        while (!_sem.try_acquire_for(150ms)) {
            if (!_stop.test()) return {};
//            rb_thread_schedule();
        }

        QueueItemType val;
        {
            std::lock_guard<std::mutex> lock(_q_mutex);
            val = std::move(_q->back());
            _q->pop_back();
        }
        return val;
    }

    VALUE RubyQueue::_run() {
        if (_running.test_and_set()) { // _running flag was already true
            throw std::runtime_error("Ruby Queue is already running in another thread.");
        }

        rb_thread_call_without_gvl([](void *rqPtr) -> void* {
            auto &rbq = *reinterpret_cast<RubyQueue*>(rqPtr);
            OptionalQueueItemType item;

            while ((item = rbq.pop())) {
                rb_thread_call_with_gvl([](void *arg) -> void * {
                    (*reinterpret_cast<QueueItemType *>(arg))();
                    return nullptr;
                }, &item);
            }
            return nullptr;
        }, this, nullptr, nullptr);
        return Qnil;
    }


}
#pragma clang diagnostic pop