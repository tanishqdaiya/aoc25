#define INPUT "input"

struct point {
    i32 x, y;
};

struct point_list {
    struct point *data;
    size_t size, alloc;
};

static i32 sv_to_int(TD_String_View);
static i64 rect_area(struct point, struct point);

int main(void)
{
    FILE *fp;
    TD_String buf;
    TD_String_View input;
    i64 max_area;
    size_t i, j;
    
    struct point_list points;

    fp = fopen(INPUT, "rb");
    if (fp == NULL) {
        perror(INPUT);
        exit(EXIT_FAILURE);
    }

    buf.data = NULL;
    buf.size = buf.alloc = 0;

    if (!td_read_file_to_string(&buf, fp)) {
        fprintf(stderr, "fatal: error reading file into buffer");
        exit(EXIT_FAILURE);
    }

    input = td_string_view_from_string(&buf);

    points.data = NULL;
    points.size = points.alloc = 0;

    while (input.size > 0) {
        TD_String_View line, x, y;
        struct point p;

        line = td_string_view_chop(&input, '\n');
        line = td_string_view_trim(line);
        if (line.size == 0)
            continue;

        x = td_string_view_chop(&line, ',');
        y = line;

        p.x = sv_to_int(x);
        p.y = sv_to_int(y);

        td_vec_append(&points, p);
    }

    max_area = 0;
    for (i = 0; i < points.size; ++i) {
        for (j = i+1; j < points.size; ++j) {
            i64 area;

            area = rect_area(points.data[i], points.data[j]);
            if (area > max_area)
                max_area = area;
        }
    }

    printf("%lld\n", max_area);
    return 0;
}

static i32 sv_to_int(TD_String_View v)
{
    i32 n;
    size_t i;
    char c;

    n = 0;
    for (i = 0; i < v.size; ++i) {
        c = v.data[i];
        if (!isdigit((unsigned char)c))
            break;
        n = n * 10 + (c - '0');
    }

    return n;
}

static i64 rect_area(struct point a, struct point b)
{
    i32 w, h;
    w = abs(a.x - b.x) + 1;
    h = abs(a.y - b.y) + 1;

    return (i64)w * (i64)h;
}
