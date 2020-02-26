#include "proto.h"

char * convert_pk2_to_pk3 (const unsigned char * in, unsigned char * out) {
  memset(out, 0, PK3_SIZE);
  uint64_t random_state = get_initial_random_seed(in, PK2_SIZE);
  if ((*in != 1) || (in[2] != 0xff)) return duplicate_string("invalid PK2 data");
  if ((in[1] != EGG) && (in[1] != in[3])) return generate_string("PK2 is an unstable hybrid (%hhu, %hhu)", in[1], in[3]);
  if ((in[3] > 251) || !in[3]) return generate_string("PK2 contains an invalid species (%hhu)", in[3]);
  out[32] = in[3]; // species
  write_number_to_buffer(out + 34, 2, item_map_2_to_3[in[4]]); // held item
  unsigned p;
  if (!in[5]) return duplicate_string("PK2 contains no moves");
  for (p = 0; p < 4; p ++) {
    if (in[5 + p] > 251) return generate_string("PK2 contains an invalid move (%hhu) at position %u", in[5 + p], p + 1);
    if (p && in[5 + p] && !in[4 + p]) return duplicate_string("PK2 contains a null move");
    out[44 + 2 * p] = in[5 + p]; // each move
  }
  write_number_to_buffer(out + 4, 2, read_big_endian_number(in + 9, 2)); // OT ID (low halfword, byte-swapped)
  write_number_to_buffer(out + 36, 3, read_big_endian_number(in + 11, 3)); // experience points (3 bytes, byte-swapped)
  for (p = 0; p < 5; p ++) out[56 + p] = square_root(read_big_endian_number(in + 14 + 2 * p, 2)); // each EV (converted from stat exp)
  out[61] = out[60]; // special defense EV is the same as special attack EV
  write_number_to_buffer(out + 72, 4, calculate_IVs_from_DVs(read_big_endian_number(in + 24, 2)) // converted DVs
                                      + 0x40000000u * (in[1] == EGG) // egg flag
                                      + 0x80000000u * libsrng_random(&random_state, 2, 0)); // alternate ability flag
  for (p = 0; p < 4; p ++) out[52 + p] = in[26 + p]; // PP
  out[41] = in[30]; // friendship
  out[68] = in[31]; // Pokerus - fortunately stored in the same format in both gens
  p = read_number_from_buffer(in + 32, 2) & 0x803f; // origin data, but only keep the useful parts
  p |= libsrng_random(&random_state, 6, 0) << 7;
  if (!(p & 0x780)) p |= 0x780; // randomize the game of origin
  p |= (1 + libsrng_random(&random_state, 12, 0)) << 11; // randomize the caught ball
  write_number_to_buffer(out + 70, 2, p);
  if (in[3] == MEW) {
    out[79] = 0x80; // fateful encounter flag for Mew
    out[69] = 0xff; // met location: fateful encounter
  } else
    out[69] = 0xfe; // met location: trade
  char * error;
  if (error = convert_string_2_to_3(in + 51, out + 20, 7)) return error; // OT name
  if (in[1] == EGG) {
    memcpy(out + 8, (unsigned char []) {0x60, 0x6f, 0x8b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, 10); // katakana for "tamago" (egg)
    out[18] = 1;
    out[19] = 6; // language: special value for eggs
  } else {
    if (error = convert_string_2_to_3(in + 62, out + 8, 10)) return error; // nickname
    out[18] = out[19] = 2; // language: English
  }
  p = read_big_endian_number(in + 24, 2);
  if (in[3] == UNOWN)
    // if the species is Unown, use the gender value to store the Unown letter
    p = (((p & 6) >> 1) | ((p & 0x60) >> 3) | ((p & 0x600) >> 5) | ((p & 0x6000) >> 7)) / 10;
  else
    // otherwise, get the actual gender (0 if male, 1 if female, don't care if genderless)
    p = (p >> 13) < gender_ratio[in[3]];
  p = generate_personality_value(&random_state, in[3], p, out[75] >> 7);
  write_number_to_buffer(out, 4, p);
  p = generate_secret_OTID(&random_state, p, read_number_from_buffer(out + 4, 2), (in[25] == 0xaa) && ((in[24] & 0x2f) == 0x2a));
  write_number_to_buffer(out + 6, 2, p);
  unsigned short checksum = 0;
  for (p = 0; p < 24; p ++) checksum += read_number_from_buffer(out + 32 + p * 2, 2);
  write_number_to_buffer(out + 28, 2, checksum);
  out[80] = in[35]; // status
  out[84] = in[34]; // level
  out[85] = (in[31] && !(in[31] & 0xf)) ? 0 : 0xff; // Pokerus status
  compute_stats(out, read_big_endian_number(in + 39, 2), read_big_endian_number(in + 37, 2));
  return NULL;
}

char * convert_string_2_to_3 (const unsigned char * in, unsigned char * out, unsigned length) {
  unsigned p;
  if (*in == 0x5d) {
    // special case for gen 1 in-game trades; those might have been brought into gen 2
    memcpy(out, (unsigned char []) {0xce, 0xcc, 0xbb, 0xc3, 0xc8, 0xbf, 0xcc}, (length > 7) ? 7 : length); // "TRAINER"
    if (length > 7) for (p = 7; p < length; p ++) out[p] = STRING_TERMINATOR;
    return NULL;
  }
  for (p = 0; p < length; p ++, in ++, out ++) {
    *out = character_map_2_to_3[*in];
    if (*out == STRING_TERMINATOR) break;
    if (*out == INVALID_CHARACTER) return generate_string("string contains invalid character (%hhu)", *in);
  }
  if (character_map_2_to_3[*in] != STRING_TERMINATOR) return duplicate_string("string too long");
  for (; p < length; p ++) *(out ++) = STRING_TERMINATOR;
  return NULL;
}
