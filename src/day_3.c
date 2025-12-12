i32
find_joltage(char *banks)
{
    i32 best = 0;
    
    for (i32 i = 0; i < strlen(banks); ++i) {
        for (i32 j = i + 1; j < strlen(banks); ++j) {
            i32 n1 = banks[i] - 48; /* man 7 ascii */
            i32 n2 = banks[j] - 48; /* man 7 ascii */
            i32 jolts = n1 * 10 + n2;

            if (jolts >= best)
                best = jolts;
        }
    }
    
    return best;
}

int
main()
{
    FILE *fp;
    const char *filename = "input";

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen: can't open file \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }

    i32 result = 0;

    char line_buffer[128];
    while (fgets(line_buffer, sizeof(line_buffer), fp) != NULL) {
        i32 jolts = find_joltage(line_buffer);
        result += jolts;
        printf("%d\n", jolts);
    }

    printf("=> %d\n", result);
    
    return 0;
}
