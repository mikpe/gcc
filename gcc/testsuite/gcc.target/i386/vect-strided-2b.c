/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -msse2 -mno-avx --param ix86-vect-compare-costs=1" } */

void foo (int * __restrict a, int *b, int s)
{
  for (int i = 0; i < 1024; ++i)
    {
      a[4*i+0] = b[s*i+0];
      a[4*i+1] = b[s*i+1];
      a[4*i+2] = b[s*i+0];
      a[4*i+3] = b[s*i+1];
    }
}

/* One two-element load, two two-element stores.  */
/* { dg-final { scan-assembler-times "movq" 3 } } */
/* { dg-final { scan-assembler-times "movups" 0 } } */
