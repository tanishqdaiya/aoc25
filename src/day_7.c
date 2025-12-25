int
main()
{
    FILE *fp;
    const char *filename = "input";

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    TD_String file_content = { 0 };
    td_read_file_to_string(&file_content, fp);
    fclose(fp);

    TD_String_View content = td_string_view_from_string(&file_content);

    TD_String_View first_line = td_string_view_chop(&content, '\n');
    size_t cols = first_line.size;

    i64 *prev = (i64*)calloc(cols, sizeof(i64));
    i64 *curr = (i64*)calloc(cols, sizeof(i64));
    if (prev == NULL || curr == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < first_line.size; ++i)
        if (first_line.data[i] == 'S')
            prev[i] = 1;

    i64 result = 0;
    while (content.size) {
            TD_String_View line = td_string_view_chop(&content, '\n');
            if (line.size == 0)
                continue;
            
            memset(curr, 0, cols * sizeof(i64));

            for (size_t i = 0; i < line.size; ++i) {
                i64 beams = prev[i];
                if (beams == 0)
                    continue;

                if (line.data[i] == '.')
                    curr[i] += beams;

                if (line.data[i] == '^') {
                    result += 1;

                    if (i > 0) curr[i-1] += beams;
                    if (i+1 < cols) curr[i+1] += beams;
                }
            }

            i64 *tmp = prev;
            prev = curr;
            curr = tmp;
    }

    printf("%I64d\n", result);

    free(prev);
    free(curr);
    free(file_content.data);

    return 0;
}
