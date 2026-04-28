/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -msse4.2 -mno-avx -fno-tree-slp-vectorize --param ix86-vect-compare-costs=1" } */

void foo (int * __restrict a, int * __restrict b, int *c, int s)
{
  if (s >= 2)
    for (int i = 0; i < 1024; ++i)
      {
	a[s*i+0] = c[4*i+0];
	a[s*i+1] = c[4*i+1];
	b[s*i+0] = c[4*i+2];
	b[s*i+1] = c[4*i+3];
      }
}

/* Vectorization factor two, two two-element stores using movq
   and two two-element stores to b via movq.  One reg-reg copy with movq.  */
/* { dg-final { scan-assembler-times "movq\[^\r\n\]+\\\(" 4 } } */
/* { dg-final { scan-assembler-times "pextrq" 0 } } */
