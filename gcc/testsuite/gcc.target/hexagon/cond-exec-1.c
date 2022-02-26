/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-skip-if "Require >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

/* { dg-final { scan-assembler "if.*\\.new.*sxth" } } */
int
test_sxth (int a, int b)
{
  if (a)
    b = (short) b;
  return b + a;
}

/* { dg-final { scan-assembler "if.*\\.new.*sxtb" } } */
int
test_sxtb (int a, int b)
{
  if (a)
    b = (char) b;
  return b + a;
}

/* { dg-final { scan-assembler "if.*\\.new.*zxth" } } */
int
test_zxth (int a, int b)
{
  if (a)
    b = (unsigned short) b;
  return b + a;
}

/* { dg-final { scan-assembler "if.*\\.new.*zxtb" } } */
int
test_zxtb (int a, int b)
{
  if (a)
    b = (unsigned char) b;
  return b + a;
}

/* { dg-final { scan-assembler "if.*\\.new.*asrh" } } */
int
test_asrh (int a, int b)
{
  if (a)
    b >>= 16;
  return b + a;
}

/* { dg-final { scan-assembler "if.*\\.new.*aslh" } } */
int
test_aslh (int a, int b)
{
  if (a)
    b <<= 16;
  return b + a;
}
