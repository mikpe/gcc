/* PR middle-end/126221 */
/* { dg-do compile { target bitint } } */
/* { dg-options "-O3 -fvect-cost-model=unlimited --param=vect-partial-vector-usage=2 -mavx10.1" } */

#if __BITINT_MAXWIDTH__ >= 10986
_BitInt(10986) b;
char c;

void
foo (void)
{
  int t;
  if (__builtin_mul_overflow (0xffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffff0000ffffuwb, c, &t))
    t = 1;
  b = t;
}

#endif
