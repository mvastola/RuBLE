#include "exception_handling.hpp"

namespace RuBLE::ExceptionHandling {
    std::terminate_handler _old_terminate_handler = nullptr;



    void wrap_termination_handler() {
        if (_old_terminate_handler != nullptr) {
            throw std::runtime_error("Old terminate exception already set. Would overflow.");
        }
        _old_terminate_handler = std::get_terminate();

        std::set_terminate([] {
            std::exception_ptr ex = std::current_exception();

//            std::cerr << "terminate called after throwing an instance of '";
//            std::cerr << Utils::human_type_name(ex) << "'" << std::endl;
//            std::cerr << "what():  " << ex
            try {
                std::cerr << "C++ Backtrace: " << std::endl;
                std::cerr << boost::stacktrace::stacktrace() << std::endl;
            } catch (const std::exception &ex2) {
                std::throw_with_nested(ex); // TODO: test that this does what I think it does
            }
            try {
                std::cerr << "Ruby Backtrace: " << std::endl;
                rb_backtrace();
                std::cerr << std::endl << std::endl;
            } catch (const std::exception &ex2) {
                std::throw_with_nested(ex);
            }

            if (_old_terminate_handler != nullptr) _old_terminate_handler();
            // TODO: consider using boost::core::verbose_terminate_handler somewhere
        });
    }

    [[maybe_unused]] void abort_with_stack() {
        try {
            std::cerr << boost::stacktrace::stacktrace();
        } catch (const std::exception &ex2) {
            std::cerr << "Could not generate C++ stacktrace. Skipping." << std::endl;
        }
        std::abort();
    }

    void throw_exception() { throw std::runtime_error("test error"); }
}

