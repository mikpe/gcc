void foo (unsigned int * p)
{
#ifdef __PDP10__
  if ((signed char)(*p & 0x1FF) == 49)
  /* The second expression right of the logical OR in the original
     source did not make sense to me, so it was eliminated in the
     PDP10 port.
     MdB, XKL LLC, 02/22/2006 */
#else
  if ((signed char)(*p & 0xFF) == 17 || (signed char)(*p & 0xFF) == 18)
#endif
    return;
  else
    abort ();
}

int main ()
{
#ifdef __PDP10__
  int i = 0x30031;
#else
  int i = 0x30011;
#endif
  foo(&i);
  exit (0);
}
