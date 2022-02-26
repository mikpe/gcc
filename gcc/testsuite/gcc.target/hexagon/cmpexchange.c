/* { dg-do run } */
/* { dg-options "-O2" } */

#include "../../../config/hexagon/linux-atomic.c"

extern void abort (void);

int main ()
{
  int a = 12;
  int oldval = 11;
  int newval = 13;
  int res = __kernel_cmpxchg (oldval, newval, &a);
  if (res != 1)
    abort ();
  return 0;
}
