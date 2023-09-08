struct s
{
  int i[18];
  char f;
  char b[2];
};

struct s s1;

int
main()
{
  struct s s2;
  s2.b[0] = 100;
#ifdef __PDP10__
  __builtin_memcpy(&s2, &s1, ((&((struct s *)0)->b[0]) - ((char *)(int *)0)));
#else
  __builtin_memcpy(&s2, &s1, ((unsigned int) &((struct s *)0)->b));
#endif
  if (s2.b[0] != 100)
    abort();
  exit(0);
}
