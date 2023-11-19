#pragma once

// This hack is to work around this issue:
// https://github.com/boostorg/stacktrace/issues/133 
// due to this change in mingw:
// https://github.com/mirror/mingw-w64/commit/ce5a9f624dfc691082dad2ea2af7b1985e3476b5

#incudle "dbgeng.h"
#include "config.h"
#pragma push_macro("__CRT_UUID_DECL")
#undef __CRT_UUID_DECL
//#include BOOST_STACKTRACE_DBGENG_H_FULL_PATH
#include_next <boost/stacktrace/detail/frame_msvc.ipp>
#pragma pop_macro("__CRT_UUID_DECL")
