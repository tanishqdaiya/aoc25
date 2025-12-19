// Still in testing...
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define global_variable static
#define local_persist static
#define internal static


#define VECINITSZ 1024

typedef struct {
    char *data;
    size_t size, alloc;
} string_t;

typedef struct {
    char *data;
    size_t size;
} string_view_t;

#define vec_alloc(vector, capacity)                                     \
    do {                                                                \
        if ((capacity) > (vector)->alloc) {                             \
            if ((vector)->alloc == 0) (vector)->alloc = VECINITSZ;      \
            while ((capacity) > (vector)->alloc) (vector)->alloc *= 2;  \
            (vector)->data = realloc((vector)->data,                    \
                                     (vector)->alloc * sizeof(*(vector)->data) \
                                    );                                  \
            if ((vector)->data == NULL) {                               \
                fprintf(stderr, "realloc: can't allocate memory\n");    \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        }                                                               \
    } while (0)

#define vec_append(vector, item)                        \
    do {                                                \
        vec_alloc((vector), (vector)->size + 1);        \
        (vector)->data[(vector)->size++] = (item);      \
    } while (0)

#define vec_append_bulk(vector, items, count)                           \
    do {                                                                \
        vec_alloc((vector), (vector)->size + count);                    \
        memcpy((vector)->data + (vector)->size, (items), (count)*sizeof(*(vector)->data)); \
        (vector)->size += (count);                                      \
    } while (0)

/* Thanks to tsoding for the string_view inspiration */
#define string_append_cstr(string, cstr)        \
    do {                                        \
        const char *str = cstr;                 \
        size_t length = strlen(cstr);           \
        vec_append_bulk((string), str, length); \
    } while (0)

#define string_clear(string)                    \
    do {                                        \
        free((string)->data);                   \
        (string)->data = NULL;                  \
        (string)->size = 0;                     \
        (string)->alloc = 0;                    \
    } while (0)

inline string_view_t
string_view_from_string(string_t *s)
{
    return (string_view_t){ s->data, s->size };
}

string_view_t
string_view_from_cstr(char *cstr)
{
    return (string_view_t){ cstr, strlen(cstr) };
}

bool
string_view_equal(string_view_t a, string_view_t b)
{
    if (a.size != b.size) return false;
    for (size_t i = 0; i < a.size; i++)
        if (a.data[i] != b.data[i]) return false;
    return true;
}

string_view_t
string_view_chop(string_view_t *v, char delim)
{
    size_t i = 0;
    while (i < v->size && v->data[i] != delim) i++;

    string_view_t out = { v->data, i };

    if (i < v->size) {
        v->data += i + 1;
        v->size -= i + 1;
    } else {
        v->data += i;
        v->size = 0;
    }

    return out;
}

string_view_t
string_slice(const string_t *s, size_t start, size_t end)
{
    if (start > s->size) start = s->size;
    if (end > s->size) end = s->size;
    if (end < start) end = start;

    return (string_view_t) {
        s->data + start,
        end - start
    };
}

string_view_t
string_view_trim_left(string_view_t v)
{
    size_t i = 0;
    while (i < v.size && isspace((unsigned char)v.data[i]))
        ++i;

    return (string_view_t){
        v.data + i,
        v.size - i
    };
}

string_view_t
string_view_trim_right(string_view_t v)
{
    size_t end = v.size;
    while (end > 0 && isspace((unsigned char)v.data[end - 1]))
        --end;

    return (string_view_t){
        v.data,
        end
    };
}

string_view_t
string_view_trim(string_view_t v)
{
    return string_view_trim_right(string_view_trim_left(v));
}
