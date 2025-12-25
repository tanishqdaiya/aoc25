#ifndef TDLIB_H
#define TDLIB_H

#ifndef TD_LIBDEF
#    ifdef TDLIB_STATIC
#        define TD_LIBDEF static
#    else
#        define TD_LIBDEF extern
#    endif
#endif

#if defined __clang__
#    define COMPILER_CLANG
#elif defined _MSC_VER
#    define COMPILER_MS
#elif defined __GNUC__
#    define COMPILER_GNU
#endif

#if defined _WIN32
#    define PLATFORM_WIN
#elif defined __linux__ || defined __gnu_linux__
#    define PLATFORM_LINUX
#elif defined __APPLE__ && defined __MACH__
#    define PLATFORM_MACOS
#elif defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__
#    define PLATFORM_BSD
#else
#    error "fatal: unsupported platform"
#endif

#if defined PLATFORM_LINUX || defined PLATFORM_MACOS || defined PLATFORM_BSD
#    define PLATFORM_POSIX
#endif

#ifdef COMPILER_MS
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i8 b8;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

#define global_variable static
#define local_persist static
#define internal static

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TD_MALLOC
#    define TD_MALLOC(sz) malloc(sz)
#endif

#ifndef TD_REALLOC
#    define TD_REALLOC(ptr, sz) realloc((ptr), (sz))
#endif

#ifndef TD_FREE
#    define TD_FREE(p) free(p)
#endif

#ifndef TD_VECINITSZ
#    define TD_VECINITSZ 1024
#endif

#ifndef TD_PANIC
#define TD_PANIC(msg)                                           \
    do {                                                        \
        fprintf(stderr, "%s\n", (msg));                         \
        exit(EXIT_FAILURE);                                     \
    } while (0)
#endif
        

/* It is a resizing function which required the vector in its right structural
   format and the required capacity. If the capacity exceeds the pre-allocated
   size of the vector, we resize. This function is unsafe and provides no
   guaranteed successful reallocation */
#define td__vec_alloc(vector, capacity)                                 \
    do {                                                                \
        if ((capacity) > (vector)->alloc) {                             \
            if ((vector)->alloc == 0) (vector)->alloc = TD_VECINITSZ;   \
            while ((capacity) > (vector)->alloc) (vector)->alloc *= 2;  \
            (vector)->data = TD_REALLOC((vector)->data,                 \
                                     (vector)->alloc * sizeof(*(vector)->data)); \
            if ((vector)->data == NULL) {                               \
                TD_PANIC("TD_REALLOC: out of memory");                  \
            }                                                           \
        }                                                               \
    } while (0)

#define td_vec_append(vector, item)                     \
    do {                                                \
        td__vec_alloc((vector), (vector)->size + 1);    \
        (vector)->data[(vector)->size++] = (item);      \
    } while (0)

#define td_vec_append_bulk(vector, items, count)                \
    do {                                                        \
        td__vec_alloc((vector), (vector)->size + count);        \
        memcpy((vector)->data + (vector)->size,                 \
               (items),                                         \
               (count)*sizeof(*(vector)->data));                \
        (vector)->size += (count);                              \
    } while (0)

/* Not null-terminated by default behavior */
typedef struct {
    char   *data;
    size_t  size, alloc;
} TD_String;

typedef struct {
    char   *data;
    size_t  size;
} TD_String_View;

#define td_string_append_cstr(string, cstr)             \
    do {                                                \
        const char *str = cstr;                         \
        size_t size = strlen(cstr);                     \
        td_vec_append_bulk((string), str, size);        \
    } while (0)

#define td_string_clear(string)                 \
    do {                                        \
        TD_FREE((string)->data);                \
        (string)->data = NULL;                  \
        (string)->size = (string)->alloc = 0;   \
    } while (0)

TD_LIBDEF TD_String_View td_string_view_from_string(TD_String *str);
TD_LIBDEF TD_String_View td_string_view_from_cstr(char *cstr);
TD_LIBDEF bool           td_string_view_equal(TD_String_View a, TD_String_View b);
TD_LIBDEF TD_String_View td_string_view_chop(TD_String_View *v, char delim);
TD_LIBDEF TD_String_View td_string_view_trim_left(TD_String_View v);
TD_LIBDEF TD_String_View td_string_view_trim_right(TD_String_View v);
TD_LIBDEF TD_String_View td_string_view_trim(TD_String_View v);

TD_LIBDEF TD_String_View td_string_slice(const TD_String *str,
                                         size_t           start,
                                         size_t           end);

TD_LIBDEF bool td_read_file_to_string(TD_String *str, FILE *fp);

#endif /* TDLIB_H */


#ifdef TDLIB_IMPLEMENTATION

#include <ctype.h>

TD_LIBDEF TD_String_View
td_string_view_from_string(TD_String *str)
{
    return (TD_String_View){ str->data, str->size };
}
    
TD_LIBDEF TD_String_View
td_string_view_from_cstr(char *cstr)
{
    return (TD_String_View){ cstr, strlen(cstr) };
}

TD_LIBDEF bool
td_string_view_equal(TD_String_View a, TD_String_View b)
{
    if (a.size != b.size) return false;
    for (size_t i = 0; i < a.size; i++)
        if (a.data[i] != b.data[i]) return false;
    return true;
}

TD_LIBDEF TD_String_View
td_string_view_chop(TD_String_View *v, char delim)
{
    size_t i = 0;
    while (i < v->size && v->data[i] != delim) i++;

    TD_String_View out = { v->data, i };

    if (i < v->size) {
        v->data += i + 1;
        v->size -= i + 1;
    } else {
        v->data += i;
        v->size = 0;
    }

    return out;
}

TD_LIBDEF TD_String_View
td_string_view_trim_left(TD_String_View v)
{
    size_t i = 0;
    while (i < v.size && isspace((unsigned char)v.data[i]))
        ++i;
    return (TD_String_View){ v.data + i, v.size - i };
}

TD_LIBDEF TD_String_View
td_string_view_trim_right(TD_String_View v)
{
    size_t end = v.size;
    while (end > 0 && isspace((unsigned char)v.data[end - 1]))
        --end;
    return (TD_String_View){ v.data, end };
}

TD_LIBDEF TD_String_View
td_string_view_trim(TD_String_View v)
{
    return td_string_view_trim_right(td_string_view_trim_left(v));
}

TD_LIBDEF TD_String_View
td_string_slice(const TD_String *str,
                size_t           start,
                size_t           end)
{
    if (start > str->size) start = str->size;
    if (end > str->size) end = str->size;
    if (end < start) end = start;

    return (TD_String_View) { str->data + start, end - start };
}

TD_LIBDEF bool
td_read_file_to_string(TD_String *str, FILE *fp)
{
    i32 file_size;
    size_t read;

    if (fseek(fp, 0, SEEK_END) != 0)
        return false;

    file_size = ftell(fp);
    if (file_size < 0)
        return false;

    rewind(fp);

    td__vec_alloc(str, (size_t)file_size);
    read = fread(str->data, 1, (size_t)file_size, fp);
    if (read != (size_t)file_size)
        return false;

    str->size = read;
    return true;
}

#endif /* TDLIB_IMPLEMENTATION */
