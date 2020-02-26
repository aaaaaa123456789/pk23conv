#include "proto.h"

void scramble (unsigned char * pkmn) {
  unsigned char order = read_number_from_buffer(pkmn, 4) % 24;
  unsigned char current = order / 6;
  order %= 6;
  while (current --) swap_sub_blocks(pkmn, current, current + 1);
  current = order >> 1;
  while (current --) swap_sub_blocks(pkmn, current + 1, current + 2);
  if (order & 1) swap_sub_blocks(pkmn, 2, 3);
}

void swap_sub_blocks (unsigned char * pkmn, unsigned char first, unsigned char second) {
  unsigned char block[12];
  memcpy(block, pkmn + 32 + first * 12, 12);
  memcpy(pkmn + 32 + first * 12, pkmn + 32 + second * 12, 12);
  memcpy(pkmn + 32 + second * 12, block, 12);
}

void encrypt (unsigned char * pkmn) {
  unsigned char key[4];
  unsigned char pos;
  for (pos = 0; pos < 4; pos ++) key[pos] = pkmn[pos] ^ pkmn[pos + 4];
  for (pos = 32; pos < 80; pos ++) pkmn[pos] ^= key[pos & 3];
}
