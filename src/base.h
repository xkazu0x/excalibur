#ifndef BASE_H
#define BASE_H

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

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

#define swap_t(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define sign_t(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define abs_t(T, x) (sign_t(T, x)*(x))

////////////////////////
// NOTE: Stretchy Buffer

#include <stdlib.h>
#include <assert.h>

typedef struct {
    uxx len;
    uxx cap;
    u8 mem[0];
} Buffer_Header;

#define buf__hdr(b) ((Buffer_Header *)((u8 *)(b) - offsetof(Buffer_Header, mem)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit((b), 1), (b)[buf__hdr(b)->len++] = (x))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = 0) : 0)

internal void *
buf__grow(const void *buf, uxx new_len, uxx elem_size) {
    uxx new_cap = MAX(1 + 2*buf_cap(buf), new_len);
    assert(new_len <= new_cap);
    uxx new_size = offsetof(Buffer_Header, mem) + new_cap*elem_size;
    Buffer_Header *new_hdr;
    if (buf) {
        new_hdr = realloc(buf__hdr(buf), new_size);
    } else {
        new_hdr = malloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return(new_hdr->mem);
}

///////////////
// NOTE: Vector

typedef struct {
    f32 x, y;
} Vector2;

internal Vector2
make_vector2(f32 x, f32 y) {
    Vector2 result = { x, y };
    return(result);
}

#endif // BASE_H
