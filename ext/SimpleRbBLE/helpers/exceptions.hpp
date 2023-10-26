#pragma once

#include RUBY_EXTCONF_H
#include <exception>    // std::set_terminate
#include <iostream>     // std::cerr
#ifdef BOOST_STACKTRACE_SUPPORT
#include <boost/stacktrace.hpp>
#endif
#ifdef HAVE_BOOST_EXCEPTION_ALL_HPP
#include <boost/exception/all.hpp>
#endif

namespace boost {
#ifdef BOOST_STACKTRACE_SUPPORT
    inline void assertion_failed_msg(char const *expr, char const *msg, char const *function, char const * /*file*/,
                                     long /*line*/) {
        std::cerr << "Expression '" << expr << "' is false in function '" << function << "': " << (msg ? msg : "<...>")
                  << ".\nBacktrace:\n" << boost::stacktrace::stacktrace() << '\n';

        std::abort();
    }

    inline constexpr void assertion_failed(char const *expr, char const *function, char const *file, long line) {
        ::boost::assertion_failed_msg(expr, 0 /*nullptr*/, function, file, line);
    }
#endif
} // namespace boost

namespace SimpleRbBLE::Exceptions {
#ifndef BOOST_STACKTRACE_SUPPORT
#else
    inline void abort_with_stack() {
        try {
            std::cerr << boost::stacktrace::stacktrace();
        } catch (...) {}
        std::abort();
    }
#endif

#ifdef HAVE_BOOST_EXCEPTION_ALL_HPP
    using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

    constexpr boost::stacktrace::stacktrace &&extract_stacktrace(const auto &e) {
        boost::stacktrace::stacktrace *st = boost::get_error_info<traced>(e);
        // TODO: does this method work / is it memory-safe
        boost::stacktrace::stacktrace &st2(*st);
        return std::move(st2);
    }

    template<class E>
    constexpr void throw_with_trace(const E &e) {
        throw boost::enable_error_info(e)
                << traced(boost::stacktrace::stacktrace());
    }
#else
    template<class E>
    constexpr void throw_with_trace(const E &e) {
        throw e;
    }
#endif
}


