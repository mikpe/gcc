/* The bit-field below would have a problem if __INT_MAX__ is too
   small.  */
#if __INT_MAX__ < 2147483647
int
main (void)
{
  exit (0);
}
#else
/*
CONF:m68k-sun-sunos4.1.1
OPTIONS:-O
*/
struct T
{
#ifdef __PDP10__
    unsigned i:9;
    unsigned c:27;
#else
    unsigned i:8;
    unsigned c:24;
#endif
};

f(struct T t)
{
    struct T s[1];
    s[0]=t;
    return(char)s->c;
}

main()
{
    struct T t;
#ifdef __PDP10__
    t.i=0x1ff;
    t.c=0x7ffff11;
    if(f(t)!=0x111)
#else
    t.i=0xff;
    t.c=0xffff11;
    if(f(t)!=0x11)
#endif
        abort();
    exit(0);
}
#endif
