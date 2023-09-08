f(got)
{
#ifdef __PDP10__
    if (got != 0x3ffff)
#else
    if (got != 0xffff)
#endif
        abort();
}

main()
{
    signed char c = -1;
    unsigned u = (unsigned short)c;
    f(u);
    exit(0);
}
