x(const char *s)
{
    char a[1];
    const char *ss = s;
    a[*s++] |= 1;
#ifdef __PDP10__
    return ss + 1 == s;
#else
    return (int) ss + 1 == (int)s;
#endif
}

main()
{
    if(x("") != 1)
        abort();
    exit(0);
}
