#pragma once

#include "config.h"
#include <backtrace.h>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <iostream>     // std::cerr
#include <exception>    // std::set_terminate
#include <ruby/ruby.h>

namespace boost {
    inline void assertion_failed_msg(const char *expr, const char *msg, const char *function, const char *, long) {
        std::cerr << "Expression '" << expr << "' is false in function '" << function << "': " << (msg ? msg : "<...>")
                  << ".\nBacktrace:\n" << boost::stacktrace::stacktrace() << '\n';

        std::abort();
    }

    [[maybe_unused]] inline void assertion_failed(const char *expr, const char *function, const char *file, long line) {
        ::boost::assertion_failed_msg(expr, nullptr, function, file, line);
    }
} // namespace boost

namespace RuBLE::ExceptionHandling {
    [[noreturn]] void throw_exception();
    [[maybe_unused,noreturn]] void abort_with_stack();

    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

    template <typename T>
    [[maybe_unused]] boost::stacktrace::stacktrace extract_stacktrace(const T &e) {
        boost::stacktrace::stacktrace *st = boost::get_error_info<traced>(e);
        // TODO: does this method work / is it memory-safe
        boost::stacktrace::stacktrace &st2(*st);
        return st2;
    }

    template<class E>
    constexpr auto add_backtrace(const E &e) {
        return boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
    }

    // TODO: see if we can somehow add C++ backtrace data to ruby exceptions
    template<class E>
    constexpr void throw_with_backtrace(const E &e) {
        throw add_backtrace(e);
    }

    void wrap_termination_handler();
    extern std::terminate_handler _old_terminate_handler;
}


