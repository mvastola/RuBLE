#include "exception_handling.hpp"

namespace SimpleRbBLE::ExceptionHandling {
    std::terminate_handler _old_terminate_handler = nullptr;

    void wrap_termination_handler() {
        if (_old_terminate_handler != nullptr) {
            throw std::runtime_error("Old terminate exception already set. Would overflow.");
        }
        _old_terminate_handler = std::get_terminate();

        std::set_terminate([] {
            std::exception_ptr ex = std::current_exception();
#ifdef HAVE_BOOST_STACKTRACE
            try {
                std::cerr << "C++ Backtrace: " << std::endl;
                std::cerr << boost::stacktrace::stacktrace();
            } catch (const std::exception &ex2) {
                std::throw_with_nested(ex);
            }
#endif
            try {
                std::cerr << "Ruby Backtrace: " << std::endl;
                rb_backtrace();
                std::cerr << std::endl << std::endl;
            } catch (const std::exception &ex2) {
                std::throw_with_nested(ex);
            }

            if (_old_terminate_handler != nullptr) _old_terminate_handler();

        });
    }

    [[maybe_unused]] void abort_with_stack() {
#ifdef HAVE_BOOST_STACKTRACE
        try {
            std::cerr << boost::stacktrace::stacktrace();
        } catch (const std::exception &ex2) {
            std::cerr << "Could not generate C++ stacktrace. Skipping." << std::endl;
        }
#endif
        std::abort();
    }
}

