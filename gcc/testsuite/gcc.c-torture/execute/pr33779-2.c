int foo(int i)
{
  return ((int)((unsigned)(i + 1) * 4)) / 4;
}

extern void abort(void);
int main()
{
#ifdef __PDP10__
  if (foo(0x3ffffffff) != 0)
#else
  if (foo(0x3fffffff) != 0)
#endif
    abort ();
  return 0;
}
