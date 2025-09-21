#include "base.h"
#include "os.h"

internal void
unpack_rgba32(u32 color, u8 *r, u8 *g, u8 *b, u8 *a) {
    *a = ((color >> 24) & 0xFF);
    *r = ((color >> 16) & 0xFF);
    *g = ((color >>  8) & 0xFF);
    *b = ((color >>  0) & 0xFF);
}

internal u32
pack_rgba32(u8 r, u8 g, u8 b, u8 a) {
    u32 result = ((a << 24) | (r << 16) | (g << 8) | (b << 0));
    return(result);
}

internal u32
color_blend(u32 dst, u32 src) {
    u8 r1, g1, b1, a1;
    u8 r2, g2, b2, a2; 

    unpack_rgba32(dst, &r1, &g1, &b1, &a1);
    unpack_rgba32(src, &r2, &g2, &b2, &a2);

    u8 r = r1 + (r2 - r1)*a2/255;
    u8 g = g1 + (g2 - g1)*a2/255;
    u8 b = b1 + (b2 - b1)*a2/255;
    u8 a = a1;

    u32 result = pack_rgba32(r, g, b, a);
    return(result);
}

internal void
clear(u32 *buffer, u32 width, u32 height, u32 color) {
    for (u32 i = 0; i < width*height; ++i) {
        buffer[i] = color;
    }
}

internal void
draw_rect(u32 *buffer, u32 width, u32 height,
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
            u32 *dst = buffer + y*width + x;
            *dst = color_blend(*dst, color);
        }
    }
}

internal void
draw_circle(u32 *buffer, u32 width, u32 height,
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
                u32 *dst = buffer + y*width + x;
                *dst = color_blend(*dst, color);
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
                    u32 *dst = buffer + y*width + x;
                    *dst = color_blend(*dst, color); 
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
                    u32 *dst = buffer + y*width + x;
                    *dst = color_blend(*dst, color); 
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

// TODO: OLD ONE!!!!!!!!!!!!!!
internal void
draw_triangle0(u32 *buffer, u32 width, u32 height,
               s32 x0, s32 y0, u32 c0, 
               s32 x1, s32 y1, u32 c1, 
               s32 x2, s32 y2, u32 c2) {
    s32 x01 = x1 - x0;
    s32 y01 = y1 - y0;

    s32 x12 = x2 - x1;
    s32 y12 = y2 - y1;

    s32 x20 = x0 - x2;
    s32 y20 = y0 - y2;

    s32 x02 = x2 - x0;
    s32 y02 = y2 - y0;
    s32 det = x01*y02 - y01*x02;

    s32 bias0 = ((y01 == 0 && x01 > 0) || (y01 < 0)) ? 0 : -1;
    s32 bias1 = ((y12 == 0 && x12 > 0) || (y12 < 0)) ? 0 : -1;
    s32 bias2 = ((y20 == 0 && x20 > 0) || (y20 < 0)) ? 0 : -1;

    s32 x_min = MIN(MIN(x0, x1), x2);
    s32 y_min = MIN(MIN(y0, y1), y2);
    s32 x_max = MAX(MAX(x0, x1), x2);
    s32 y_max = MAX(MAX(y0, y1), y2);

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
                        f32 alpha = (f32)w0/(f32)det;
                        f32 beta  = (f32)w1/(f32)det;
                        f32 gamma = (f32)w2/(f32)det;
                        
                        u8 r0, g0, b0, a0;
                        unpack_rgba32(c0, &r0, &g0, &b0, &a0);
                        u8 r1, g1, b1, a1;
                        unpack_rgba32(c1, &r1, &g1, &b1, &a1);
                        u8 r2, g2, b2, a2;
                        unpack_rgba32(c2, &r2, &g2, &b2, &a2);

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
draw_triangle(u32 *buffer, u32 width, u32 height,
              s32 x0, s32 y0, u32 c0, 
              s32 x1, s32 y1, u32 c1, 
              s32 x2, s32 y2, u32 c2) {
    s32 x01 = x1 - x0;
    s32 y01 = y1 - y0;

    s32 x12 = x2 - x1;
    s32 y12 = y2 - y1;

    s32 x20 = x0 - x2;
    s32 y20 = y0 - y2;

    s32 x02 = x2 - x0;
    s32 y02 = y2 - y0;
    s32 det = x01*y02 - y01*x02;

    s32 bias0 = ((y01 == 0 && x01 > 0) || (y01 < 0)) ? 0 : -1;
    s32 bias1 = ((y12 == 0 && x12 > 0) || (y12 < 0)) ? 0 : -1;
    s32 bias2 = ((y20 == 0 && x20 > 0) || (y20 < 0)) ? 0 : -1;

    s32 x_min = MIN(MIN(x0, x1), x2);
    s32 y_min = MIN(MIN(y0, y1), y2);
    s32 x_max = MAX(MAX(x0, x1), x2);
    s32 y_max = MAX(MAX(y0, y1), y2);

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
                        f32 alpha = (f32)w0/(f32)det;
                        f32 beta  = (f32)w1/(f32)det;
                        f32 gamma = (f32)w2/(f32)det;

                        u8 r0, g0, b0, a0;
                        unpack_rgba32(c0, &r0, &g0, &b0, &a0);
                        u8 r1, g1, b1, a1;
                        unpack_rgba32(c1, &r1, &g1, &b1, &a1);
                        u8 r2, g2, b2, a2;
                        unpack_rgba32(c2, &r2, &g2, &b2, &a2);

                        u32 r = beta*r0 + gamma*r1 + alpha*r2;
                        u32 g = beta*g0 + gamma*g1 + alpha*g2;
                        u32 b = beta*b0 + gamma*b1 + alpha*b2;
                        u32 a = beta*a0 + gamma*a1 + alpha*a2;
                        u32 color0 = pack_rgba32(r, g, b, a);  
                        
                        u32 *dst = buffer + y*width + x;
                        *dst = color_blend(*dst, color0);
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
draw_rect_test(u32 *buffer, u32 width, u32 height) {
    s32 x, y, size;
    size = height/4;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Check buffer overflow on corners
    {
        x = -size/2;
        y = -size/2;
        draw_rect(buffer, width, height, x, y, x + size, y + size, 0xFFFF8033);
        x = width - size/2;
        y = -size/2;
        draw_rect(buffer, width, height, x, y, x + size, y + size, 0xFFFF8033);
        x = width - size/2;
        y = height - size/2;
        draw_rect(buffer, width, height, x, y, x + size, y + size, 0xFFFF8033);
        x = -size/2;
        y = height - size/2;
        draw_rect(buffer, width, height, x, y, x + size, y + size, 0xFFFF8033);
    }

    // NOTE: Check Bottom-Right to Top-Left draw order
    {
        x = -size/2;
        y = (height - size)/2;
        draw_rect(buffer, width, height, x + size, y + size, x, y, 0xFFFF8033);
        x = width - size/2;
        y = (height - size)/2;
        draw_rect(buffer, width, height, x + size, y + size, x, y, 0xFFFF8033);
        x = (width - size)/2;
        y = -size/2;
        draw_rect(buffer, width, height, x + size, y + size, x, y, 0xFFFF8033);
        x = (width - size)/2;
        y = height - size/2;
        draw_rect(buffer, width, height, x + size, y + size, x, y,  0xFFFF8033);
    }
}

internal void
draw_circle_test(u32 *buffer, u32 width, u32 height) {
    s32 cx, cy, r; 
    r = height/8;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Check buffer overflow on corners
    {
        cx = -16;
        cy = -16;
        draw_circle(buffer, width, height, cx, cy, r, 0xFFFF8033);
        cx = width;
        cy = -16;
        draw_circle(buffer, width, height, cx, cy, r, 0xFFFF8033);
        cx = width;
        cy = height;
        draw_circle(buffer, width, height, cx, cy, r, 0xFFFF8033);
        cx = -16;
        cy = height;
        draw_circle(buffer, width, height, cx, cy, r, 0xFFFF8033);
    }

    // NOTE: Check negative radius
    {
        r = -(height/4);
        cx = width/2;
        cy = height/2;
        draw_circle(buffer, width, height, cx, cy, r, 0xFFFF8033);
    }
}

internal void
draw_line_test(u32 *buffer, u32 width, u32 height) {
    s32 x0, y0, x1, y1;
    clear(buffer, width, height, 0x334c4c);

    // NOTE: Check draw order
    {
        // NOTE: Top-Left - Bottom-Right
        x0 = 0;
        y0 = 0;
        x1 = width/2;
        y1 = height;
        draw_line(buffer, width, height, x0, y0, x1, y1, 0xff8033);

        // NOTE: Bottom-Left - Top-Right
        x0 = 0;
        y0 = height;
        x1 = width/2;
        y1 = 0;
        draw_line(buffer, width, height, x0, y0, x1, y1, 0xff8033);

        // NOTE: Top-Right - Bottom-Left
        x0 = width;
        y0 = 0;
        x1 = width - width/2;
        y1 = height;
        draw_line(buffer, width, height, x0, y0, x1, y1, 0xff8033);

        // NOTE: Bottom-Right - Top-Left
        x0 = width;
        y0 = height;
        x1 = width - width/2;
        y1 = 0;
        draw_line(buffer, width, height, x0, y0, x1, y1, 0xff8033);
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
draw_triangle_test(u32 *buffer, u32 width, u32 height) {
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

    draw_triangle(buffer, width, height,
                  p0.x, p0.y, 0xFFFF0000,
                  p1.x, p1.y, 0xFF00FF00,
                  p2.x, p2.y, 0xFF0000FF);
    draw_triangle(buffer, width, height,
                  p2.x, p2.y, 0xFFFF0000,
                  p3.x, p3.y, 0xFF00FF00,
                  p0.x, p0.y, 0xFF0000FF);
}

internal void
alpha_blending_test(u32 *buffer, u32 width, u32 height) {
    s32 x, y, r;
    clear(buffer, width, height, 0x202020);
    {
        r = height/4;
        x = width/2 + r/3;
        y = height/2 + r/3;
        draw_rect(buffer, width, height, x, y, x - r, y - r, 0xffaa2020);

        x = width/2 - r/3;
        y = height/2 - r/3;
        draw_rect(buffer, width, height, x, y, x + r, y + r, 0x5520aa20);
    
        x = width/2;
        y = height/2;
        r = height/10;
        draw_circle(buffer, width, height, x, y, r, 0x552020aa);
    
        r = height/2;
        s32 x0 = (width - r)/2;
        s32 y0 = (height + r)/2;
        s32 x1 = width/2;
        s32 y1 = (height - r)/2;
        s32 x2 = (width + r)/2;
        s32 y2 = (height + r)/2;
        draw_triangle(buffer, width, height,
                      x0, y0, 0xEEFF0000,
                      x1, y1, 0xAA00FF00,
                      x2, y2, 0x110000FF);
    }
}

internal void
draw_test(u32 *buffer, u32 width, u32 height) {
    // draw_rect_test(buffer, width, height);
    // draw_circle_test(buffer, width, height);
    // draw_line_test(buffer, width, height);
    // draw_triangle_test(buffer, width, height); 
    alpha_blending_test(buffer, width, height);
}

int
main(void) {
    u32 window_width = 800;
    u32 window_height = 600;
    char *window_title = "EXCALIBUR";
    os_create_window(window_width, window_height, window_title);
    os_create_window_bitmap(window_width, window_height);
    for (b32 quit = false; !quit;) {
        os_update_window_events();
        for (uxx event_index = 0; 
             event_index < buf_len(events);
             ++event_index) {
            OS_Event *event = events + event_index;
            switch(event->type) {
                case OS_EVENT_QUIT: {
                    quit = true;
                } break;
                case OS_EVENT_WINDOW_RESIZED: {
                    window_width = event->width;
                    window_height = event->height;
                    os_resize_window_bitmap(window_width, window_height);
                } break;
                default: {
                    invalid_path;
                } break;
            }
        }
        draw_test(bitmap.buffer, bitmap.width, bitmap.height);
        os_window_present_bitmap(window_width, window_height);
    }
}
