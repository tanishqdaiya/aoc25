#define SV_Fmt "%.*s"
#define SV_EL "\n"
#define SV_Arg(sv) (int)sv.size, sv.data

typedef struct {
    u32 *data;
    size_t size, alloc;
} U32_Row;

typedef struct {
    U32_Row *data;
    size_t size, alloc;
} U32_Matrix;

internal inline u32
u32_matrix_get(U32_Matrix *m, size_t i, size_t j)
{
    assert(i < m->size);
    assert(j < m->data[i].size);
    return m->data[i].data[j];
}

u8
read_file_into_string(TD_String *str, FILE *fp)
{
    i32 file_size;
    size_t read;

    if (fseek(fp, 0, SEEK_END) != 0)
        return 0;

    file_size = ftell(fp);
    if (file_size < 0)
        return 0;

    rewind(fp);

    td__vec_alloc(str, (size_t)file_size);
    read = fread(str->data, 1, (size_t)file_size, fp);
    if (read != (size_t)file_size)
        return 0;

    str->size = read;
    return 1;
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

    /*
      read file and get no of rows and no of cols
      read file by line.
      fill columns for each row.
      lastly, process the symbols and add.
     */

    TD_String content = { 0 };
    read_file_into_string(&content, fp);

    TD_String_View content_view, line, character = { 0 };
    content_view = td_string_view_from_string(&content);

    TD_String symbols = { 0 };
    U32_Matrix matrix = { 0 };
    while (content_view.size) {
        line = td_string_view_chop(&content_view, '\n');
        if (line.size == 0)
            continue;

        printf(">" SV_Fmt SV_EL, SV_Arg(line));

        U32_Row row = { 0 };
        while (line.size) {
            character = td_string_view_chop(&line, ' ');
            if (character.size == 0)
                continue;

            if (character.data[0] == '*' || character.data[0] == '+') {
                printf("'%c' encountered.\n", character.data[0]);
                td_vec_append(&symbols, character.data[0]);
                continue;
            }
            
            printf(">>" SV_Fmt SV_EL, SV_Arg(character));

            u32 n = 0;
            for (i32 i = 0; i < character.size; ++i)
                n = (n * 10) + (int)(character.data[i] - '0');
            td_vec_append(&row, n);
        }

        if (row.size > 0)
            td_vec_append(&matrix, row);
    }

    #if 0
    for (size_t r = 0; r < matrix.size; ++r) {
        U32_Row *row = &matrix.data[r];

        for (size_t c = 0; c < row->size; ++c) {
            printf("%u", row->data[c]);
            if (c + 1 < row->size)
                printf(" ");
        }

        printf("\n");
    }
    #endif

    u64 final_sum = 0;
    for (size_t col = 0; col < symbols.size; ++col) {
        char symbol = symbols.data[col];
        if (symbol == '+') {
            u64 post_op = 0;
            for (size_t row = 0; row < matrix.size; ++row)
                post_op += u32_matrix_get(&matrix, row, col);
            final_sum += post_op;
        }

        if (symbol == '*') {
            u64 post_op = 1;
            for (size_t row = 0; row < matrix.size; ++row)
                post_op *= u32_matrix_get(&matrix, row, col);
            final_sum += post_op;
        }
    } printf("\n");

    printf("%I64u\n", final_sum);
    
    
    return 0;
}
