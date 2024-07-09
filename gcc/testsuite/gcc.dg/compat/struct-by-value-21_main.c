/* Test function argument passing.  This was written when correcting
   a deviation from the ABI on SPARC64 between 3.3 and 3.4.  */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

extern void struct_by_value_21_x (void);
extern void exit (int);
int fails;

int
main ()
{
  struct_by_value_21_x ();
  exit (0);
}
