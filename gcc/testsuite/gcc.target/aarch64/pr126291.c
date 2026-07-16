/* PR tree-optimization/126291 */
/* { dg-do compile } */
/* { dg-options "-O3 -funsafe-math-optimizations -mcpu=ampere1a" } */

double
f (const double *p, double x, double y, long n)
{
  double best = 1e30;
  for (long i = 0; i < n; i++)
    {
      int dx = p[2 * i] - x;
      int dy = p[2 * i + 1] - y;
      double ax = __builtin_abs (dx);
      double ay = __builtin_abs (dy);
      double d = ax * ax + ay * ay;
      if (d < best)
	best = d;
    }
  return best;
}
