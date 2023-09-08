short optab[5];
char buf[10];
execute (ip)
     register short *ip;
{
#ifndef NO_LABEL_VALUES
  register void *base = &&x;
  char *bp = buf;
  static void *tab[] = {&&x, &&y, &&z};
  if (ip == 0)
    {
      int i;
      for (i = 0; i < 3; ++i)
#ifdef __PDP10__
		optab[i] = (short)((int *)tab[i] - (int *)base);
#else
		optab[i] = (short)(tab[i] - base);
#endif
      return;
    }
x:  *bp++='x';
#ifdef __PDP10__
  goto *(void *)((int *)base + *ip++);
#else
  goto *(base + *ip++);
#endif
y:  *bp++='y';
#ifdef __PDP10__
  goto *(void *)((int *)base + *ip++);
#else
  goto *(base + *ip++);
#endif
z:  *bp++='z';
    *bp=0;
    return;
#else
    strcpy (buf, "xyxyz");
#endif
}

short p[5];

main ()
{
  execute ((short *) 0);
  p[0] = optab[1];
  p[1] = optab[0];
  p[2] = optab[1];
  p[3] = optab[2];
  execute (&p);
  if (strcmp (buf, "xyxyz"))
    abort ();
  exit (0);
}
