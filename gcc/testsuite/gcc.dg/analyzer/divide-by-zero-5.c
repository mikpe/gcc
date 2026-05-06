/* { dg-additional-options "-fno-analyzer-state-merge" } */

/* TODO: we shouldn't need this:  */
/* { dg-additional-options "-fno-analyzer-state-purge" } */

int
maybe_get_zero (int flag)
{
  if (flag)
    return 0; /* { dg-message "zero value originates here" } */
  else
    return 42;
}

struct foo { int x; int y; };

void
init_foo (struct foo *f, int x, int y) /* { dg-message "\\\(11\\\) entry to 'init_foo' with zero from \\\(9\\\) for 'y'" } */
{
  f->x = x;
  f->y = y; /* { dg-message "\\\(12\\\) copying zero value from \\\(11\\\) from 'y' to '\\*f\\.y'" } */
}

int
do_divide (struct foo *f)
{
  return f->x / f->y; /* { dg-message "using zero value from \\\(12\\\) from '\\*f\\.y'" } */
  /* { dg-warning "division by zero" "" { target *-*-* } .-1 } */
}

int
test (int flag, int flag_2, int flag_3)
{
  struct foo f;
  int a = 42;
  int b = 10;
  if (flag)
    b = maybe_get_zero (flag_2); /* { dg-bogus "value of 'b' unchanged here" } */
    /* { dg-message "returning zero from \\\(8\\\) from 'maybe_get_zero' here" "" { target *-*-* } .-1 } */
  init_foo (&f, a, b); /* { dg-message "passing zero from \\\(9\\\) from 'test' to 'init_foo' via parameter 3" } */
  return do_divide (&f);
}
