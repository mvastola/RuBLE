#pragma once

#include "config.h"
#ifdef HAVE_BOOST_STACKTRACE
#include <backtrace.h>
#include <boost/stacktrace.hpp>
#endif
#ifdef HAVE_BOOST_EXCEPTION
#include <boost/exception/all.hpp>
#endif
#include <iostream>     // std::cerr
#include <exception>    // std::set_terminate
#include <ruby/ruby.h>

#ifdef HAVE_BOOST_STACKTRACE
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
#endif

namespace SimpleRbBLE::ExceptionHandling {
    [[maybe_unused]] void abort_with_stack();

#if defined(HAVE_BOOST_EXCEPTION) && defined(HAVE_BOOST_STACKTRACE)
    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

    template <typename T>
    [[maybe_unused]] boost::stacktrace::stacktrace extract_stacktrace(const T &e) {
        boost::stacktrace::stacktrace *st = boost::get_error_info<traced>(e);
        // TODO: does this method work / is it memory-safe
        boost::stacktrace::stacktrace &st2(*st);
        return st2;
    }
#endif

    template<class E>
    constexpr auto add_backtrace(const E &e) {
#if defined(HAVE_BOOST_EXCEPTION) && defined(HAVE_BOOST_STACKTRACE)
        return boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
#else // no-op if boost isn't present
        return e;
#endif
    }

    template<class E>
    [[maybe_unused]] constexpr void throw_with_backtrace(const E &e) {
        throw add_backtrace(e);
    }

    void wrap_termination_handler();
    extern std::terminate_handler _old_terminate_handler;
}


