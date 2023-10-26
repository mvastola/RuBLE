#include "common.hpp"
#include "utils.hpp"
#include "LambdaProps.hpp"

namespace SimpleRbBLE {
    namespace Utils {

        void thread_sleep() {
            rb_thread_schedule();
            // TODO: maybe multiply sleep time by getloadavg() on linux (from cstdlib)
            std::this_thread::sleep_for(250ms);
            std::this_thread::yield();
        }

    }

    Module rb_mUtils;
    void Init_Utils() {
        rb_mUtils = define_module_under(rb_mSimpleRbBLE, "Utils");
        if (!DEBUG) return;
        rb_mUtils.define_singleton_function("maybe_swap_endianness", &Utils::maybe_swap_endianness<uint64_t>);
        rb_mUtils.define_singleton_function("get_props_array", &LambdaProps::get_props_array);
    }
}