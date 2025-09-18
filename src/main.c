#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define internal static
#define global static
#define local static

#define false 0
#define true 1

#include <stdint.h>
#include <stddef.h>
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s8      b8;
typedef s16     b16;
typedef s32     b32;
typedef s64     b64;
typedef float   f32;
typedef double  f64;

#define BYTES_PER_PIXEL 4
#define BITS_PER_PIXEL 32

int
main(void) {
    s32 window_width = 800;
    s32 window_height = 800;

    u32 back_buffer_width = window_width;
    u32 back_buffer_height = window_height;
    u32 back_buffer_pitch = back_buffer_width*BYTES_PER_PIXEL;
    size_t back_buffer_size = back_buffer_pitch*back_buffer_height;
    u8 *back_buffer_memory = malloc(back_buffer_size);

    Display *display = XOpenDisplay(0);
    if (display) {
        Window root = XDefaultRootWindow(display);
        Window window = XCreateSimpleWindow(display, root, 0, 0, window_width, window_height, 0, 0, 0xFF00FF);

        Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &wm_delete_window, 1);

        u32 event_masks = StructureNotifyMask;
        XSelectInput(display, window, event_masks);
        
        XStoreName(display, window, "EXCALIBUR");
        XMapWindow(display, window);
        XSync(display, false);

        GC context = XCreateGC(display, window, 0, 0);

        XWindowAttributes attributes = {0};
        XGetWindowAttributes(display, window, &attributes);
        XImage *image = XCreateImage(display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                     (char *)back_buffer_memory, back_buffer_width, back_buffer_height, 
                                     BITS_PER_PIXEL, back_buffer_pitch);

        b32 quit = false;
        while (!quit) {
            while (XPending(display)) {
                XEvent base_event = {0};
                XNextEvent(display, &base_event);
                switch (base_event.type) {
                    case ClientMessage: {
                        XClientMessageEvent *event = (XClientMessageEvent *)&base_event;
                        if ((Atom)event->data.l[0] == wm_delete_window) {
                            quit = true;
                        }
                    } break;

                    case FocusIn:
                    case FocusOut: {
                        XFocusChangeEvent *event = (XFocusChangeEvent *)&base_event;
                        if (event->type == FocusIn) XAutoRepeatOff(display);
                        if (event->type == FocusOut) XAutoRepeatOn(display);
                    } break; 

                    case ConfigureNotify: {
                        XConfigureEvent *event = (XConfigureEvent *)&base_event;
                        window_width = event->width;
                        window_height = event->height;
                        XDestroyImage(image);
                        back_buffer_width = window_width;
                        back_buffer_height = window_height;
                        back_buffer_pitch = back_buffer_width*BYTES_PER_PIXEL;
                        back_buffer_size = back_buffer_pitch*back_buffer_height;
                        back_buffer_memory = malloc(back_buffer_size);
                        image = XCreateImage(display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                             (char *)back_buffer_memory, back_buffer_width, back_buffer_height, 
                                             BITS_PER_PIXEL, back_buffer_pitch);
                    } break;
                }
            }

            u8 *row = back_buffer_memory;
            for (u32 y = 0; y < back_buffer_height; ++y) {
                u32 *pixel = (u32 *)row;
                for (u32 x = 0; x < back_buffer_width; ++x) {
                    *pixel++ = (y << 8) | (x << 0);
                }
                row += back_buffer_pitch;
            }

            XPutImage(display, window, context, image, 0, 0, 0, 0, window_width, window_height);
        }
    }
    return(0);
}
