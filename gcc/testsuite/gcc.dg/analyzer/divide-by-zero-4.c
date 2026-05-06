/* { dg-additional-options "-fno-analyzer-state-merge" } */

int
get_zero (void)
{
  return 0; /* { dg-message "\\\(6\\\) zero value originates here" } */
}

struct foo { int x; int y; };

void
init_foo (struct foo *f, int x, int y) /* { dg-message "\\\(9\\\) entry to 'init_foo' with zero from \\\(7\\\) for 'y'" } */
{
  f->x = x;
  f->y = y; /* { dg-message "\\\(10\\\) copying zero value from \\\(9\\\) from 'y' to '\\*f\\.y'" } */
}

int
do_divide (struct foo *f)
{
  return f->x / f->y; /* { dg-message "using zero value from \\\(10\\\) from '\\*f\\.y'" } */
  /* { dg-warning "division by zero" "" { target *-*-* } .-1 } */
}

int
test (int flag, int flag_2, int flag_3)
{
  struct foo f;
  int a = 42;
  int b = 10;
  if (flag)
    b = get_zero ();
    /* { dg-message "\\\(7\\\) returning zero from \\\(6\\\) from 'get_zero' here" "" { target *-*-* } .-1 } */
  init_foo (&f, a, b); /* { dg-message "passing zero from \\\(7\\\) from 'test' to 'init_foo' via parameter 3" } */
  return do_divide (&f);
}
