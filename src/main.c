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
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef s8       b8;
typedef s16      b16;
typedef s32      b32;
typedef s64      b64;
typedef float    f32;
typedef double   f64;

#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))

#define swap_t(type, a, b) do { type t = a; a = b; b = t;} while (0)
#define sign_t(type, x) ((type)((x) > 0) - (type)((x) < 0))
#define abs_t(type, x) (sign_t(type, x)*(x))

typedef struct {
    f32 x, y;
} Vector2;

internal Vector2
make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };
    return(result);
}

internal void
draw_clear(u32 *buffer, u32 width, u32 height, u32 color) {
    for (u32 i = 0; i < width*height; ++i) {
        buffer[i] = color;
    }
}

internal void
draw_fill_rect(u32 *buffer, u32 width, u32 height,
               s32 x0, s32 y0, s32 x1, s32 y1,
               u32 color) {
    for (s32 y = y0; y <= y1; ++y) {
        if (y >= 0 && y < (s32)height) {
            for (s32 x = x0; x <= x1; ++x) {
                if (x >= 0 && x < (s32)width) {
                    buffer[y*width + x] = color;
                }
            }
        }
    }
}

internal void
draw_fill_circle(u32 *buffer, u32 width, u32 height,
                 s32 cx, s32 cy, s32 r, 
                 u32 color) {
    s32 x0 = cx - r; 
    s32 y0 = cy - r;
    s32 x1 = cx + r;
    s32 y1 = cy + r;
    for (s32 y = y0; y <= y1; ++y) {
        if (y >= 0 && y < (s32)height) {
            for (s32 x = x0; x <= x1; ++x) {
                if (x >= 0 && x < (s32)width) {
                    s32 dx = x - cx;
                    s32 dy = y - cy;
                    if (dx*dx + dy*dy <= r*r) {
                        buffer[y*width + x] = color; 
                    }
                }
            }
        }
    }
}

internal void
draw_line(u32 *buffer, u32 width, u32 height,
          s32 x0, s32 y0, s32 x1, s32 y1,
          u32 color) {
    if (abs_t(s32, x1 - x0) > abs_t(s32, y1 - y0)) {
        if (x0 > x1) {
            swap_t(s32, x0, x1);
            swap_t(s32, y0, y1);
        }
        s32 dx = x1 - x0;
        s32 dy = y1 - y0;
        s32 dir = (dy < 0) ? -1 : 1;
        dy *= dir;
        if (dx != 0) {
            s32 y = y0;
            s32 d = 2*dy - dx;
            for (s32 x = x0; x <= x1; ++x) {
                if ((y >= 0 && y < (s32)height) &&
                    (x >= 0 && x < (s32)width)) {
                    buffer[y*width + x] = color; 
                }
                if (d >= 0) {
                    y += dir;
                    d = d - 2*dx;
                }
                d = d + 2*dy;
            }
        }
    } else {
        if (y0 > y1) {
            swap_t(s32, x0, x1);
            swap_t(s32, y0, y1);
        }
        s32 dx = x1 - x0;
        s32 dy = y1 - y0;
        s32 dir = (dx < 0) ? -1 : 1;
        dx *= dir;
        if (dy != 0) {
            s32 x = x0;
            s32 d = 2*dx - dy;
            for (s32 y = y0; y <= y1; ++y) {
                if ((y >= 0 && y < (s32)height) &&
                    (x >= 0 && x < (s32)width)) {
                    buffer[y*width + x] = color; 
                }
                if (d >= 0) {
                    x += dir;
                    d = d - 2*dy;
                }
                d = d + 2*dx;
            }
        }
    }
}

internal void
draw_fill_triangle0(u32 *buffer, u32 width, u32 height,
                    s32 x0, s32 y0, u32 r0, u32 g0, u32 b0, 
                    s32 x1, s32 y1, u32 r1, u32 g1, u32 b1, 
                    s32 x2, s32 y2, u32 r2, u32 g2, u32 b2) {
    s32 x01 = x1 - x0;
    s32 y01 = y1 - y0;

    s32 x12 = x2 - x1;
    s32 y12 = y2 - y1;

    s32 x20 = x0 - x2;
    s32 y20 = y0 - y2;

    s32 x02 = x2 - x0;
    s32 y02 = y2 - y0;
    s32 area = x01*y02 - y01*x02;

    s32 bias0 = ((y01 == 0 && x01 > 0) || (y01 < 0)) ? 0 : -1;
    s32 bias1 = ((y12 == 0 && x12 > 0) || (y12 < 0)) ? 0 : -1;
    s32 bias2 = ((y20 == 0 && x20 > 0) || (y20 < 0)) ? 0 : -1;

    s32 x_min = min(min(x0, x1), x2);
    s32 y_min = min(min(y0, y1), y2);
    s32 x_max = max(max(x0, x1), x2);
    s32 y_max = max(max(y0, y1), y2);

    for (s32 y = y_min; y < y_max; ++y) {
        if (y >= 0 && y < (s32)height) {
            for (s32 x = x_min; x < x_max; ++x) {
                if (x >= 0 && x < (s32)width) {
                    s32 dx0 = x - x0;
                    s32 dy0 = y - y0;
                    s32 w0 = x01*dy0 - y01*dx0 + bias0;

                    s32 dx1 = x - x1;
                    s32 dy1 = y - y1;
                    s32 w1 = x12*dy1 - y12*dx1 + bias1;

                    s32 dx2 = x - x2;
                    s32 dy2 = y - y2;
                    s32 w2 = x20*dy2 - y20*dx2 + bias2;

                    if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                        f32 alpha = (f32)w0/(f32)area;
                        f32 beta  = (f32)w1/(f32)area;
                        f32 gamma = (f32)w2/(f32)area;
                        
                        u32 r = alpha*r0 + beta*r1 + gamma*r2;
                        u32 g = alpha*g0 + beta*g1 + gamma*g2;
                        u32 b = alpha*b0 + beta*b1 + gamma*b2;
                        
                        u32 color = (r << 16) | (g << 8) | (b << 0);
                        buffer[y*width + x] = color;
                    }
                }
            }
        }
    }
}

// NOTE: This one avoids computing the cross product on every pixel
internal void
draw_fill_triangle(u32 *buffer, u32 width, u32 height,
                   s32 x0, s32 y0, u32 r0, u32 g0, u32 b0, 
                   s32 x1, s32 y1, u32 r1, u32 g1, u32 b1, 
                   s32 x2, s32 y2, u32 r2, u32 g2, u32 b2) {
    s32 x01 = x1 - x0;
    s32 y01 = y1 - y0;

    s32 x12 = x2 - x1;
    s32 y12 = y2 - y1;

    s32 x20 = x0 - x2;
    s32 y20 = y0 - y2;

    s32 x02 = x2 - x0;
    s32 y02 = y2 - y0;
    s32 area = x01*y02 - y01*x02;

    s32 bias0 = ((y01 == 0 && x01 > 0) || (y01 < 0)) ? 0 : -1;
    s32 bias1 = ((y12 == 0 && x12 > 0) || (y12 < 0)) ? 0 : -1;
    s32 bias2 = ((y20 == 0 && x20 > 0) || (y20 < 0)) ? 0 : -1;

    s32 x_min = min(min(x0, x1), x2);
    s32 y_min = min(min(y0, y1), y2);
    s32 x_max = max(max(x0, x1), x2);
    s32 y_max = max(max(y0, y1), y2);

    s32 dx0 = x_min - x0;
    s32 dy0 = y_min - y0;
    s32 w0_row = x01*dy0 - y01*dx0 + bias0;

    s32 dx1 = x_min - x1;
    s32 dy1 = y_min - y1;
    s32 w1_row = x12*dy1 - y12*dx1 + bias1;

    s32 dx2 = x_min - x2;
    s32 dy2 = y_min - y2;
    s32 w2_row = x20*dy2 - y20*dx2 + bias2;
    
    // TODO: Check if this is correct
    // {
    s32 dw0_col = y0 - y1;
    s32 dw1_col = y1 - y2;
    s32 dw2_col = y2 - y0;

    s32 dw0_row = x1 - x0;
    s32 dw1_row = x2 - x1;
    s32 dw2_row = x0 - x2;
    // }

    for (s32 y = y_min; y < y_max; ++y) {
        f32 w0 = w0_row;
        f32 w1 = w1_row;
        f32 w2 = w2_row;
        if (y >= 0 && y < (s32)height) {
            for (s32 x = x_min; x < x_max; ++x) {
                if (x >= 0 && x < (s32)width) {
                    if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                        f32 alpha = (f32)w0/(f32)area;
                        f32 beta  = (f32)w1/(f32)area;
                        f32 gamma = (f32)w2/(f32)area;
                        
                        u32 r = alpha*r0 + beta*r1 + gamma*r2;
                        u32 g = alpha*g0 + beta*g1 + gamma*g2;
                        u32 b = alpha*b0 + beta*b1 + gamma*b2;
                        
                        u32 color = (r << 16) | (g << 8) | (b << 0);
                        buffer[y*width + x] = color;
                    }
                }
                w0 += dw0_col;
                w1 += dw1_col;
                w2 += dw2_col;
            }
        }
        w0_row += dw0_row;
        w1_row += dw1_row;
        w2_row += dw2_row;
    }
}

internal void
project_world_to_screen(u32 window_width, u32 window_height, 
                        f32 world_x, f32 world_y, 
                        f32 *screen_x, f32 *screen_y) {
    f32 aspect_ratio = (f32)window_height/(f32)window_width;
    *screen_x = (world_x*aspect_ratio + 1.0f)*(f32)window_width/2;
    *screen_y = (-world_y + 1.0f)*(f32)window_height/2;
}

#define BYTES_PER_PIXEL 4
#define BITS_PER_PIXEL 32

int
main(void) {
    s32 window_width = 800;
    s32 window_height = 600;

    u32 back_buffer_width = window_width;
    u32 back_buffer_height = window_height;
    u32 back_buffer_pitch = back_buffer_width*BYTES_PER_PIXEL;
    u64 back_buffer_size = back_buffer_pitch*back_buffer_height;
    u32 *back_buffer = malloc(back_buffer_size);

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
        XFlush(display);

        GC context = XCreateGC(display, window, 0, 0);

        XWindowAttributes attributes = {0};
        XGetWindowAttributes(display, window, &attributes);
        XImage *image = XCreateImage(display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                     (char *)back_buffer, back_buffer_width, back_buffer_height, 
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
                        back_buffer = malloc(back_buffer_size);
                        image = XCreateImage(display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                             (char *)back_buffer, back_buffer_width, back_buffer_height, 
                                             BITS_PER_PIXEL, back_buffer_pitch);
                    } break;
                }
            }

            draw_clear(back_buffer, back_buffer_width, back_buffer_height, 0);

            Vector2 w0, w1, w2;
            Vector2 p0, p1, p2;
            
            // NOTE: Clockwise
            w0 = make_vector2( 0.0f,  0.5f);
            w1 = make_vector2( 0.5f, -0.5f);
            w2 = make_vector2(-0.5f, -0.5f);

            project_world_to_screen(window_width, window_height, w0.x, w0.y, &p0.x, &p0.y);
            project_world_to_screen(window_width, window_height, w1.x, w1.y, &p1.x, &p1.y);
            project_world_to_screen(window_width, window_height, w2.x, w2.y, &p2.x, &p2.y);

            draw_fill_triangle(back_buffer, back_buffer_width, back_buffer_height,
                               p0.x, p0.y, 0xFF, 0x00, 0x00,
                               p1.x, p1.y, 0x00, 0xFF, 0x00,
                               p2.x, p2.y, 0x00, 0x00, 0xFF);

            XPutImage(display, window, context, image, 0, 0, 0, 0, window_width, window_height);
        }
    }
    return(0);
}
