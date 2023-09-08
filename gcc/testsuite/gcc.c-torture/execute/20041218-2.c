extern void abort (void);

int test(int n)
{
  struct s { char b[n]; } __attribute__((packed));
  n++;
  return sizeof(struct s);
}

int main()
{
#ifdef __PDP10__
  if (test(123) != 124)
#else
  if (test(123) != 123)
#endif
    abort ();
  return 0;
}
