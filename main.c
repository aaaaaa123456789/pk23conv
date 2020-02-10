#include "proto.h"

int main (int argc, char ** argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s input.pk2 output.pk3\n", *argv);
    return 1;
  }
  FILE * fp = fopen(argv[1], "rb");
  if (!fp) {
    fprintf(stderr, "error: could not open file %s for reading\n", argv[1]);
    return 2;
  }
  unsigned char in[PK2_SIZE], out[PK3_SIZE];
  if (fread(in, 1, sizeof in, fp) != sizeof in) {
    fputs("error: input does not contain a valid .pk2 file\n", stderr);
    fclose(fp);
    return 2;
  }
  char * error = convert_pk2_to_pk3(in, out);
  if (error) {
    fprintf(stderr, "conversion error: %s\n", error);
    free(error);
    return 3;
  }
  fp = fopen(argv[2], "wb");
  if (!fp) {
    fprintf(stderr, "error: could not open file %s for writing\n", argv[2]);
    return 2;
  }
  if (fwrite(out, 1, sizeof out, fp) != sizeof out) {
    fprintf(stderr, "error: could not write output data to %s\n", argv[2]);
    fclose(fp);
    return 2;
  }
  fclose(fp);
  return 0;
}
