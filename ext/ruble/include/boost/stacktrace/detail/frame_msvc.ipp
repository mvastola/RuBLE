#pragma once

#include "config.h"
#ifdef STACKTRACE_BACKEND_IS_WINDBG
#pragma push_macro("__CRT_UUID_DECL")
#undef __CRT_UUID_DECL
//#include BOOST_STACKTRACE_DBGENG_H_FULL_PATH
#include_next <boost/stacktrace/detail/frame_msvc.ipp>
#pragma pop_macro("__CRT_UUID_DECL")
#endif
