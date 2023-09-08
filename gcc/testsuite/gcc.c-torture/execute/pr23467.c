struct s1
{
#ifdef __PDP10__
  int __attribute__ ((aligned (32))) a;
#else
  int __attribute__ ((aligned (8))) a;
#endif
};

struct
{
  char c;
  struct s1 m;
} v;

int
main (void)
{
  if ((int)&v.m & 7)
    abort ();
  exit (0);
}
