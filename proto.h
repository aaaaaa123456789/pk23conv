#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "libsrng/libsrng.h"

#define PK2_SIZE 73
#define PK3_SIZE 80

#define INVALID_CHARACTER 0xFE
#define STRING_TERMINATOR 0xFF

#define MEW 151
#define UNOWN 201
#define EGG 253

// convert.c
char * convert_pk2_to_pk3(const unsigned char *, unsigned char *);
unsigned calculate_IVs_from_DVs(unsigned short);
char * convert_string_2_to_3(const unsigned char *, unsigned char *, unsigned);
unsigned generate_personality_value(uint64_t *, unsigned char, unsigned char, unsigned char);
unsigned short generate_secret_OTID(uint64_t *, unsigned, unsigned short, unsigned char);

// main.c
int main(int, char **);

// misc.c
char * duplicate_string(const char *);
char * generate_string(const char * fmt, ...);
char * generate_string_from_varargs(const char *, va_list);
uint64_t get_initial_random_seed(const void *, unsigned);
unsigned long long read_number_from_buffer(const unsigned char *, unsigned char);
unsigned long long read_big_endian_number(const unsigned char *, unsigned char);
void write_number_to_buffer(unsigned char *, unsigned char, unsigned long long);
unsigned char square_root(unsigned short);

// sha1.c
unsigned char * calculate_sha1(const void *, unsigned);
void sha1_process_block(const unsigned char *, uint32_t *);

// tables.c
extern const unsigned short item_map_2_to_3[];
extern const unsigned char gender_ratio[];
extern const unsigned char character_map_2_to_3[];
