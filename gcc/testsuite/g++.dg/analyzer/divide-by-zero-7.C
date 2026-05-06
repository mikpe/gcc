/* { dg-additional-options "-fno-analyzer-state-merge" } */

// TODO: we shouldn't need this:
/* { dg-additional-options "-fno-analyzer-state-purge" } */

struct foo
{
  foo (int x_, int y_)
  : x (x_), y (y_) // TODO: should show event here
  {
  }

  int divide () const
  {
    return x / y; /* { dg-message "using zero value from '\\*this\\.foo::y'" } */
    /* { dg-warning "division by zero" "" { target *-*-* } .-1 } */
  }

  int x;
  int y;
};

int
test ()
{
  foo f (5, 0); // TODO: should show "origin of zero" event here
  return f.divide ();
}
