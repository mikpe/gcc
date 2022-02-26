/* { dg-do link } */
/* { dg-options "-O2" } */

/* Check that transformations from pow to exp2, exp10, and ldexp work.  */

#ifdef __hexagon__
#define HAVE_EXP10 0
#else
#define HAVE_EXP10 1
#endif

double pow (double x, double y);
float powf (float x, float y);
long double powl (long double x, long double y);

double exp2 (double x);
float exp2f (float x);
long double exp2l (long double x);

#if HAVE_EXP10
double exp10 (double x);
float exp10f (float x);
long double exp10l (long double x);
#endif

double ldexp (double x, int exp);
float ldexpf (float x, int exp);
long double ldexpl (long double x, int exp);

void link_error (void);

void test (double x, int y)
{
  if (pow (2.0, x) != exp2 (x))
    link_error ();

#if HAVE_EXP10
  if (pow (10.0, x) != exp10 (x))
    link_error ();
#endif

  if (pow (2.0, y) != ldexp (1.0, y))
    link_error ();
}

void testf (float x, int y)
{
  if (powf (2.0, x) != exp2f (x))
    link_error ();

#if HAVE_EXP10
  if (powf (10.0, x) != exp10f (x))
    link_error ();
#endif

  if (powf (2.0, y) != ldexpf (1.0, y))
    link_error ();
}

void testl (long double x, int y)
{
  if (powl (2.0, x) != exp2l (x))
    link_error ();

#if HAVE_EXP10
  if (powl (10.0, x) != exp10l (x))
    link_error ();
#endif

  if (powl (2.0, y) != ldexpl (1.0, y))
    link_error ();
}

int main ()
{
  test(0, 0);
  testf(0, 0);
  testl(0, 0);
  return 0;
}

