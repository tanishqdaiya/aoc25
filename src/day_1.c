int
dialer (int current, int rotation)
{
  int min, max, x;

  min = 0;
  max = 99;
  //  min = 1;
  //  max = 6;

  rotation = rotation % (max + 1);
  x = current + rotation;

  if (x < 0 || x > max)
    {
      int v;
      
      v = x % (max + 1);
      if (v < 0)
        v += max + 1;
      return v;
    }
  else
    {
      return x;
    }
}

int
parse_rotation_from_line (char *line)
{
  int rotation;
  char *pline;

  rotation = 0;
  pline = line;
  ++pline;
  while (*pline != '\n')
    {
      int digit;

      digit = (int)*pline - 48; /* man 7 ascii */
      rotation = rotation * 10 + digit;
      pline++;
    }

  if (line[0] == 'L')
      return -1 * rotation;
  else
    {
      if (line[0] != 'R') {
        fprintf (stderr, "error: bad input \"%s\"\n", line);
        exit (EXIT_FAILURE);
      }

      return rotation;
    }
}

int
main (int argc, char **argv)
{
  FILE *input_file;
  const char *filename = "input.txt";

  input_file = fopen (filename, "r");
  if (input_file == NULL)
    {
      fprintf (stderr, "fopen: can't open file \"%s\"\n", filename);
      exit (EXIT_FAILURE);
    }

  {
    char line_buffer[64];
    int current, zero_count;

    current = 50;
    zero_count = 0;
    while (fgets (line_buffer, sizeof (line_buffer), input_file) != NULL)
      {
        int rotation;

        rotation = parse_rotation_from_line (line_buffer);
        current = dialer (current, rotation);
        if (current == 0)
          ++zero_count;
      }

    printf ("zero_count: %d\n", zero_count);
  }

  return 0;
}
