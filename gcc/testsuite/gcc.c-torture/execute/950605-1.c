f (c)
    unsigned char c;
{
#ifdef __PDP10__
  if (c != 0x1FF)
#else
  if (c != 0xFF)
#endif
    abort ();
}

main ()
{
  f (-1);
  exit (0);
}
