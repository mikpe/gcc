typedef struct
{
  unsigned char a __attribute__ ((packed));
  unsigned short b __attribute__ ((packed));
} three_byte_t;

unsigned char
f (void)
{
#ifdef __PDP10__
  return 0x157;
#else
  return 0xab;
#endif
}

unsigned short
g (void)
{
#ifdef __PDP10__
  return 0x21234;
#else
  return 0x1234;
#endif
}

main ()
{
  three_byte_t three_byte;

  three_byte.a = f ();
  three_byte.b = g ();
#ifdef __PDP10__
  if (three_byte.a != 0x157 || three_byte.b != 0x21234)
#else
  if (three_byte.a != 0xab || three_byte.b != 0x1234)
#endif
    abort ();
  exit (0);
}

