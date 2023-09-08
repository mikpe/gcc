void
bar (int N)
{
  int foo (char a[2][++N]) { N += 4; return sizeof (a[0]); }
#ifdef __PDP10__
  if (foo (0) != 4)
#else
  if (foo (0) != 2)
#endif
    abort ();
#ifdef __PDP10__
  if (foo (0) != 8)
#else
  if (foo (0) != 7)
#endif
    abort ();
  if (N != 11)
    abort ();
}

int
main()
{
  bar (1);
  exit (0);
}
