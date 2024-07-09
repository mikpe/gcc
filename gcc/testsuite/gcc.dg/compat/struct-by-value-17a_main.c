/* Test structures passed by value, including to a function with a
   variable-length argument lists.  All struct members are of type
   _Complex double.  */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

extern void struct_by_value_17a_x (void);
extern void exit (int);
int fails;

int
main ()
{
  struct_by_value_17a_x ();
  exit (0);
}
