#include "os/core/os_core.c"
#if defined(OS_FEATURE_GFX)
#include "os/gfx/os_gfx.c"
#endif
#if defined(OS_FEATURE_NET)
#include "os/net/os_net.c"
#endif

#if OS_WINDOWS
# include "os/core/win32/os_core_win32.c"
# if defined(OS_FEATURE_GFX)
#  include "os/gfx/win32/os_gfx_win32.c"
# endif
# if defined(OS_FEATURE_NET)
#  include "os/net/win32/os_net_win32.c"
# endif
#else
# error OS layer not implemented.
#endif
