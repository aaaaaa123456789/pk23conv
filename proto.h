#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "libsrng/libsrng.h"

#define PK2_SIZE 73
#define PK3_SIZE 100

#define INVALID_CHARACTER 0xFE
#define STRING_TERMINATOR 0xFF

#define MEW 151
#define UNOWN 201
#define EGG 253

// basestat.c
extern const unsigned char gen3_base_stats[];

// convert.c
char * convert_pk2_to_pk3(const unsigned char *, unsigned char *);
char * convert_string_2_to_3(const unsigned char *, unsigned char *, unsigned);

// main.c
int main(int, char **);
int get_options(char **, const char **, const char **);
void usage(const char *);
void error_exit(int, const char *, ...);

// misc.c
char * duplicate_string(const char *);
char * generate_string(const char * fmt, ...);
char * generate_string_from_varargs(const char *, va_list);
uint64_t get_initial_random_seed(const void *, unsigned);
unsigned long long read_number_from_buffer(const unsigned char *, unsigned char);
unsigned long long read_big_endian_number(const unsigned char *, unsigned char);
void write_number_to_buffer(unsigned char *, unsigned char, unsigned long long);

// pv.c
unsigned generate_personality_value(uint64_t *, unsigned char, unsigned char, unsigned char);
unsigned short generate_secret_OTID(uint64_t *, unsigned, unsigned short, unsigned char);

// scramble.c
void scramble(unsigned char *);
void swap_sub_blocks(unsigned char *, unsigned char, unsigned char);
void encrypt(unsigned char *);

// sha1.c
unsigned char * calculate_sha1(const void *, unsigned);
void sha1_process_block(const unsigned char *, uint32_t *);

// stats.c
void compute_stats(unsigned char *, unsigned short, unsigned short);
unsigned calculate_IVs_from_DVs(unsigned short);
unsigned char square_root(unsigned short);

// tables.c
extern const unsigned short item_map_2_to_3[];
extern const unsigned char gender_ratio[];
extern const unsigned char character_map_2_to_3[];
