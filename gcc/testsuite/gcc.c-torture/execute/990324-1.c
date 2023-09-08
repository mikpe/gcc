void f(long i)
{
  if ((signed char)i < 0 || (signed char)i == 0) 
    abort ();
  else
    exit (0);
}

main()
{
#ifdef __PDP10__
  f(0xffffffe01);
#else
  f(0xffffff01);
#endif
}

