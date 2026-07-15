/* { dg-do compile } */
/* { dg-require-effective-target vect_int } */

int foo (int *a, int *b, int c)
{
  return (c ^ 1) + ((a[0] | b[0]) + (a[1] | b[1]) + (a[2] | b[2]) + (a[3] | b[3]) + (a[4] | b[4]));
}

/* Make sure that we pick matching lanes when attempting to BB vectorize
   a reduction rather than arbitrarily cutting back to the number of
   vector lanes.  */
/* { dg-final { scan-tree-dump "optimized: basic block part vectorized" "slp2" { target { vect_hw_misalign && { x86_64-*-* i?86-*-* aarch64-*-* } } } } } */
