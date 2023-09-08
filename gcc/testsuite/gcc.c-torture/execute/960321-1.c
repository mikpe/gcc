char a[10] = "deadbeef";

#ifdef __PDP10__

char
acc_a (long i)
{
  return a[i-0x773594000L];
}

char
acc_b (long long i)
{
  return a[i-0x173594000000000000LL];
}

main ()
{
  /* 36-bit long test case */
  if (acc_a (0x773594000L) != 'd')
    abort ();
  /* 71-bit long long test case */
  if (acc_a (0x173594000000000000LL) != 'd')
    abort ();
  exit (0);
}

#else
char
acc_a (long i)
{
  return a[i-2000000000L];
}

main ()
{
  if (acc_a (2000000000L) != 'd')
    abort ();
  exit (0);
}

#endif
