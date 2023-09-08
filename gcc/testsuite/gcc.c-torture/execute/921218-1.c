f()
{
#ifdef __PDP10__
  return (unsigned char)("\777"[0]);
#else
  return (unsigned char)("\377"[0]);
#endif
}

main()
{
#ifdef __PDP10__
  if (f() != (unsigned char)(0777))
#else
  if (f() != (unsigned char)(0377))
#endif
    abort();
  exit (0);
}
