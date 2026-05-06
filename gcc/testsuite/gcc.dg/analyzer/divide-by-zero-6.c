/* { dg-additional-options "-fno-analyzer-state-merge" } */

/* TODO: we shouldn't need this:  */
/* { dg-additional-options "-fno-analyzer-state-purge" } */

struct foo { int x; int y; };

void
init_foo (struct foo *f)
{
  __builtin_memset (f, 0, sizeof (f));
}

int
do_divide (struct foo *f)
{
  return f->x / f->y; /* { dg-message "using zero value from '\\*f\\.y'" } */
  /* { dg-warning "division by zero" "" { target *-*-* } .-1 } */
}

int
test (int flag, int flag_2, int flag_3)
{
  struct foo f;
  init_foo (&f);
  return do_divide (&f);
}
