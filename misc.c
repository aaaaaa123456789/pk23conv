#include "proto.h"

char * duplicate_string (const char * string) {
  return strcpy(malloc(strlen(string) + 1), string);
}

char * generate_string (const char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char * result = generate_string_from_varargs(fmt, ap);
  va_end(ap);
  return result;
}

char * generate_string_from_varargs (const char * fmt, va_list varargs) {
  char testbuf[2];
  va_list temp;
  va_copy(temp, varargs);
  int rv = vsnprintf(testbuf, 1, fmt, temp);
  va_end(temp);
  if (rv < 0) return NULL;
  char * result = malloc(rv + 1);
  vsnprintf(result, rv + 1, fmt, varargs);
  return result;
}

uint64_t get_initial_random_seed (const void * data, unsigned length) {
  unsigned char * hash = calculate_sha1(data, length);
  unsigned p;
  uint64_t result = read_number_from_buffer(hash, 8);
  libsrng_random(&result, 1, 1);
  result ^= read_number_from_buffer(hash + 8, 8);
  libsrng_random(&result, 1, 1);
  p = read_number_from_buffer(hash + 16, 4);
  result ^= ((uint64_t) p << 32) | p;
  free(hash);
  for (p = 0; p < 5; p ++) libsrng_random(&result, 0, 0);
  return result;
}

unsigned long long read_number_from_buffer (const unsigned char * buffer, unsigned char length) {
  unsigned long long result = 0;
  unsigned char pos;
  for (pos = 0; pos < length; pos ++) result |= (unsigned long long) buffer[pos] << (pos * 8);
  return result;
}

unsigned long long read_big_endian_number (const unsigned char * buffer, unsigned char length) {
  unsigned long long result = 0;
  unsigned char pos;
  buffer += length;
  for (pos = 0; pos < length; pos ++) result |= (unsigned long long) *(-- buffer) << (pos * 8);
  return result;
}

void write_number_to_buffer (unsigned char * buffer, unsigned char length, unsigned long long number) {
  while (length --) {
    *(buffer ++) = number;
    number >>= 8;
  }
}
