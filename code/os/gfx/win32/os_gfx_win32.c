#include <uxtheme.h>
#undef DeleteFile
#undef IsMaximized
#include <dwmapi.h>
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "UxTheme")
#pragma comment(lib, "ole32")

#define OS_W32_GraphicalWindowClassName L"ApplicationWindowClass"

////////////////////////////////
//~ rjf: root_globals

#if BUILD_CORE
OS_W32_GetDpiForWindowType *w32_GetDpiForWindow = 0;
OS_W32_GfxState *os_w32_gfx_state = 0;
per_thread Arena *os_w32_tl_events_arena;
per_thread OS_EventList *os_w32_tl_events_list;
#endif

////////////////////////////////
//~ rjf: Helpers

internal OS_Handle
OS_W32_HandleFromWindow(OS_W32_Window *window)
{
 OS_Handle handle = {0};
 handle.u64[0] = (U64)window;
 return handle;
}

internal OS_W32_Window *
OS_W32_WindowFromHandle(OS_Handle handle)
{
 OS_W32_Window *w = (OS_W32_Window *)handle.u64[0];
 return w;
}

internal OS_Modifiers
OS_W32_GetModifiers(void)
{
 OS_Modifiers modifiers = 0;
 if(GetKeyState(VK_CONTROL) & 0x8000)
 {
  modifiers |= OS_Modifier_Ctrl;
 }
 if(GetKeyState(VK_SHIFT) & 0x8000)
 {
  modifiers |= OS_Modifier_Shift;
 }
 if(GetKeyState(VK_MENU) & 0x8000)
 {
  modifiers |= OS_Modifier_Alt;
 }
 return modifiers;
}

////////////////////////////////
//~ rjf: Window Proc

internal LRESULT
OS_W32_WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
 LRESULT result = 0;
 OS_Event *event = 0;
 OS_W32_Window *window = (OS_W32_Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
 OS_Handle window_handle = OS_W32_HandleFromWindow(window);
 Temp scratch = ScratchBegin(&os_w32_tl_events_arena, 1);
 OS_EventList fallback_event_list = {0};
 if(os_w32_tl_events_arena == 0)
 {
  os_w32_tl_events_arena = scratch.arena;
  os_w32_tl_events_list = &fallback_event_list;
 }
 B32 is_release = 0;
 Axis2 scroll_axis = Axis2_Y;
 switch(message)
 {
  default:
  {
   result = DefWindowProcW(hwnd, message, w_param, l_param);
  }break;
  
  //- rjf: general window events
  case WM_CLOSE:
  {
   event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
   event->kind = OS_EventKind_WindowClose;
   event->window = window_handle;
  }break;
  case WM_KILLFOCUS:
  {
   event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
   event->kind = OS_EventKind_WindowLoseFocus;
   event->window = window_handle;
   ReleaseCapture();
  }break;
  
  //- rjf: mouse buttons
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  {
   ReleaseCapture();
   is_release = 1;
  }fallthrough;
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
  {
   if(is_release == 0)
   {
    SetCapture(hwnd);
   }
   OS_EventKind kind = is_release ? OS_EventKind_Release : OS_EventKind_Press;
   OS_Key key = OS_Key_MouseLeft;
   switch(message)
   {
    case WM_MBUTTONUP: case WM_MBUTTONDOWN: key = OS_Key_MouseMiddle; break;
    case WM_RBUTTONUP: case WM_RBUTTONDOWN: key = OS_Key_MouseRight; break;
   }
   event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
   event->kind = kind;
   event->window = window_handle;
   event->key = key;
   event->position = OS_MouseFromWindow(window_handle);
  }break;
  
  //- rjf: mouse wheel
  case WM_MOUSEHWHEEL: scroll_axis = Axis2_X; goto scroll;
  case WM_MOUSEWHEEL:
  scroll:;
  {
   S16 wheel_delta = HIWORD(w_param);
   event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
   event->kind = OS_EventKind_MouseScroll;
   event->window = window_handle;
   event->scroll.v[scroll_axis] = -(F32)wheel_delta;
  }break;
  
  //- rjf: cursor setting
  case WM_SETCURSOR:
  {
   if(Contains2F32(OS_ClientRectFromWindow(window_handle), OS_MouseFromWindow(window_handle)) && os_w32_gfx_state->cursor_kind != OS_CursorKind_Null)
   {
    // TODO(rjf): remove statics
    local_persist B32 table_initialized = 0;
    local_persist HCURSOR cursor_table[OS_CursorKind_COUNT];
    if(table_initialized == 0)
    {
     table_initialized = 1;
     cursor_table[OS_CursorKind_Pointer]                  = LoadCursorA(0, IDC_ARROW);
     cursor_table[OS_CursorKind_Hand]                     = LoadCursorA(0, IDC_HAND);
     cursor_table[OS_CursorKind_WestEast]                 = LoadCursorA(0, IDC_SIZEWE);
     cursor_table[OS_CursorKind_NorthSouth]               = LoadCursorA(0, IDC_SIZENS);
     cursor_table[OS_CursorKind_NorthEastSouthWest]       = LoadCursorA(0, IDC_SIZENESW);
     cursor_table[OS_CursorKind_NorthWestSouthEast]       = LoadCursorA(0, IDC_SIZENWSE);
     cursor_table[OS_CursorKind_AllCardinalDirections]    = LoadCursorA(0, IDC_SIZEALL);
     cursor_table[OS_CursorKind_IBar]                     = LoadCursorA(0, IDC_IBEAM);
     cursor_table[OS_CursorKind_Blocked]                  = LoadCursorA(0, IDC_NO);
     cursor_table[OS_CursorKind_Loading]                  = LoadCursorA(0, IDC_WAIT);
     cursor_table[OS_CursorKind_Pan]                      = LoadCursorA(0, IDC_SIZEALL);
    }
    if(os_w32_gfx_state->cursor_kind == OS_CursorKind_Hidden)
    {
     ShowCursor(0);
    }
    else
    {
     ShowCursor(1);
     SetCursor(cursor_table[os_w32_gfx_state->cursor_kind]);
    }
   }
   else
   {
    result = DefWindowProcW(hwnd, message, w_param, l_param);
   }
  }break;
  
  //- rjf: keyboard events
  case WM_SYSKEYDOWN: case WM_SYSKEYUP:
  {
   DefWindowProcW(hwnd, message, w_param, l_param);
  }fallthrough;
  case WM_KEYDOWN: case WM_KEYUP:
  {
   B32 was_down = !!(l_param & (1 << 30));
   B32 is_down =   !(l_param & (1 << 31));
   OS_EventKind kind = is_down ? OS_EventKind_Press : OS_EventKind_Release;
   
   // TODO(rjf): remove statics
   local_persist OS_Key key_table[256] = {0};
   local_persist B32 key_table_initialized = 0;
   if(!key_table_initialized)
   {
    key_table_initialized = 1;
    
    for (U32 i = 'A', j = OS_Key_A; i <= 'Z'; i += 1, j += 1)
    {
     key_table[i] = (OS_Key)j;
    }
    for (U32 i = '0', j = OS_Key_0; i <= '9'; i += 1, j += 1)
    {
     key_table[i] = (OS_Key)j;
    }
    for (U32 i = VK_F1, j = OS_Key_F1; i <= VK_F24; i += 1, j += 1)
    {
     key_table[i] = (OS_Key)j;
    }
    
    key_table[VK_ESCAPE]        = OS_Key_Esc;
    key_table[VK_OEM_3]         = OS_Key_GraveAccent;
    key_table[VK_OEM_MINUS]     = OS_Key_Minus;
    key_table[VK_OEM_PLUS]      = OS_Key_Equal;
    key_table[VK_BACK]          = OS_Key_Backspace;
    key_table[VK_TAB]           = OS_Key_Tab;
    key_table[VK_SPACE]         = OS_Key_Space;
    key_table[VK_RETURN]        = OS_Key_Enter;
    key_table[VK_CONTROL]       = OS_Key_Ctrl;
    key_table[VK_SHIFT]         = OS_Key_Shift;
    key_table[VK_MENU]          = OS_Key_Alt;
    key_table[VK_UP]            = OS_Key_Up;
    key_table[VK_LEFT]          = OS_Key_Left;
    key_table[VK_DOWN]          = OS_Key_Down;
    key_table[VK_RIGHT]         = OS_Key_Right;
    key_table[VK_DELETE]        = OS_Key_Delete;
    key_table[VK_PRIOR]         = OS_Key_PageUp;
    key_table[VK_NEXT]          = OS_Key_PageDown;
    key_table[VK_HOME]          = OS_Key_Home;
    key_table[VK_END]           = OS_Key_End;
    key_table[VK_OEM_2]         = OS_Key_ForwardSlash;
    key_table[VK_OEM_PERIOD]    = OS_Key_Period;
    key_table[VK_OEM_COMMA]     = OS_Key_Comma;
    key_table[VK_OEM_7]         = OS_Key_Quote;
    key_table[VK_OEM_4]         = OS_Key_LeftBracket;
    key_table[VK_OEM_6]         = OS_Key_RightBracket;
    key_table[VK_INSERT]        = OS_Key_Insert;
    key_table[VK_OEM_1]         = OS_Key_Semicolon;
   }
   
   OS_Key key = OS_Key_Null;
   if(w_param < ArrayCount(key_table))
   {
    key = key_table[w_param];
   }
   
   event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
   event->kind = kind;
   event->window = window_handle;
   event->key = key;
  }break;
  
  //- rjf: windows-wide shortcuts
  case WM_SYSCOMMAND:
  {
   switch (w_param)
   {
    case SC_CLOSE:
    {
     event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
     event->kind = OS_EventKind_WindowClose;
     event->window = window_handle;
    } break;
    case SC_KEYMENU:
    {}break;
    default:
    {
     result = DefWindowProcW(hwnd, message, w_param, l_param);
    }break;
   }
  }break;
  
  //- rjf: text input
  case WM_CHAR: case WM_SYSCHAR:
  {
   U32 char_input = w_param;
   if(char_input == '\r')
   {
    char_input = '\n';
   }
   if((char_input >= 32 && char_input != 127) || char_input == '\t' || char_input == '\n')
   {
    event = PushArray(os_w32_tl_events_arena, OS_Event, 1);
    event->kind = OS_EventKind_Text;
    event->window = window_handle;
    event->character = char_input;
   }
  }break;
  
  //- rjf: dpi changes
  case WM_DPICHANGED:
  {
   F32 new_dpi = (F32)w_param;
   result = DefWindowProcW(hwnd, message, w_param, l_param);
  }break;
  
  //- rjf: painting
  case WM_PAINT:
  {
   if(window != 0 && window->repaint != 0)
   {
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    window->repaint();
    EndPaint(hwnd, &ps);
   }
   result = DefWindowProcW(hwnd, message, w_param, l_param);
  }break;
  
  //- rjf: [custom border] activation - without this `result`, stuff flickers.
  case WM_NCACTIVATE:
  {
   if(window == 0 || window->custom_border == 0)
   {
    result = DefWindowProcW(hwnd, message, w_param, l_param);
   }
   else
   {
    result = 1;
   }
  }break;
  
  //- rjf: [custom border] client/window size calculation
  case WM_NCCALCSIZE:
  {
   if(window == 0 || window->custom_border == 0)
   {
    result = DefWindowProcW(hwnd, message, w_param, l_param);
   }
   else
   {
    MARGINS m = {0, 0, 0, 0};
    RECT *r = (RECT *)l_param;
    if(IsZoomed(hwnd))
    {
     int x_push_in = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
     int y_push_in = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
     r->left   += x_push_in;
     r->top    += y_push_in;
     r->bottom -= x_push_in;
     r->right  -= y_push_in;
     m.cxLeftWidth = m.cxRightWidth = x_push_in;
     m.cyTopHeight = m.cyBottomHeight = y_push_in;
    }
    DwmExtendFrameIntoClientArea(hwnd, &m);
   }
  }break;
  
  //- rjf: [custom border] client/window hit testing (mapping mouse -> action)
  case WM_NCHITTEST:
  {
   if(window == 0 || window->custom_border == 0)
   {
    result = DefWindowProcW(hwnd, message, w_param, l_param);
   }
   else
   {
    POINT pos_monitor;
    pos_monitor.x = GET_X_LPARAM(l_param);
    pos_monitor.y = GET_Y_LPARAM(l_param);
    POINT pos_client = pos_monitor;
    ScreenToClient(hwnd, &pos_client);
    
    //- rjf: check against window boundaries
    RECT frame_rect;
    GetWindowRect(hwnd, &frame_rect);
    B32 is_over_window = (frame_rect.left <= pos_monitor.x && pos_monitor.x < frame_rect.right &&
                          frame_rect.top <= pos_monitor.y && pos_monitor.y < frame_rect.bottom);
    
    //- rjf: check against borders
    B32 is_over_left   = 0;
    B32 is_over_right  = 0;
    B32 is_over_top    = 0;
    B32 is_over_bottom = 0;
    {
     RECT rect;
     GetClientRect(hwnd, &rect);
     if(!IsZoomed(hwnd))
     {
      if(rect.left <= pos_client.x && pos_client.x < rect.left + window->custom_border_edge_thickness)
      {
       is_over_left = 1;
      }
      if(rect.right - window->custom_border_edge_thickness <= pos_client.x && pos_client.x < rect.right)
      {
       is_over_right = 1;
      }
      if(rect.bottom - window->custom_border_edge_thickness <= pos_client.y && pos_client.y < rect.bottom)
      {
       is_over_bottom = 1;
      }
      if(rect.top <= pos_client.y && pos_client.y < rect.top + window->custom_border_edge_thickness)
      {
       is_over_top = 1;
      }
     }
    }
    
    //- rjf: check against title bar
    B32 is_over_title_bar = 0;
    {
     RECT rect;
     GetClientRect(hwnd, &rect);
     is_over_title_bar = (rect.left <= pos_client.x && pos_client.x < rect.right &&
                          rect.top <= pos_client.y && pos_client.y < rect.top + window->custom_border_title_thickness);
    }
    
    //- rjf: check against title bar client areas
    B32 is_over_title_bar_client_area = 0;
    for(OS_W32_TitleBarClientArea *area = window->first_title_bar_client_area;
        area != 0;
        area = area->next)
    {
     Rng2F32 rect = area->rect;
     if(rect.x0 <= pos_client.x && pos_client.x < rect.x1 &&
        rect.y0 <= pos_client.y && pos_client.y < rect.y1)
     {
      is_over_title_bar_client_area = 1;
      break;
     }
    }
    
    //- rjf: resolve hovering to result
    result = HTNOWHERE;
    if(is_over_window)
    {
     // rjf: default to client area
     result = HTCLIENT;
     
     // rjf: title bar
     if(is_over_title_bar)
     {
      result = HTCAPTION;
     }
     
     // rjf: title bar client area
     if(is_over_title_bar_client_area)
     {
      result = HTCLIENT;
     }
     
     // rjf: normal edges
     if(is_over_left)   { result = HTLEFT; }
     if(is_over_right)  { result = HTRIGHT; }
     if(is_over_top)    { result = HTTOP; }
     if(is_over_bottom) { result = HTBOTTOM; }
     
     // rjf: corners
     if(is_over_left  && is_over_top)    { result = HTTOPLEFT; }
     if(is_over_left  && is_over_bottom) { result = HTBOTTOMLEFT; }
     if(is_over_right && is_over_top)    { result = HTTOPRIGHT; }
     if(is_over_right && is_over_bottom) { result = HTBOTTOMRIGHT; }
    }
   }
  }break;
  
 }
 
 //- rjf: set modifiers & push
 if(event)
 {
  event->modifiers = OS_W32_GetModifiers();
  DLLPushBack(os_w32_tl_events_list->first, os_w32_tl_events_list->last, event);
  os_w32_tl_events_list->count += 1;
 }
 
 ScratchEnd(scratch);
 return result;
}

////////////////////////////////
//~ rjf: @os_per_backend Main API

internal OS_InitGfxReceipt
OS_InitGfx(OS_InitReceipt os_init_receipt)
{
 if(IsMainThread() && os_w32_gfx_state == 0)
 {
  //- rjf: make root_global state
  {
   Arena *arena = ArenaAlloc(Gigabytes(1));
   os_w32_gfx_state = PushArray(arena, OS_W32_GfxState, 1);
   os_w32_gfx_state->arena = arena;
   os_w32_gfx_state->window_arena = ArenaAlloc(Gigabytes(1));
   InitializeSRWLock(&os_w32_gfx_state->window_srw_lock);
  }
  
  //- rjf: set dpi awareness
  OS_W32_SetProcessDpiAwarenessContextFunctionType *set_dpi_awareness_function = 0;
  HMODULE module = LoadLibraryA("user32.dll");
  if(module != 0)
  {
   set_dpi_awareness_function =
   (OS_W32_SetProcessDpiAwarenessContextFunctionType *)GetProcAddress(module, "SetProcessDpiAwarenessContext");
   FreeLibrary(module);
  }
  if(set_dpi_awareness_function != 0)
  {
   set_dpi_awareness_function(OS_W32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  }
  
  //- rjf: load DPI fallback
  {
   HMODULE user32 = LoadLibraryA("user32.dll");
   w32_GetDpiForWindow = (OS_W32_GetDpiForWindowType*)GetProcAddress(user32, "GetDpiForWindow");
   FreeLibrary(user32);
  }
  
  //- rjf: register window class
  {
   WNDCLASSW window_class = {0};
   window_class.style = CS_HREDRAW | CS_VREDRAW;
   window_class.lpfnWndProc = OS_W32_WindowProc;
   window_class.hInstance = os_w32_hinstance;
   window_class.lpszClassName = OS_W32_GraphicalWindowClassName;
   window_class.hCursor = LoadCursor(0, IDC_ARROW);
   RegisterClassW(&window_class);
  }
  
  //- rjf: make root_global invisible window
  {
   os_w32_gfx_state->root_global_hwnd = CreateWindowW(OS_W32_GraphicalWindowClassName,
                                                 L"",
                                                 WS_OVERLAPPEDWINDOW,
                                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                                 100, 100,
                                                 0, 0,
                                                 os_w32_hinstance, 0);
   os_w32_gfx_state->root_global_hdc = GetDC(os_w32_gfx_state->root_global_hwnd);
  }
  
  
  //- rjf: find refresh rate
  {
   os_w32_gfx_state->refresh_rate = 60.f;
   DEVMODEA device_mode = {0};
   if(EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &device_mode))
   {
    os_w32_gfx_state->refresh_rate = (F32)device_mode.dmDisplayFrequency;
   }
  }
 }
 OS_InitGfxReceipt receipt = {0};
 return receipt;
}

////////////////////////////////
//~ rjf: @os_per_backend System Info

internal F32
OS_DefaultRefreshRate(void)
{
 return os_w32_gfx_state->refresh_rate;
}

////////////////////////////////
//~ rjf: @os_per_backend Windows

//- rjf: open/closing/metadata/equipment

internal OS_Handle
OS_WindowOpen(OS_WindowFlags flags, Vec2S64 size, String8 title)
{
 OS_Handle handle = {0};
 AcquireSRWLockExclusive(&os_w32_gfx_state->window_srw_lock);
 {
  //- rjf: allocate window
  OS_W32_Window *window = os_w32_gfx_state->free_window;
  {
   if(window != 0)
   {
    StackPop(os_w32_gfx_state->free_window);
   }
   else
   {
    window = PushArrayNoZero(os_w32_gfx_state->window_arena, OS_W32_Window, 1);
   }
   MemoryZeroStruct(window);
   DLLPushBack(os_w32_gfx_state->first_window, os_w32_gfx_state->last_window, window);
  }
  
  //- rjf: open window
  HWND hwnd = 0;
  HDC hdc = 0;
  {
   Temp scratch = ScratchBegin(0, 0);
   String16 title16 = Str16From8(scratch.arena, title);
   hwnd = CreateWindowW(OS_W32_GraphicalWindowClassName, (LPCWSTR)title16.str, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, size.x, size.y, 0, 0, os_w32_hinstance, 0);
   hdc = GetDC(hwnd);
   SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
   ScratchEnd(scratch);
  }
  
  //- rjf: fill window
  {
   window->hwnd = hwnd;
   window->hdc = hdc;
  }
  
  //- rjf: custom border
  if(flags & OS_WindowFlag_CustomBorder)
  {
   SetWindowTheme(hwnd, L" ", L" ");
   window->custom_border = 1;
   window->paint_arena = ArenaAlloc(Kilobytes(4));
  }
  
  handle = OS_W32_HandleFromWindow(window);
 }
 ReleaseSRWLockExclusive(&os_w32_gfx_state->window_srw_lock);
 return handle;
}

internal void
OS_WindowClose(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 AcquireSRWLockExclusive(&os_w32_gfx_state->window_srw_lock);
 {
  DLLRemove(os_w32_gfx_state->first_window, os_w32_gfx_state->last_window, window);
  StackPush(os_w32_gfx_state->free_window, window);
  if(window->hdc)
  {
   ReleaseDC(window->hwnd, window->hdc);
  }
  if(window->hwnd)
  {
   DestroyWindow(window->hwnd);
  }
  if(window->paint_arena)
  {
   ArenaRelease(window->paint_arena);
  }
 }
 ReleaseSRWLockExclusive(&os_w32_gfx_state->window_srw_lock);
}

internal void
OS_WindowSetTitle(OS_Handle handle, String8 title)
{
 Temp scratch = ScratchBegin(0, 0);
 OS_W32_Window *w = OS_W32_WindowFromHandle(handle);
 String16 title16 = Str16From8(scratch.arena, title);
 SetWindowTextW(w->hwnd, (WCHAR *)title16.str);
 ScratchEnd(scratch);
}

internal void
OS_WindowSetIcon(OS_Handle handle, Vec2S32 size, String8 rgba_data)
{
 Temp scratch = ScratchBegin(0, 0);
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 {
  S32 size_area = size.x*size.y;
  HICON icon = NULL;
  ICONINFO icon_info = {0};
  icon_info.fIcon = TRUE;
  
  U8 *bgra_data = PushArrayNoZero(scratch.arena, U8, 4 * size_area);
  U8 *rgba = rgba_data.str;
  U8 *bgra = bgra_data;
  for(int i_pixel = 0; i_pixel < size_area; ++i_pixel, rgba += 4, bgra += 4)
  {
   bgra[0] = rgba[2];
   bgra[1] = rgba[1];
   bgra[2] = rgba[0];
   bgra[3] = rgba[3];
  }
  
  icon_info.hbmColor = CreateBitmap(size.x, size.y, 1, 32, bgra_data);
  if(icon_info.hbmColor)
  {
   icon_info.hbmMask = CreateCompatibleBitmap(window->hdc, size.x, size.y);
   if(icon_info.hbmMask)
   {
    icon = CreateIconIndirect(&icon_info);
    DeleteObject(icon_info.hbmMask);
   }
   DeleteObject(icon_info.hbmColor);
  }
  
  if(icon)
  {
   SendMessage(window->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
   SendMessage(window->hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
  }
 }
 ScratchEnd(scratch);
}

internal void
OS_WindowSetRepaint(OS_Handle handle, OS_RepaintFunction *repaint)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 window->repaint = repaint;
}

//- rjf: per-paint custom border hit-testing info

internal void
OS_WindowClearCustomBorderData(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 ArenaClear(window->paint_arena);
 window->first_title_bar_client_area = window->last_title_bar_client_area = 0;
 window->custom_border_title_thickness = 0;
 window->custom_border_edge_thickness = 0;
}

internal void
OS_WindowPushCustomTitleBar(OS_Handle handle, F32 thickness)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 window->custom_border_title_thickness = thickness;
}

internal void
OS_WindowPushCustomEdges(OS_Handle handle, F32 thickness)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 window->custom_border_edge_thickness = thickness;
}

internal void
OS_WindowPushCustomTitleBarClientArea(OS_Handle handle, Rng2F32 rect)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 OS_W32_TitleBarClientArea *area = PushArray(window->paint_arena, OS_W32_TitleBarClientArea, 1);
 if(area != 0)
 {
  area->rect = rect;
  QueuePush(window->first_title_bar_client_area, window->last_title_bar_client_area, area);
 }
}

//- rjf: minimizing/maximizing

internal B32
OS_WindowIsMaximized(OS_Handle handle)
{
 B32 result = 0;
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window)
 {
  result = !!(IsZoomed(window->hwnd));
 }
 return result;
}

internal void
OS_WindowMinimize(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 ShowWindow(window->hwnd, SW_MINIMIZE);
}

internal void
OS_WindowMaximize(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 ShowWindow(window->hwnd, SW_MAXIMIZE);
}

internal void
OS_WindowRestore(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 ShowWindow(window->hwnd, SW_RESTORE);
}

//- rjf: focusing

internal B32
OS_WindowIsFocused(OS_Handle handle)
{
 B32 result = 0;
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window != 0)
 {
  result = GetForegroundWindow() == window->hwnd;
 }
 return result;
}

//- rjf: fullscreen

internal B32
OS_WindowIsFullscreen(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 HWND hwnd = window->hwnd;
 DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
 B32 is_fullscreen = 1;
 if(!(window_style & WS_OVERLAPPEDWINDOW))
 {
  is_fullscreen = 0;
 }
 return is_fullscreen;
}

internal void
OS_WindowToggleFullscreen(OS_Handle handle)
{
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 HWND hwnd = window->hwnd;
 if(window->last_window_placement_initialized == 0)
 {
  window->last_window_placement_initialized = 1;
  window->last_window_placement.length = sizeof(WINDOWPLACEMENT);
 }
 
 DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
 if(window_style & WS_OVERLAPPEDWINDOW)
 {
  MONITORINFO monitor_info = { sizeof(monitor_info) };
  if(GetWindowPlacement(hwnd, &window->last_window_placement) &&
     GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY),
                    &monitor_info))
  {
   SetWindowLong(hwnd, GWL_STYLE, window_style & ~WS_OVERLAPPEDWINDOW);
   SetWindowPos(hwnd, HWND_TOP,
                monitor_info.rcMonitor.left,
                monitor_info.rcMonitor.top,
                monitor_info.rcMonitor.right -
                monitor_info.rcMonitor.left,
                monitor_info.rcMonitor.bottom -
                monitor_info.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
 }
 else
 {
  SetWindowLong(hwnd, GWL_STYLE, window_style | WS_OVERLAPPEDWINDOW);
  SetWindowPlacement(hwnd, &window->last_window_placement);
  SetWindowPos(hwnd, 0, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
               SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
 }
}

//- rjf: first paint

internal void
OS_WindowFirstPaint(OS_Handle handle)
{
 Temp scratch = ScratchBegin(0, 0);
 OS_EventList evts = {0};
 os_w32_tl_events_list = &evts;
 os_w32_tl_events_arena = scratch.arena;
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 ShowWindow(window->hwnd, SW_SHOW);
 UpdateWindow(window->hwnd);
 ScratchEnd(scratch);
}

//- rjf: accessors

internal Rng2F32
OS_RectFromWindow(OS_Handle handle)
{
 Rng2F32 rect = {0};
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window != 0)
 {
  RECT w32_rect = {0};
  if(GetWindowRect(window->hwnd, &w32_rect))
  {
   rect.x0 = (F32)w32_rect.left;
   rect.y0 = (F32)w32_rect.top;
   rect.x1 = (F32)w32_rect.right;
   rect.y1 = (F32)w32_rect.bottom;
  }
 }
 return rect;
}

internal Rng2F32
OS_ClientRectFromWindow(OS_Handle handle)
{
 Rng2F32 rect = {0};
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window != 0)
 {
  RECT w32_rect = {0};
  if(GetClientRect(window->hwnd, &w32_rect))
  {
   rect.x0 = (F32)w32_rect.left;
   rect.y0 = (F32)w32_rect.top;
   rect.x1 = (F32)w32_rect.right;
   rect.y1 = (F32)w32_rect.bottom;
  }
 }
 return rect;
}

internal F32
OS_DPIFromWindow(OS_Handle handle)
{
 F32 result = 96.f;
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window != 0)
 {
  HWND wnd = window->hwnd;
  HDC dc = window->hdc;
  F32 dpi;
  if(w32_GetDpiForWindow == 0)
  {
   F32 x = (F32)GetDeviceCaps(dc, LOGPIXELSX);
   F32 y = (F32)GetDeviceCaps(dc, LOGPIXELSY);
   dpi = x;
  }
  else
  {
   dpi = w32_GetDpiForWindow(wnd);
  }
  result = dpi;
 }
 return result;
}

internal Vec2F32
OS_MouseFromWindow(OS_Handle handle)
{
 Vec2F32 result = V2F32(-100, -100);
 OS_W32_Window *window = OS_W32_WindowFromHandle(handle);
 if(window != 0)
 {
  POINT point;
  if(GetCursorPos(&point))
  {
   if(ScreenToClient(window->hwnd, &point))
   {
    result = V2F32(point.x, point.y);
   }
  }
 }
 return result;
}

////////////////////////////////
//~ rjf: @os_per_backend Events

internal OS_Modifiers
OS_GetModifiers(void)
{
 OS_Modifiers modifiers = OS_W32_GetModifiers();
 return modifiers;
}

internal OS_EventList
OS_GetEvents(Arena *arena)
{
 ProfBeginFunction();
 OS_EventList list = {0};
 os_w32_tl_events_arena = arena;
 os_w32_tl_events_list = &list;
 for(MSG message; PeekMessage(&message, 0, 0, 0, PM_REMOVE);)
 {
  TranslateMessage(&message);
  DispatchMessage(&message);
 }
 os_w32_tl_events_arena = 0;
 os_w32_tl_events_list = 0;
 ProfEnd();
 return list;
}

internal void
OS_EatEvent(OS_EventList *events, OS_Event *event)
{
 DLLRemove(events->first, events->last, event);
 events->count -= 1;
 event->kind = OS_EventKind_Null;
}

////////////////////////////////
//~ rjf: @os_per_backend Cursors

internal void
OS_SetCursor(OS_CursorKind kind)
{
 os_w32_gfx_state->cursor_kind = kind;
}

////////////////////////////////
//~ rjf: @os_per_backend Clipboard

internal void
OS_SetClipboardText(String8 string)
{
 if(OpenClipboard(0))
 {
  EmptyClipboard();
  HANDLE string_copy_handle = root_globalAlloc(GMEM_MOVEABLE, string.size+1);
  if(string_copy_handle)
  {
   U8 *copy_buffer = (U8 *)root_globalLock(string_copy_handle);
   MemoryCopy(copy_buffer, string.str, string.size);
   copy_buffer[string.size] = 0;
   root_globalUnlock(string_copy_handle);
   SetClipboardData(CF_TEXT, string_copy_handle);
  }
  CloseClipboard();
 }
}

internal String8
OS_GetClipboardText(Arena *arena)
{
 String8 result = {0};
 if(IsClipboardFormatAvailable(CF_TEXT) &&
    OpenClipboard(0))
 {
  HANDLE data_handle = GetClipboardData(CF_TEXT);
  if(data_handle)
  {
   U8 *buffer = (U8 *)root_globalLock(data_handle);
   if(buffer)
   {
    U64 size = CalculateCStringLength((char *)buffer);
    result = PushStr8Copy(arena, Str8(buffer, size));
    root_globalUnlock(data_handle);
   }
  }
  CloseClipboard();
 }
 return result;
}
