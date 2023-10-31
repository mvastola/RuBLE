#include "management/RubyQueue.hpp"
#include "utils/ruby.hpp"
#include "utils/async.hpp"
#include <ruby/thread.h>

using namespace std::chrono_literals;

using namespace std::chrono_literals;
namespace SimpleRbBLE {
    constinit std::shared_ptr<RubyQueue> RubyQueue::_instance {};

    const std::shared_ptr<RubyQueue> &RubyQueue::instance() {
        if (!_instance) _instance = std::shared_ptr<RubyQueue>(new RubyQueue());
        return _instance;
    }

    Object RubyQueue::rb_thread() const {
        return { _rb_thread.load() };
    }

    void RubyQueue::start() {
        if (!_starting.test_and_set()) {
//            if constexpr (DEBUG) std::cerr << "Starting RubyQueue" << std::endl;
            RbThreadCreateFn<void> runThread = [](void *rubyQueuePtr) -> VALUE {
                return reinterpret_cast<RubyQueue*>(rubyQueuePtr)->run();
            };
            // TODO: figure out how/if to handle case where thread creation fails
            // TODO: wrap this in rice's `protect` fn
            RubyThreadId tid = rb_thread_create(runThread, this);
            if constexpr (DEBUG) std::cerr << "RubyQueue thread ID: " << tid << std::endl;
            _rb_thread.store(tid);
        }
    }
    void RubyQueue::ensure_started() {
        if (Utils::wait_until([&]{ return _startup_latch.try_acquire(); }, 15s)) {
//            std::cout << "RubyQueue started." << std::endl;
//            std::cerr << "SimpleRbBLE queue start ensured." << std::endl;
            return;
        }

        std::cerr << "SimpleRbBLE queue failed to start. Killing it." << std::endl;
        kill();
        throw std::runtime_error("SimpleRbBLE queue failed to start. Aborting.");
    }

    VALUE RubyQueue::run() {
        // TODO: add thread unblocking handlers
        auto loopFn = [](void *rqPtr) -> void * {
//            std::cout << "About to call RubyQueue::loop" << std::endl;
            reinterpret_cast<RubyQueue*>(rqPtr)->loop();
            return nullptr;
        };
        auto ubf = [](void *rqPtr) -> void {
            reinterpret_cast<RubyQueue*>(rqPtr)->stop();
        };
        rb_thread_call_without_gvl(loopFn, this, ubf, this);
        return Qnil;
    }

    void RubyQueue::loop() {
        std::lock_guard lck(_run_mtx);

        _thread_id.store(std::this_thread::get_id());
        _startup_latch.release();
        _running.test_and_set();
//        std::cerr << "SimpleRbBLE::RubyQueue has started." << std::endl;

        QueueItemType item;
        while (!_stopping.test()) {
            _sem.acquire();
            if (_stopping.test()) break;

            {
                std::lock_guard<std::mutex> lock(_mtx);
                if constexpr (DEBUG) assert(!_q->empty());
                item = std::move(_q->back());
                _q->pop_back();
            }

            rb_thread_call_with_gvl([](void *queueItem) -> void * {
                auto in_ruby_guard = in_ruby.assert_guard();
                (*reinterpret_cast<QueueItemType*>(queueItem))();
                return nullptr;
            }, &item);
        }
        _running.clear();
//        std::cout << "RubyQueue::loop exiting" << std::endl;
    }

    void RubyQueue::push(QueueItemType fn) {
        if (_stopping.test()) {
//            if (DEBUG) std::cerr << "Warning: attempted to push to SimpleRbBLE callback queue, ";
//            if (DEBUG) std::cerr << "but queue has been stopped. Refusing to push." << std::endl;
            return;
        }

        {
            std::lock_guard<std::mutex> lck(_mtx);
            _q->emplace_back(std::move(fn));
        }
        _sem.release();
    }

    void RubyQueue::stop() {
        if (!_stopping.test_and_set()) {
//            std::cerr << "Signalling RubyQueue to stop" << std::endl;
            _sem.release(); // if stop flag wasn't set before, notify cv
        }
        std::this_thread::yield();
    }

    // This is ensured to be accurate because it only returns true if
    // the _running std::atomic_flag is set AND the _run_mtx can't be acquired
    bool RubyQueue::running() const {
        if (!_running.test()) return false;

        // using a shared lock here means we don't have to worry about
        // multiple simultaneous calls to RubyQueue::running causing issues
        std::shared_lock<std::shared_mutex> lck(_run_mtx, std::defer_lock);
        // if we were are able to acquire the shared lock, obviously the queue isn't holding it
        // if so, we clear _running, because this means the queue previously set _running,
        // but isn't any longer
        if (lck.try_lock()) _running.clear();
        return !lck;
    }

    // returns if _stopping was requested AND we're not running
    bool RubyQueue::stopped() const {
        return _stopping.test() && !running();
    }

    void RubyQueue::stop_on_exit(VALUE) {
        std::shared_ptr<RubyQueue> rq = instance();

        rq->stop();
        if (rq->stopped()) return;

        if (Utils::wait_until([&]{ return !rq->running(); }, 1s)) return;

        std::cerr << "SimpleRbBLE::RubyQueue won't exit. Forcing it.." << std::endl;
        rq->kill();
    }

    void RubyQueue::kill() {
        stop();
        if (rb_thread().test()) rb_thread_kill(rb_thread());
        // TODO: is there any way to join threads? ruby's thread join functions
        //  don't seem to be exported (I guess we can use Rice::Object#call)
    }
}
