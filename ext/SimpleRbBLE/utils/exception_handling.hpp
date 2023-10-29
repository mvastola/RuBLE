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

namespace boost {
#ifdef HAVE_BOOST_STACKTRACE
    //NOLINTNEXTLINE(*-redundant-declaration)
    void assertion_failed_msg(char const *expr, char const *msg, char const *function, char const * /*file*/,
                                     long /*line*/);

    //NOLINTNEXTLINE(*-redundant-declaration)
    [[maybe_unused]] void assertion_failed(char const *expr, char const *function, char const *file, long line);
#endif
} // namespace boost

namespace SimpleRbBLE::ExceptionHandling {
#ifdef HAVE_BOOST_STACKTRACE

    [[maybe_unused]] void abort_with_stack();
#endif

#if defined(HAVE_BOOST_EXCEPTION) && defined(HAVE_BOOST_STACKTRACE)
    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

    template <typename T>
    [[maybe_unused]] boost::stacktrace::stacktrace extract_stacktrace(const T &e) {
        boost::stacktrace::stacktrace *st = boost::get_error_info<traced>(e);
        // TODO: does this method work / is it memory-safe
        boost::stacktrace::stacktrace &st2(*st);
        return st2;
    }

    template<class E>
    constexpr auto add_error_info(const E &e) {
        return boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
    }

    template<class E>
    constexpr void throw_with_trace(const E &e) {
        throw add_error_info(e);

    }
#else
    template<class E>
    constexpr void throw_with_trace(const E &e) {
        throw e;
    }
#endif

    void wrap_termination_handler();
    extern std::terminate_handler _old_terminate_handler;
}


