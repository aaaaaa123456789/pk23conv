#include "proto.h"

unsigned generate_personality_value (uint64_t * random_state, unsigned char species, unsigned char gender, unsigned char ability) {
  unsigned p, result;
  // first step: pick some bits to fix the gender and ability - positions to fill (marked with X): 000000xx000000xx000000xxxxxxxxxx
  if (species == UNOWN) {
    // don't care about the ability here since it can only have one; pick the letter (in the gender argument) correctly and randomize the rest
    p = libsrng_random(random_state, 9 + (gender < 4), 0) * 28 + gender; // 10 possible values for 0-3, 9 values for 4+
    result = (p & 3) | ((p & 0xc) << 6) | ((p & 0x30) << 12) | ((p & 0xc0) << 18);
    // Unown is genderless, so the gender thresholds don't matter; randomize the rest of the lowest byte
    result |= libsrng_random(random_state, 0, 0) & 0xfc;
  } else {
    p = gender_ratio[species];
    if (!p || (p >= 8))
      // if the species doesn't come in male and female variants, just pick the non-ability bits at random
      result = libsrng_random(random_state, 0, 0) & 0xfe;
    else {
      // generate the upper seven bits of the lower byte to meet the threshold - start by computing the threshold
      if (!gender) p = 8 - p;
      p <<= 4;
      // for the second ability, shift the threshold by 1, to account for the game's off-by-one error when determining genders
      if (ability) p = gender ? (p - 1) : (p + 1);
      // and generate the corresponding value
      result = libsrng_random(random_state, p, 0) << 1;
      if (!gender) result ^= 0xfe;
    }
    // now fill in the ability bit and the remaining upper bits
    if (ability) result |= 1;
    p = libsrng_random(random_state, 0, 0);
    result |= ((p & 0x303) << 8) | ((p & 0xc000) << 10);
  }
  // now, fill up the upper halfword - those bits don't really do anything interesting
  result |= (unsigned) (libsrng_random(random_state, 0x4000, 0) & 0x3f3f) << 18;
  // finally, ensure the result gets a neutral nature - pick one and manipulate the six remaining bits to make it have that nature
  p = (result % 25) - 6 * libsrng_random(random_state, 5, 0);
  if (p >= 25) p += 25; // underflow
  p += libsrng_random(random_state, 2 + (p < 13), 0) * 25;
  return result | (p << 10);
}

unsigned short generate_secret_OTID (uint64_t * random_state, unsigned personality, unsigned short OTID, unsigned char shiny) {
  OTID ^= personality ^ (personality >> 16);
  OTID &= 0xfff8; // the lower three bits don't matter
  if (shiny) return OTID | libsrng_random(random_state, 8, 0);
  unsigned short result;
  do
    result = libsrng_random(random_state, 0, 0);
  while ((result & 0xfff8) == OTID);
  return result;
}
