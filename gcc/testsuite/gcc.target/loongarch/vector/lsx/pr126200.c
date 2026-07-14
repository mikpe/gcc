/* { dg-do assemble } */
/* { dg-options "-O2 -mlsx" } */

typedef signed char v16i8 __attribute__ ((vector_size (16), aligned (16)));

v16i8
lsx_convert_utf16_to_utf8 (v16i8 in)
{
  v16i8 x = __builtin_lsx_vshuf4i_b (in, 0b10110001);
  return __builtin_lsx_vpickev_b (x, x);
}
