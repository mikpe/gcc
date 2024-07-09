/* Test structures passed by value, including to a function with a
   variable-length argument lists.  All struct members are float
   scalars.  */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

extern void struct_by_value_5a_x (void);
extern void exit (int);
int fails;

int
main ()
{
  struct_by_value_5a_x ();
  exit (0);
}
