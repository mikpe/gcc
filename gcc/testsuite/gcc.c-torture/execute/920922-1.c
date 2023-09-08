unsigned long*
f(p)
  unsigned long*p;
{
#ifdef __PDP10__
  unsigned long a = (*p++) >> 28;
#else
  unsigned long a = (*p++) >> 24;
#endif
  return p + a;
}

main ()
{
#ifdef __PDP10__
  unsigned long x = 0x800000000UL;
#else
  unsigned long x = 0x80000000UL;
#endif
  if (f(&x) != &x + 0x81)
    abort();
  exit(0);
}
