int
foo1 (value)
     long long value;
{
#ifdef __PDP10__
  register const long long constant = 0x600000000800000000LL;
#else
  register const long long constant = 0xc000000080000000LL;
#endif

  if (value < constant)
    return 1;
  else
    return 2;
}

int
foo2 (value)
     unsigned long long value;
{
#ifdef __PDP10__
  register const unsigned long long constant = 0x600000000800000000LL;
#else
  register const unsigned long long constant = 0xc000000080000000LL;
#endif

  if (value < constant)
    return 1;
  else
    return 2;
}

main ()
{
#ifdef __PDP10__
  unsigned long long value = 0x600000000000000001LL;
#else
  unsigned long long value = 0xc000000000000001LL;
#endif
  int x, y;

  x = foo1 (value);
  y = foo2 (value);
  if (x != y || x != 1)
    abort ();
  exit (0);
}
