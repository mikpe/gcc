/*  On H8, the predicate general_operand_src(op,mode) used to ignore
    mode when op is a (mem (post_inc ...)).  As a result, the pattern
    for extendhisi2 was recognized as extendqisi2.  */

extern void abort ();
extern void exit (int);

short *q;

long
foo (short *p)
{
  long b = *p;
  q = p + 1;
  return b;
}

int
main ()
{
#ifdef __PDP10__
  short a = 0x3ff00;
  if (foo (&a) != (long) (short) 0x3ff00)
#else
  short a = 0xff00;
  if (foo (&a) != (long) (short) 0xff00)
#endif
    abort ();
  exit (0);
}
