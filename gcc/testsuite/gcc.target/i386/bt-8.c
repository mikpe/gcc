/* { dg-do compile } */
/* { dg-options "-O2 -mtune=core2" } */

_Bool mask_get_flag(long mask, int offset)
{
  return mask & 1ul << (offset % (__SIZEOF_LONG__ * __CHAR_BIT__));
}

/* { dg-final { scan-assembler "bt\[lq\]\[ \t\]" } } */
/* { dg-final { scan-assembler-not "sal\[lq\]\[ \t\]" } } */
