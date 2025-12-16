/* @Todo tanishqdaiya: Optimize later */

typedef enum {
    READ_RANGE,
    READ_ITEMS
} Read_Mode;

typedef struct {
    u64 upper;
    u64 lower;
} Range;

typedef struct {
    Range *data;
    size_t size, alloc;
} Range_Vector;

#define vec_append(vector, item)                                        \
    do {                                                                \
        if ((vector)->size >= (vector)->alloc) {                        \
            if ((vector)->alloc == 0) (vector)->alloc = 1024;           \
            else (vector)->alloc *= 2;                                  \
            (vector)->data = realloc((vector)->data,                    \
                                     sizeof(*(vector)->data) * (vector)->alloc); \
            if ((vector)->data == NULL) {                               \
                fprintf(stderr, "realloc: can't allocate memory\n");    \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        }                                                               \
        (vector)->data[(vector)->size++] = (item);                      \
    } while (0)

bool
is_empty_line(char *line)
{
    char *ch;
    bool empty = true;

    for (ch = line; *ch != '\0'; ++ch) {
        if (isspace(*ch)) continue;
        empty = false;
    }

    return empty;
}

/* In programmer we trust */
void
strslice(char *dst, char *src, i32 start, i32 end)
{
    i32 len = end - start;
    memcpy(dst, src + start, end);
    dst[len] = '\0';
}

Range
parse_line(char *line)
{
    Range r = {0};
    char *pline;
    i32 start, end;
    
    start = end = 0;
    for (pline = line; *pline != '\0'; ++pline, ++end) {
        if (*pline == '-') {
            char buf[16];
            strslice(buf, line, start, end);
            r.lower = (u64)strtoull(buf, NULL, 10);
            start = end + 1;
        }
    }

    char buf[16];
    strslice(buf, line, start, end);
    r.upper = (u64)strtoull(buf, NULL, 10);
    
    return r;
}

int
main(void)
{
    FILE *fp;
    const char *filename = "input";

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char line[128];
    Read_Mode rm = READ_RANGE;
    Range_Vector ranges = { 0 };
    u32 fresh = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (is_empty_line(line)) {
            rm = READ_ITEMS;
            continue;
        }

        if (rm == READ_RANGE) {
            Range range = parse_line(line);
            vec_append(&ranges, range);
        }

        if (rm == READ_ITEMS) {
            u64 n = strtoull(line, NULL, 10);
            for (size_t i = 0; i < ranges.size; ++i) {
                Range r = ranges.data[i];
                if (n <= r.upper && n >= r.lower) {
                    ++fresh;
                    break;
                }
            }
        }
    }

    printf("%I32u\n", fresh);
        
    return 0;
}
