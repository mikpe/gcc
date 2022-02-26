/* Check .comm decls include alignment information. */

/* { dg-do compile { target { ! *-*-linux* } } } */
/* { dg-skip-if "-G0 disables small data sections" { "*-*-*" } { "-G0" } { "" } } */

/* { dg-final { scan-assembler ".comm\tbar,4,4,4" } } */
long bar;
long bar;

/* { dg-final { scan-assembler ".lcomm\tfoo,4,4,4" } } */
static long foo;
static long foo;

long
get_foo ()
{
  return foo;
}

void
set_foo (long newfoo)
{
  foo = newfoo;
}
