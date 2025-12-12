/*
  Admittedly, I spend waaay more time debugging this than I am brave to admit. At last, however, we have solved it.
 */

typedef struct { char *data; size_t size, alloc; } buffer_t;
typedef struct { u64 first, second; } pair_t;
typedef struct { pair_t data; size_t size, alloc; } vector_t;

#define vec_append(array, item)                                         \
    do {                                                                \
        if (array.size >= array.alloc) {                                \
            if (array.alloc == 0) array.alloc = 1024;                   \
            else array.alloc *= 2;                                      \
            array.data = realloc(array.data,                            \
                                 sizeof(*array.data) * array.alloc);    \
            if (array.data == NULL) {                                   \
                fprintf(stderr, "realloc: can't allocate memory\n");    \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        }                                                               \
        array.data[array.size++] = item;                                \
    } while (0)

u64
find_number_length (u64 number)
{
  u64 n, k;

  n = 1; /* The number will at least have a length of 1 */
  k = 10;
  while (number%k != number)
    {
      n += 1;
      k *= 10;
    }

  return n;
}

/* Inefficiency at its best */
u64
pow10 (u64 power)
{
  u64 p = 1;
  while (power-- > 0) p *= 10;
  return p;
}

u64
symmetrical_split (u64 number)
{
  u64 len, first_half, second_half;

  //printf ("=>%I64u\n", number);
  
  len = find_number_length(number);
  if (len < 2 || len % 2 != 0)
    return 0;

  second_half = number % (pow10 (len/2));
  first_half = (number - second_half) / (pow10 (len/2));

  //printf("first_half=%I64u;second_half=%I64u => %I64u\n", first_half, second_half, first_half == second_half);

  if (first_half == second_half)
    {
      //printf("first_half=%I64u;second_half=%I64u => %I64u\n", first_half, second_half, first_half == second_half);
      return 1;
    }

  return 0;
}

u64
add_invalids_in_range (u64 lower, u64 upper)
{
  /* Lower and Upper included */
  u64 invalids;

  invalids = 0;
  for (u64 i = lower; i <= upper; ++i)
    {
      if (symmetrical_split (i))
        {
          //printf ("symmetry: %I64u\n", symmetrical_split (i));
          invalids += i;
        }
    }

  return invalids;
}

int
str_substr_bounded(char *dst,
                   size_t dst_size,
                   const char *src,
                   size_t src_len,
                   u32 start,
                   u32 end)
{
    if (!dst || !src || dst_size == 0) return -1;
    if (start > end || (size_t)end > src_len) return -1;
    size_t len = (size_t)(end - start);
    if (len >= dst_size) return 1;
    if (len)
        memcpy(dst, src + start, len);
    dst[len] = '\0';
    return 0;
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

    buffer_t buffer = { 0 };

    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') continue; // Since the input file is a single line, we get proper input
        vec_append(buffer, ch);
    } vec_append(buffer, '\0');

    printf("Buffer {\n\tdata: \"%s\";\n\tsize: %zu\n\talloc: %zu\n};\n",
           buffer.data, buffer.size, buffer.alloc);

    u64 result = 0;
    u32 k = 0;
    u32 have_lower = 0;
    pair_t range = { 0 };
    for (u32 i = 0; i < buffer.size; ++i) {
        char buf[256];
        char c = buffer.data[i];

        if (c == '-') {
            u32 length = i - k;
            if (length > sizeof(buf)) {
                fprintf(stderr, "Token too long! (%u)\n", length);
                exit(EXIT_FAILURE);
            }

            if (str_substr_bounded(buf, sizeof(buf), buffer.data, buffer.size, k, i) != 0) {
                fprintf(stderr, "Bad substring (%u,%u)\n", k, i);
                exit(EXIT_FAILURE);
            }

            range.first = (u64) strtoull(buf, NULL, 10);
            have_lower = 1;
            k = i + 1;
        } else if (c == ',' || c == '\0') {
            u32 length = i - k;
            if (length >= sizeof(buf)) {
                fprintf(stderr, "Token too long! (%u)\n", length);
                exit(EXIT_FAILURE);
            }

            if (str_substr_bounded(buf, sizeof(buf), buffer.data, buffer.size, k, i) != 0) {
                fprintf(stderr, "Bad substring (%u,%u)\n", k, i);
                exit(EXIT_FAILURE);
            }

            range.second = (u64) strtoull(buf, NULL, 10); /* or your str_to_u64 */

            if (!have_lower) {
                fprintf(stderr,
                        "Format error: missing lower bound before upper=%I64u\n",
                        range.second);
                exit(EXIT_FAILURE);
            }
            
            if (range.second < range.first) {
                fprintf(stderr,
                        "Invalid range: lower=%I64u > upper=%I64u\n",
                        range.first, range.second);
                exit(EXIT_FAILURE);
            }

            u64 r = add_invalids_in_range(range.first, range.second);
            result += r;
            printf("(%I64u, %I64u) => %I64u\n", range.first, range.second, r);
            have_lower = 0;
            range.first = range.second = 0;
            k = i + 1;
        } else {
        }
    }

    printf("%I64u\n", result);

    return 0;
}
