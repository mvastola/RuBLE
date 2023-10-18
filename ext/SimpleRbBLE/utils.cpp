#include "common.hpp"
#include "utils.hpp"

namespace SimpleRbBLE {
    namespace Utils {
        template<>
        std::string human_type_name(const std::type_info &type) {
            int status;
            std::string original_name = type.name();
            std::string result;
            char *result_ptr = abi::__cxa_demangle(original_name.c_str(), nullptr, nullptr, &status);

            if (result_ptr == nullptr || status) {
                std::cerr << "Warning. Demangling of type name " << original_name;
                std::cerr << " failed with status " << status << std::endl;
                result = original_name;
            } else {
                result = result_ptr;
            }
            if (result_ptr != nullptr) std::free(result_ptr);
            return result;
        }

        template<>
        std::string human_type_name(const std::any &any) { return human_type_name(any.type()); }

        std::string basic_object_inspect_start(const Object &o) {
            std::ostringstream oss;
            oss << "#<" << o.class_name();
            oss << ":" << to_hex_string(reinterpret_cast<uint64_t>(o.value()));
            return oss.str();
        }

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
    }
}