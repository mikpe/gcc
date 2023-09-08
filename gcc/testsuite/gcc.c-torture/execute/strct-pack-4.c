typedef struct
{
  unsigned char a __attribute__((packed));
  unsigned short b __attribute__((packed));
} three_char_t;

unsigned char
my_set_a (void)
{
#ifdef __PDP10__
  return 0x157;
#else
  return 0xab;
#endif
}

unsigned short
my_set_b (void)
{
#ifdef __PDP10__
  return 0x21234;
#else
  return 0x1234;
#endif
}

main ()
{
  three_char_t three_char;

  three_char.a = my_set_a ();
  three_char.b = my_set_b ();
#ifdef __PDP10__
  if (three_char.a != 0x157 || three_char.b != 0x21234)
#else
  if (three_char.a != 0xab || three_char.b != 0x1234)
#endif
    abort ();
  exit (0);
}

