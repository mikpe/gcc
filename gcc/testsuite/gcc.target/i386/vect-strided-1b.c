/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -msse2 -mno-avx --param ix86-vect-compare-costs=1" } */

void foo (int * __restrict a, int *b, int s)
{
  for (int i = 0; i < 1024; ++i)
    {
      a[8*i+0] = b[s*i+0];
      a[8*i+1] = b[s*i+1];
      a[8*i+2] = b[s*i+2];
      a[8*i+3] = b[s*i+3];
      a[8*i+4] = b[s*i+4];
      a[8*i+5] = b[s*i+5];
      a[8*i+6] = b[s*i+4];
      a[8*i+7] = b[s*i+5];
    }
}

/* Three two-element loads, four two-element stores.  No wider loads
   or permutes.  */
/* { dg-final { scan-assembler-times "movq" 7 } } */
/* { dg-final { scan-assembler-times "movhps" 0 } } */
/* { dg-final { scan-assembler-times "movups" 0 } } */
