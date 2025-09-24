#include <stdio.h>
#include "record.h"

int main(int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "Need filename.\n");
    return 1;
  }
  FILE *f = fopen(argv[1], "rb");
  if (f == NULL) {
    perror("fopen");
    return 1;
  }
  
  record record;
  unsigned char name_len;

  while(fread(&record, sizeof(record), 1, f) == 1) {
    name_len = record.name_len;

    printf("%.*s has %hu sunspots\n", name_len, record.name, record.sunspots);

  }

  fclose(f);
  return 0;
}
