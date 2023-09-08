extern void exit (int);
extern void abort (void);
extern unsigned short f (short a) __attribute__((__noinline__));

unsigned short
f (short a)
{
  short b;

  if (a > 0)
    return 0;
#ifdef __PDP10__
  b = ((int) a) + - (int) 131072;
#else
  b = ((int) a) + - (int) 32768;
#endif
  return b;
}

int
main (void)
{
  if (sizeof (short) < 2
      || sizeof (short) >= sizeof (int))
    exit (0);

#ifdef __PDP10__
  if (f (-131071) != 1)
#else
  if (f (-32767) != 1)
#endif
    abort ();

  exit (0);
}
