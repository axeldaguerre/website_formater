/* date = September 9th 2023 2:03 pm */

#ifndef BASE_MARKUP_H
#define BASE_MARKUP_H

//- rjf: metagen-ingested code markup
#define TweakB32(name, default)           (TWEAK_##name)
#define TweakF32(name, default, min, max) (TWEAK_##name)

//- rjf: telemetry profiling backend
#if BUILD_TELEMETRY
# include "rad_tm.h"
# pragma comment(lib, "rad_tm_win64.lib")
# define ProfThreadName(...) tmThreadName(0, 0, __VA_ARGS__)
# define ProfBeginCapture(name) tmOpen(0, (name), __DATE__, "localhost", TMCT_TCP, TELEMETRY_DEFAULT_PORT, TMOF_INIT_NETWORKING|TMOF_CAPTURE_CONTEXT_SWITCHES, 100)
# define ProfEndCapture(...) tmClose(0)
# define ProfCaptureIsActive(...) tmRunning()
# define ProfTick(...) tmTick(0)
# define ProfBegin(...) TM_API_PTR->_tmEnterZoneV_Core(0, 0, __FILE__, &g_telemetry_filename_id, __LINE__, __VA_ARGS__)
# define ProfEnd(...) TM_API_PTR->_tmLeaveZone(0)
#endif

//- rjf: zeroify if undefined
#if !defined(ProfBegin)
# define ProfThreadName(...)
# define ProfBeginCapture(name)
# define ProfEndCapture(...)
# define ProfCaptureIsActive(...)
# define ProfTick(...)
# define ProfBegin(...) (0)
# define ProfEnd(...) (0)
#endif

//- rjf: helpers
#define ProfBeginFunction(...) ProfBegin(this_function_name)
#define ProfScope(...) DeferLoop(ProfBegin(__VA_ARGS__), ProfEnd())

#endif //BASE_MARKUP_H
