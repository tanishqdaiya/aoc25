typedef struct { char *data; i32 rows, cols; } char_matrix;

u8
matrix_at_index(char_matrix *matrix, i32 row, i32 col, char *value)
{
    if (row >= matrix->rows || col >= matrix->cols || row < 0 || col < 0) {
#ifdef DEBUG
        fprintf(stderr,
                "Index Out Of Bounds: (%d, %d) for %dx%d matrix.\n",
                row, col, matrix->rows, matrix->cols);
#endif
        return -1;
    }
    
    if (value != NULL) {
        i32 index = row * matrix->cols + col;
        *(value) = matrix->data[index];
    }

    return 0;
}

void
matrix_print(char_matrix *matrix)
{
    for (i32 i = 0; i < matrix->rows; ++i) {
        for (i32 j = 0; j < matrix->cols; ++j) {
            char value;
            if (matrix_at_index(matrix, i, j, &value) == 0)
                printf("%c", value);
        }
        putchar('\n');
    }
}

u8
matrix_set_at_index(char_matrix *matrix, i32 row, i32 col, char value)
{
    if (row >= matrix->rows || col >= matrix->cols || row < 0 || col < 0) {
#ifdef DEBUG
        fprintf(stderr,
                "Index Out Of Bounds: (%d, %d) for %dx%d matrix.\n",
                row, col, matrix->rows, matrix->cols);
#endif
        return -1;
    }
    
    i32 index = row * matrix->cols + col;
    matrix->data[index] = value;

    return 0;
}

global_variable const i32 dirs[8][2] = {
    { 1,  0}, {-1,  0},
    { 0,  1}, { 0, -1},
    { 1,  1}, { 1, -1},
    {-1,  1}, {-1, -1}
};

i32
count_neighbours(char_matrix *matrix)
{
    i32 count = 0;
    
    for (i32 i = 0; i < matrix->rows; ++i) {
        for (i32 j = 0; j < matrix->cols; ++j) {
            char me;
            matrix_at_index(matrix, i, j, &me);
            if (me != '@')
                continue;

            i32 neighbours = 0;
            for (i32 k = 0; k < 8; ++k) {
                char v;
                if (matrix_at_index(matrix,
                                    i + dirs[k][0],
                                    j + dirs[k][1],
                                    &v) == 0 && v == '@')
                    ++neighbours;
            }

            if (neighbours < 4) {
                ++count;
            }
        }
    }
    
    return count;
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

    char_matrix matrix = {0};
    i32 ch;
    i32 col = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            ++matrix.rows;
            if (col > matrix.cols)
                matrix.cols = col;
            col = 0;
        } else {
            ++col;
        }
    }

    if (col > 0) {
        ++matrix.rows;
        if (col > matrix.cols)
            matrix.cols = col;
    }

    matrix.data = malloc(matrix.rows * matrix.cols);
    if (matrix.data == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    rewind(fp);

    i32 row = 0;
    col = 0;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            ++row;
            col = 0;
            continue;
        }

        matrix_set_at_index(&matrix, row, col, (char)ch);
        ++col;
    }

    i32 cnt = count_neighbours(&matrix);
    printf("=> %d\n", cnt);
    matrix_print(&matrix);
    
    return 0;
}
