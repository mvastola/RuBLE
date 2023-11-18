#pragma once

#ifdef STACKTRACE_BACKEND_IS_WINDBG
#pragma push_macro("T")
#undef __CRT_UUID_DECL
#include_next "dbgeng.h"
#pragma pop_macro("T")
#endif
