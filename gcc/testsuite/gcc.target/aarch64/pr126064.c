/* { dg-options "-std=c99" } */

/* Check that `gimple_permute_pair::fold()` does not trip an assert when the LHS
   type is `uint32x2_t` and the RHS type is `__Uint32x2_t`.  */

#include <arm_neon.h>

void
repro (uint8_t *dst, uint8x8_t a, uint8x8_t b)
{
  uint32x2x2_t t;
  uint32x2_t r;
  uint8x8_t c;

  r = vrev64_u32 (vreinterpret_u32_u8 (b));
  t = vtrn_u32 (vreinterpret_u32_u8 (a), r);
  c = vreinterpret_u8_u32 (t.val[1]);
  t = vtrn_u32 (t.val[0], vreinterpret_u32_u8 (c));
  t.val[1] = vrev64_u32 (t.val[1]);

  vst1_u8 (dst, vreinterpret_u8_u32 (t.val[1]));
}

uint32x2_t
minimized_repro (uint32x2_t a, uint32x2_t b)
{
  return vtrn_u32 (a, vrev64_u32 (b)).val[1];
}
