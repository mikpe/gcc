main ()
{
  int tmp;
  unsigned long long utmp1, utmp2;
#ifdef __PDP10__
  unsigned long long utmp3;
#endif

  tmp = 16;

  utmp1 = (~((unsigned long long) 0)) >> tmp;
  utmp2 = (~((unsigned long long) 0)) >> 16;

  if (utmp1 != utmp2)
    abort();

#ifdef __PDP10__
  /* Added 71-bit ULL specific test */
  /* MdB, XKL LLC, 02/23/2006       */
  utmp3 = (~((unsigned long long) 0)) & 0x00007FFFFFFFFFFFFFULL;
  if (utmp1 != utmp3 || utmp2 != utmp3)
    abort();
#endif

  exit (0);
}

