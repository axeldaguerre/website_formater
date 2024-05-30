#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

////////////////////////////////
//~ rjf: Window Types

typedef struct OS_W32_TitleBarClientArea OS_W32_TitleBarClientArea;
struct OS_W32_TitleBarClientArea
{
 OS_W32_TitleBarClientArea *next;
 Rng2F32 rect;
};

typedef struct OS_W32_Window OS_W32_Window;
struct OS_W32_Window
{
 OS_W32_Window *next;
 OS_W32_Window *prev;
 HWND hwnd;
 HDC hdc;
 B32 last_window_placement_initialized;
 WINDOWPLACEMENT last_window_placement;
 OS_RepaintFunction *repaint;
 B32 custom_border;
 F32 custom_border_title_thickness;
 F32 custom_border_edge_thickness;
 Arena *paint_arena;
 OS_W32_TitleBarClientArea *first_title_bar_client_area;
 OS_W32_TitleBarClientArea *last_title_bar_client_area;
};

typedef UINT OS_W32_GetDpiForWindowType(HWND hwnd);

#define OS_W32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void *)-4)
typedef BOOL OS_W32_SetProcessDpiAwarenessContextFunctionType(void *value);

////////////////////////////////
//~ rjf: Main State

typedef struct OS_W32_GfxState OS_W32_GfxState;
struct OS_W32_GfxState
{
 Arena *arena;
 
 // rjf: metadata
 F32 refresh_rate;
 
 // rjf: root_global hwnd/hdc
 HWND root_global_hwnd;
 HDC root_global_hdc;
 
 // rjf: cursors
 OS_CursorKind cursor_kind;
 
 // rjf: windows
 Arena *window_arena;
 SRWLOCK window_srw_lock;
 OS_W32_Window *first_window;
 OS_W32_Window *last_window;
 OS_W32_Window *free_window;
};

////////////////////////////////
//~ rjf: root_globals

root_global OS_W32_GetDpiForWindowType *w32_GetDpiForWindow;
root_global OS_W32_GfxState *os_w32_gfx_state;
extern per_thread Arena *os_w32_tl_events_arena;
extern per_thread OS_EventList *os_w32_tl_events_list;

////////////////////////////////
//~ rjf: Helpers

internal OS_Handle OS_W32_HandleFromWindow(OS_W32_Window *window);
internal OS_W32_Window *OS_W32_WindowFromHandle(OS_Handle handle);
internal OS_Modifiers OS_W32_GetModifiers(void);

////////////////////////////////
//~ rjf: Window Proc

internal LRESULT OS_W32_WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

#endif // OS_GFX_WIN32_H
