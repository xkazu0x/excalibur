#ifndef OS_WIN32_C
#define OS_WIN32_C

#define NO_MIN_MAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")

typedef struct {
    HWND window;
    BITMAPINFO bitmap_info;
} OS_Win32_State;

global OS_Win32_State win32_state;

internal LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

internal void
os_create_window(u32 width, u32 height, char *title) {
    s32 screen_width = GetSystemMetrics(SM_CXSCREEN);
    s32 screen_height = GetSystemMetrics(SM_CYSCREEN);

    s32 window_width = width;
    s32 window_height = height;

    s32 window_x = (screen_width - window_width)/2;
    s32 window_y = (screen_height - window_height)/2;
    
    u32 window_style = WS_OVERLAPPEDWINDOW;
    u32 window_style_ex = 0; 

    RECT window_rectangle = {
        .left = 0,
        .right = window_width,
        .top = 0,
        .bottom = window_height,
    };

    if (AdjustWindowRect(&window_rectangle, window_style, 0)) {
        window_width = window_rectangle.right - window_rectangle.left;
        window_height = window_rectangle.bottom - window_rectangle.top;
    }

    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA window_class = {
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = win32_window_proc,
        .cbClsExtra = 0,
        .hInstance = instance,
        .hIcon = LoadIcon(0, IDI_APPLICATION),
        .hCursor = LoadCursor(0, IDI_ARROW),
        .hbrBackground = CreateSolidBrush(RGB(0, 0, 0)),
        .lpszMenuName = 0,
        .lpszClassName = "excalibur_window_class",
    };

    ATOM window_atom = RegisterClassA(&window_class);
    if (window_atom) {
        win32_state.window = CreateWindowExA(window_style_ex, MAKEINTATOM(window_atom), title, 
                                             window_style, window_x, window_y, window_width, window_height,
                                             0, 0, instance, 0);
        if (win32_state.window) {
            ShowWindow(win32_state.window, SW_SHOW);
        }
    }
}

internal void
os_create_window_bitmap(u32 width, u32 height) {
    bitmap.width = width;
    bitmap.height = height;
    bitmap.pitch = bitmap.width*BYTES_PER_PIXEL;
    bitmap.size = bitmap.pitch*bitmap.height;
    bitmap.buffer = VirtualAlloc(0, bitmap.size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    win32_state.bitmap_info.bmiHeader.biSize = sizeof(win32_state.bitmap_info.bmiHeader);
    win32_state.bitmap_info.bmiHeader.biWidth = bitmap.width;
    win32_state.bitmap_info.bmiHeader.biHeight = bitmap.height;
    win32_state.bitmap_info.bmiHeader.biPlanes = 1;
    win32_state.bitmap_info.bmiHeader.biBitCount = BITS_PER_PIXEL;
    win32_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
}

internal void
os_resize_window_bitmap(u32 width, u32 height) {
    VirtualFree(bitmap.buffer, 0, MEM_RELEASE);
    os_create_window_bitmap(width, height);
}

internal void
os_window_present_bitmap(u32 width, u32 height) {
    HDC window_device = GetDC(win32_state.window);
    StretchDIBits(window_device,
                  0, 0, width, height,
                  0, 0, bitmap.width, bitmap.height,
                  bitmap.buffer, &win32_state.bitmap_info,
                  DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(win32_state.window, window_device);
}

internal void
os_update_window_events(void) {
    buf_clear(events);
    MSG msg;
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
         switch (msg.message) {
            case WM_QUIT: {
                OS_Event event = {
                    .type = OS_EVENT_QUIT,
                };
                buf_push(events, event);
            } break;
            default: {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
        }
    }
}

internal LRESULT CALLBACK
win32_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    LRESULT result = 0;
    switch (message) {
        case WM_CLOSE:
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_SIZE: {
            OS_Event event = {
                .type   = OS_EVENT_WINDOW_RESIZED,
                .width  = LOWORD(lparam),
                .height = HIWORD(lparam),
            };
            buf_push(events, event);
        } break;
        default: {
            result = DefWindowProcA(window, message, wparam, lparam);
        }
    }
    return(result);
}

#endif // OS_WIN32_C
