#include "proto.h"

void compute_stats (unsigned char * pkmn, unsigned short max_HP, unsigned short current_HP) {
  unsigned char index, boost, nerf;
  unsigned stat;
  unsigned IVs = read_number_from_buffer(pkmn + 72, 4);
  nerf = read_number_from_buffer(pkmn, 4) % 25; // read back the nature to find affected stats
  boost = nerf / 5 + 1;
  nerf = nerf % 5 + 1;
  if (nerf == boost) nerf = boost = -1; // should always happen if nature was selected properly
  for (index = 0; index < 6; index ++) {
    stat = gen3_base_stats[pkmn[32] | ((unsigned) index << 8)] << 1;
    if (!index) stat += 100; // HP offset
    stat += IVs & 31;
    IVs >>= 5;
    stat += pkmn[56 + index] >> 2; // EVs
    stat = stat * pkmn[84] / 100 + (index ? 5 : 10); // scale by level and add the initial value
    if ((index == nerf) || (index == boost)) {
      stat *= 100 + 10 * (index == boost) - 10 * (index == nerf);
      stat &= 0xffff; // known bug: nature calculations will cap stats at 655.35
      stat /= 100;
    }
    write_number_to_buffer(pkmn + 88 + index * 2, 2, stat);
  }
  if (!current_HP) return; // if the mon is fainted, keep it fainted
  stat = read_number_from_buffer(pkmn + 88, 2); // read back the max HP
  if (stat <= (max_HP - current_HP))
    stat = 1; // avoid HP underflow
  else
    stat -= max_HP - current_HP;
  write_number_to_buffer(pkmn + 86, 2, stat);
}

unsigned calculate_IVs_from_DVs (unsigned short DVs) {
  unsigned result = (DVs & 1) | ((DVs & 0x10) >> 3) | ((DVs & 0x100) >> 6) | ((DVs & 0x1000) >> 9); // HP DV
  result <<= 1; // convert to IV
  result |= (unsigned) (DVs & 15) << 26; // do special defense separately
  unsigned p, main_IVs = 0;
  for (p = 0; p < 4; p ++) {
    // each of the four main IVs (attack, defense, speed, special attack)
    main_IVs = (main_IVs << 5) | ((DVs & 15) << 1);
    DVs >>= 4;
  }
  result |= main_IVs << 5;
  return result;
}

unsigned char square_root (unsigned short value) {
  // square roots the gen 1/2 way
  if (value < 3) return value;
  if (value > 64516) return 255;
  unsigned char result;
  for (result = 2; (result * result) < value; result ++);
  return result;
}
