#ifndef OS_H
#define OS_H

#define BYTES_PER_PIXEL 4
#define BITS_PER_PIXEL 32

typedef struct {
    u32 width;
    u32 height;
    u32 pitch;
    uxx size;
    u32 *buffer;
} OS_Bitmap;

typedef enum {
    OS_EVENT_NULL,
    OS_EVENT_QUIT,
    OS_EVENT_WINDOW_RESIZED,
} OS_Event_Type;

typedef struct {
    OS_Event_Type type;
    u32 width;
    u32 height;
} OS_Event;

global OS_Bitmap bitmap;
global OS_Event *events;

internal void os_create_window(u32 width, u32 height, char *title);
internal void os_create_window_bitmap(u32 width, u32 height);
internal void os_resize_window_bitmap(u32 width, u32 height);
internal void os_window_present_bitmap(u32 width, u32 height);
internal void os_update_window_events(void);

#if OS_WINDOWS
# include "os_win32.c"
#elif OS_LINUX
# include "os_linux.c"
#else
# error this operating system is not supported
#endif

#endif // OS_H
