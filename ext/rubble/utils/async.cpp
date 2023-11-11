#include "async.hpp"

namespace RubBLE::Utils::Async {
    void thread_sleep() {
        rb_thread_schedule();
        // TODO: maybe multiply sleep time by getloadavg() on linux (from cstdlib)
        std::this_thread::sleep_for(250ms);
        std::this_thread::yield();
    }
}