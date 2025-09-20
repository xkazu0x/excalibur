#ifndef OS_LINUX_C
#define OS_LINUX_C

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
    Display *display;
    Window window;
    GC context;
    Atom wm_delete_window;
    XImage *image;
} OS_Linux_State;

global OS_Linux_State linux_state;

internal void
os_create_window(u32 width, u32 height, char *title) {
    linux_state.display = XOpenDisplay(0);
    Window root = XDefaultRootWindow(linux_state.display);
    linux_state.window = XCreateSimpleWindow(linux_state.display, root, 0, 0, width, height, 0, 0, 0);
    linux_state.context = XCreateGC(linux_state.display, linux_state.window, 0, 0);

    linux_state.wm_delete_window = XInternAtom(linux_state.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(linux_state.display, linux_state.window, &linux_state.wm_delete_window, 1);

    u32 event_masks = StructureNotifyMask;
    XSelectInput(linux_state.display, linux_state.window, event_masks);

    XStoreName(linux_state.display, linux_state.window, title);
    XMapWindow(linux_state.display, linux_state.window);
    XFlush(linux_state.display);
}

#define BYTES_PER_PIXEL 4
#define BITS_PER_PIXEL 32

typedef struct {
    u32 width;
    u32 height;
    u32 pitch;
    uxx size;
    u32 *buffer;
} OS_Bitmap;

global OS_Bitmap bitmap;

internal void 
os_create_window_bitmap(u32 width, u32 height) {
    bitmap.width = width;
    bitmap.height = height;
    bitmap.pitch = bitmap.width*BYTES_PER_PIXEL;
    bitmap.size = bitmap.pitch*bitmap.height;
    bitmap.buffer = malloc(bitmap.size);
    XWindowAttributes attributes = {0};
    XGetWindowAttributes(linux_state.display, linux_state.window, &attributes);
    linux_state.image = XCreateImage(linux_state.display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                     (char *)bitmap.buffer, bitmap.width, bitmap.height, BITS_PER_PIXEL, bitmap.pitch);
}

internal void
os_resize_window_bitmap(u32 width, u32 height) {
    XDestroyImage(linux_state.image);
    os_create_window_bitmap(width, height);
}

internal void
os_window_present_bitmap(u32 width, u32 height) {
    XPutImage(linux_state.display, linux_state.window, linux_state.context, linux_state.image, 0, 0, 0, 0, width, height);
}

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

global OS_Event *events;

internal void
os_update_window_events(void) {
    while (XPending(linux_state.display)) {
        XEvent xe = {0};
        XNextEvent(linux_state.display, &xe);
        switch (xe.type) {
            case ClientMessage: {
                XClientMessageEvent *e = (XClientMessageEvent *)&xe;
                if ((Atom)e->data.l[0] == linux_state.wm_delete_window) {
                    OS_Event event = {
                        .type = OS_EVENT_QUIT,
                    };
                    buf_push(events, event);
                }
            } break;

            case ConfigureNotify: {
                XConfigureEvent *e = (XConfigureEvent *)&xe;
                OS_Event event = {
                    .type   = OS_EVENT_WINDOW_RESIZED,
                    .width  = e->width,
                    .height = e->height,
                };
                buf_push(events, event);
            } break;

            case FocusIn:
            case FocusOut: {
                XFocusChangeEvent *e = (XFocusChangeEvent *)&xe;
                if (e->type == FocusIn) XAutoRepeatOff(linux_state.display);
                if (e->type == FocusOut) XAutoRepeatOn(linux_state.display);
            } break; 
        }
    }
}

#endif // OS_LINUX_C
