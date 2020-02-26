#include "proto.h"

int main (int argc, char ** argv) {
  const char * in_name;
  const char * out_name;
  int mode = get_options(argv, &in_name, &out_name);
  FILE * fp = fopen(in_name, "rb");
  if (!fp) error_exit(2, "could not open file %s for reading", in_name);
  unsigned char in[PK2_SIZE], out[PK3_SIZE];
  if (fread(in, 1, sizeof in, fp) != sizeof in) {
    fclose(fp);
    error_exit(2, "input does not contain a valid .pk2 file");
  }
  char * error = convert_pk2_to_pk3(in, out);
  if (error) error_exit(3, "%s", error);
  if (mode) scramble(out);
  if (mode >= 2) encrypt(out);
  fp = fopen(out_name, "wb");
  if (!fp) error_exit(2, "could not open file %s for writing", out_name);
  if (fwrite(out, 1, sizeof out, fp) != sizeof out) {
    fclose(fp);
    error_exit(2, "could not write output data to %s", out_name);
  }
  fclose(fp);
  return 0;
}

int get_options (char ** argv, const char ** in, const char ** out) {
  const char * program = *(argv ++);
  int mode = 2;
  while (*argv && (**argv == '-')) {
    if (strlen(*argv) != 2) usage(program);
    switch (1[*argv]) {
      case 'u': mode = 0; break;
      case 's': mode = 1; break;
      case 'e': mode = 2; break;
      case '-': argv ++; goto done_options;
      default: usage(program);
    }
    argv ++;
  }
  done_options:
  if (!*argv || !argv[1]) usage(program);
  *in = *argv;
  *out = argv[1];
  return mode;
}

void usage (const char * program_name) {
  fprintf(stderr, "usage: %s [-u | -s | -e] [--] <input.pk2> <output.pk3>\n", program_name);
  fputs("  -u: emit an unencrypted, unscrambled file\n", stderr);
  fputs("  -s: emit an unencrypted, scrambled file\n", stderr);
  fputs("  -e: emit an encrypted and scrambled file (default)\n", stderr);
  fputs("  --: end of option list\n", stderr);
  exit(1);
}

void error_exit (int status, const char * fmtstring, ...) {
  fputs("error: ", stderr);
  va_list ap;
  va_start(ap, fmtstring);
  vfprintf(stderr, fmtstring, ap);
  va_end(ap);
  putc('\n', stderr);
  exit(status);
}
