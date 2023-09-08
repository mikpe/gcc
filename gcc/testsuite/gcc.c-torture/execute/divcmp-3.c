extern void abort(void);

int test1(char x)
{
  return x/100 == 6;
}

int test1u(unsigned char x)
{
  return x/100 == 6;
}

int test2(char x)
{
  return x/100 != 6;
}

int test2u(unsigned char x)
{
  return x/100 != 6;
}

int test3(char x)
{
  return x/100 < 6;
}

int test3u(unsigned char x)
{
  return x/100 < 6;
}

int test4(char x)
{
  return x/100 <= 6;
}

int test4u(unsigned char x)
{
  return x/100 <= 6;
}

int test5(char x)
{
  return x/100 > 6;
}

int test5u(unsigned char x)
{
  return x/100 > 6;
}

int test6(char x)
{
  return x/100 >= 6;
}

int test6u(unsigned char x)
{
  return x/100 >= 6;
}


int main()
{
  int c;

  for (c=-256; c<512; c++)
  {
    if (test1(c) != 0)
      abort ();
    if (test1u(c) != 0)
      abort ();
    if (test2(c) != 1)
      abort ();
    if (test2u(c) != 1)
      abort ();
    if (test3(c) != 1)
      abort ();
    if (test3u(c) != 1)
      abort ();
    if (test4(c) != 1)
      abort ();
    if (test4u(c) != 1)
      abort ();
    if (test5(c) != 0)
      abort ();
    if (test5u(c) != 0)
      abort ();
    if (test6(c) != 0)
      abort ();
    if (test6u(c) != 0)
      abort ();
  }
  return 0;
}

