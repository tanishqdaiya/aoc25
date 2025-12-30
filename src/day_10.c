#define INPUT "input"

struct u64_list {
    u64 *data;
    size_t size, alloc;
};

internal i32 sv_to_int(TD_String_View);
internal u64 hex_from_line(TD_String_View);
internal void parse_buttons_to_hex(struct u64_list*, TD_String_View);
internal i32 run_buttons(u64, struct u64_list*);

int main(void)
{
    FILE *fp;
    i32 result;
    TD_String buf;
    TD_String_View v;

    fp = fopen(INPUT, "rb");
    if (fp == NULL) {
        perror(INPUT);
        exit(EXIT_FAILURE);
    }

    buf.data = NULL;
    buf.alloc = buf.size = 0;
    
    if (!td_read_file_to_string(&buf, fp)) {
        fprintf(stderr, "fatal: error reading file into buffer\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(fp);

    v = td_string_view_from_string(&buf);
    result = 0;
    while (v.size > 0) {
        u64 target;
        i32 min_presses;
        TD_String_View line;
        struct u64_list buttons;
        
        line = td_string_view_chop(&v, '\n');
        line = td_string_view_trim(line);
        if (line.size == 0)
            continue;
        
        target = hex_from_line(line);

        buttons.data = NULL;
        buttons.alloc = buttons.size = 0;
        
        parse_buttons_to_hex(&buttons, line);

        min_presses = run_buttons(target, &buttons);
        if (min_presses < 0) {
            fprintf(stderr, "error: can't solve machine.\n");
            exit(EXIT_FAILURE);
        }

        result += min_presses;
    }

    printf("%d\n", result);
}

internal i32 sv_to_int(TD_String_View v)
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

internal u64 hex_from_line(TD_String_View line)
{
    size_t i, start, end;
    char ch;
    u64 bits;
    TD_String_View v;

    start = end = 0;
    
    for (i = 0; i < line.size; ++i) {
        ch = line.data[i];

        if (ch == '[')
            start = i;
        if (ch == ']') {
            end = i;
            break;
        }
    }

    bits = 0;

    v = td_string_view_slice(line, start+1, end);
    
    for (i = 0; i < v.size; ++i) {
        ch = v.data[i];
        if (ch == '#')
            bits = bits | (1ULL << i);
    }
    
    return bits;
}

internal void parse_buttons_to_hex(struct u64_list *list, TD_String_View line)
{
    char ch;
    size_t i, start;
    u64 x;

    start = 0;

    for (i = 0; i < line.size; ++i) {
        ch = line.data[i];

        /* Assuming that the input comes pre-sanitized. */
        if (ch == '(') {
            start = i;
            x = 0;
        }
        else if (ch == ')') {
            TD_String_View items, token;
            
            items = td_string_view_slice(line, start+1, i);
            while (items.size > 0) {
                token = td_string_view_chop(&items, ',');
                token = td_string_view_trim(token);
                x = x | (1ULL << sv_to_int(token));
            }
            td_vec_append(list, x);
        }
    }
}

internal i32 run_buttons(u64 target, struct u64_list *buttons)
{
    u64 s, state;
    i32 i, presses, best;
    i32 n;

    n = (i32)buttons->size;
    best = -1;

    for (s = 0; s < (1ULL << n); ++s) {
        state = 0;
        presses = 0;

        for (i = 0; i < n; ++i) {
            if (s & (1ULL << i)) {
                state ^= buttons->data[i];
                ++presses;
            }
        }

        if (state == target)
            if (best < 0 || presses < best)
                best = presses;
    }

    return best;
}
