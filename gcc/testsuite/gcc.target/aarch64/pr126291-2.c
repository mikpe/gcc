/* PR tree-optimization/126291 */
/* { dg-do compile } */
/* { dg-options "-O3" } */

void
f (double *restrict out, const double *x, const double *y, long n)
{
  for (long i = 0; i < n; i++)
    out[i] = __builtin_copysignf ((float) x[i], (float) y[i]);
}
