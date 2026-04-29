/* { dg-do compile } */
/* { dg-options "-O3 -mavx512vl -mtune=generic --param ix86-vect-compare-costs=1" } */

int
foo (int n, int* p, int* pi)
{
  int sum = 0;
  for (int i = 0; i != n; i++)
    {
      if (pi[i] > 0)
	sum -= p[i];
    }
  return sum;
}

/* We vectorize with 64 byte vectors and 16 byte vector epilog which
   we completely peel.  */
/* { dg-final { scan-assembler-times "vpsub\[^\r\n\]*%k" 4 } } */
