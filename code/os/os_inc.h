#ifndef OS_INC_H
#define OS_INC_H

#if !defined(ArenaImpl_Reserve)
# define ArenaImpl_Reserve  OS_Reserve
#endif
#if !defined(ArenaImpl_Release)
# define ArenaImpl_Release  OS_Release
#endif
#if !defined(ArenaImpl_Commit)
# define ArenaImpl_Commit   OS_Commit
#endif
#if !defined(ArenaImpl_Decommit)
# define ArenaImpl_Decommit OS_Decommit
#endif

#if !defined(ThreadCtxImpl_SetThreadName)
# define ThreadCtxImpl_SetThreadName OS_SetThreadName
#endif

#include "os/core/os_core.h"
#if defined(OS_FEATURE_GFX)
#include "os/gfx/os_gfx.h"
#endif
#if defined(OS_FEATURE_NET)
#include "os/net/os_net.h"
#endif

#if OS_WINDOWS
# include "os/core/win32/os_core_win32.h"
# if defined(OS_FEATURE_GFX)
#  include "os/gfx/win32/os_gfx_win32.h"
# endif
# if defined(OS_FEATURE_NET)
#  include "os/net/win32/os_net_win32.h"
# endif
#else
# error OS layer not implemented.
#endif

#endif // OS_INC_H
