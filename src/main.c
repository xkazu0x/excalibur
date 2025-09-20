#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define internal static
#define global   static
#define local    static

#define false 0
#define true 1

#include <stdint.h>
#include <stddef.h>
typedef size_t   uxx;
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

#define swap_t(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define sign_t(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define abs_t(T, x) (sign_t(T, x)*(x))

typedef struct {
    f32 x, y;
} Vector2;

internal Vector2
make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };
    return(result);
}

///////////////////////////////
// NOTE: Draw functions

internal void
clear(u32 *buffer, u32 width, u32 height, u32 color) {
    for (u32 i = 0; i < width*height; ++i) {
        buffer[i] = color;
    }
}

internal void
draw_fill_rect(u32 *buffer, u32 width, u32 height,
               s32 x0, s32 y0, s32 x1, s32 y1,
               u32 color) {
    if (x0 > x1) swap_t(s32, x0, x1);
    if (y0 > y1) swap_t(s32, y0, y1);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (s32)width) x1 = width;
    if (y1 > (s32)height) y1 = height;
    for (s32 y = y0; y < y1; ++y) {
        for (s32 x = x0; x < x1; ++x) {
            buffer[y*width + x] = color;
        }
    }
}

internal void
draw_fill_circle(u32 *buffer, u32 width, u32 height,
                 s32 cx, s32 cy, s32 r, 
                 u32 color) {
    r = abs_t(s32, r);
    s32 x0 = cx - r; 
    s32 y0 = cy - r;
    s32 x1 = cx + r;
    s32 y1 = cy + r;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (s32)width) x1 = width;
    if (y1 > (s32)height) y1 = height;
    for (s32 y = y0; y < y1; ++y) {
        for (s32 x = x0; x < x1; ++x) {
            s32 dx = x - cx;
            s32 dy = y - cy;
            if (dx*dx + dy*dy < r*r) {
                buffer[y*width + x] = color; 
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
// WARN: Somehow the colors are being draw at the wrong points compared
// to the other function
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

                        u32 r = beta*r0 + gamma*r1 + alpha*r2;
                        u32 g = beta*g0 + gamma*g1 + alpha*g2;
                        u32 b = beta*b0 + gamma*b1 + alpha*b2;

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
fill_rect_test(u32 *buffer, u32 width, u32 height) {
    s32 x, y, size;
    size = height/4;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Check buffer overflow on corners
    {
        x = -size/2;
        y = -size/2;
        draw_fill_rect(buffer, width, height, x, y, x + size, y + size, 0xff8033);
        x = width - size/2;
        y = -size/2;
        draw_fill_rect(buffer, width, height, x, y, x + size, y + size, 0xff8033);
        x = width - size/2;
        y = height - size/2;
        draw_fill_rect(buffer, width, height, x, y, x + size, y + size, 0xff8033);
        x = -size/2;
        y = height - size/2;
        draw_fill_rect(buffer, width, height, x, y, x + size, y + size, 0xff8033);
    }

    // NOTE: Check Bottom-Right to Top-Left draw order
    {
        x = -size/2;
        y = (height - size)/2;
        draw_fill_rect(buffer, width, height, x + size, y + size, x, y, 0xff8033);
        x = width - size/2;
        y = (height - size)/2;
        draw_fill_rect(buffer, width, height, x + size, y + size, x, y, 0xff8033);
        x = (width - size)/2;
        y = -size/2;
        draw_fill_rect(buffer, width, height, x + size, y + size, x, y, 0xff8033);
        x = (width - size)/2;
        y = height - size/2;
        draw_fill_rect(buffer, width, height, x + size, y + size, x, y,  0xff8033);
    }
}

internal void
fill_circle_test(u32 *buffer, u32 width, u32 height) {
    s32 cx, cy, r; 
    r = height/8;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Check buffer overflow on corners
    {
        cx = -16;
        cy = -16;
        draw_fill_circle(buffer, width, height, cx, cy, r, 0xff8033);
        cx = width;
        cy = -16;
        draw_fill_circle(buffer, width, height, cx, cy, r, 0xff8033);
        cx = width;
        cy = height;
        draw_fill_circle(buffer, width, height, cx, cy, r, 0xff8033);
        cx = -16;
        cy = height;
        draw_fill_circle(buffer, width, height, cx, cy, r, 0xff8033);
    }

    // NOTE: Check negative radius
    {
        r = -(height/4);
        cx = width/2;
        cy = height/2;
        draw_fill_circle(buffer, width, height, cx, cy, r, 0xff8033);
    }
}

internal void
line_test(u32 *buffer, u32 width, u32 height) {
    s32 x0, y0, x1, y1;
    clear(buffer, width, height, 0x334c4c);
    // NOTE: Check draw order
    {
        // NOTE: Top-Left - Bottom-Right
        x0 = 0;
        y0 = 0;
        x1 = width/2;
        y1 = height;
        draw_line(buffer, width, height,
                  x0, y0, x1, y1, 0xff8033);
        // NOTE: Bottom-Left - Top-Right
        x0 = 0;
        y0 = height;
        x1 = width/2;
        y1 = 0;
        draw_line(buffer, width, height,
                  x0, y0, x1, y1, 0xff8033);
        // NOTE: Top-Right - Bottom-Left
        x0 = width;
        y0 = 0;
        x1 = width - width/2;
        y1 = height;
        draw_line(buffer, width, height,
                  x0, y0, x1, y1, 0xff8033);
        // NOTE: Bottom-Right - Top-Left
        x0 = width;
        y0 = height;
        x1 = width - width/2;
        y1 = 0;
        draw_line(buffer, width, height,
                  x0, y0, x1, y1, 0xff8033);
    }
}

internal void
project_to_screen(u32 screen_width, u32 screen_height, 
                  f32 x, f32 y, f32 *sx, f32 *sy) {
    f32 width = (f32)screen_width;
    f32 height = (f32)screen_height;
    f32 aspect_ratio = height/width;
    *sx = (x*aspect_ratio + 1.0f)*width/2;
    *sy = (-y + 1.0f)*(f32)height/2;
}

internal void
fill_triangle_test(u32 *buffer, u32 width, u32 height) {
    Vector2 w0, w1, w2, w3;
    Vector2 p0, p1, p2, p3;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Clockwise
    w0 = make_vector2(-0.5f, -0.5f);
    w1 = make_vector2(-0.5f,  0.5f);
    w2 = make_vector2( 0.5f,  0.5f);
    w3 = make_vector2( 0.5f, -0.5f);

    project_to_screen(width, height, w1.x, w1.y, &p1.x, &p1.y);
    project_to_screen(width, height, w0.x, w0.y, &p0.x, &p0.y);
    project_to_screen(width, height, w2.x, w2.y, &p2.x, &p2.y);
    project_to_screen(width, height, w3.x, w3.y, &p3.x, &p3.y);

    draw_fill_triangle(buffer, width, height,
                       p0.x, p0.y, 0xFF, 0x00, 0x00,
                       p1.x, p1.y, 0x00, 0xFF, 0x00,
                       p2.x, p2.y, 0x00, 0x00, 0xFF);
    draw_fill_triangle(buffer, width, height,
                       p2.x, p2.y, 0xFF, 0x00, 0x00,
                       p3.x, p3.y, 0x00, 0xFF, 0x00,
                       p0.x, p0.y, 0x00, 0x00, 0xFF);
}

internal void
draw_test(u32 *buffer, u32 width, u32 height) {
    // fill_rect_test(buffer, width, height);
    // fill_circle_test(buffer, width, height);
    // line_test(buffer, width, height);
    fill_triangle_test(buffer, width, height); 
}

#define BYTES_PER_PIXEL 4
#define BITS_PER_PIXEL 32

typedef struct {
    u32 width;
    u32 height;
    u32 pitch;
    uxx size;
    u32 *mem;
} Back_Buffer;

internal Back_Buffer
alloc_back_buffer(u32 width, u32 height) {
    Back_Buffer result = {0};
    result.width = width;
    result.height = height;
    result.pitch = result.width*BYTES_PER_PIXEL;
    result.size = result.pitch*result.height;
    result.mem = malloc(result.size);
    return(result);
}

int
main(void) {
    u32 window_width = 800;
    u32 window_height = 600;
    Back_Buffer back_buffer = alloc_back_buffer(window_width, window_height); 
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
                                     (char *)back_buffer.mem, back_buffer.width, back_buffer.height, 
                                     BITS_PER_PIXEL, back_buffer.pitch);

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
                        back_buffer = alloc_back_buffer(window_width, window_height);
                        image = XCreateImage(display, attributes.visual, attributes.depth, ZPixmap, 0, 
                                             (char *)back_buffer.mem, back_buffer.width, back_buffer.height, 
                                             BITS_PER_PIXEL, back_buffer.pitch);
                    } break;
                }
            }

            draw_test(back_buffer.mem, back_buffer.width, back_buffer.height);
            XPutImage(display, window, context, image, 0, 0, 0, 0, window_width, window_height);
        }
    }
    return(0);
}
