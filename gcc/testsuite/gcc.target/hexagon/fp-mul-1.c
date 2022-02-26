/* { dg-do compile } */
/* { dg-options "-O1" } */
/* { dg-skip-if "fp requires >= hexagonv5" { "*-*-*" } { "*" } { "-march=hexagonv5" } } */

/* { dg-final { scan-assembler "= sfmpy" } } */
float
f_mul (float a, float b)
{
  return a * b;
}

/* { dg-final { scan-assembler "= dfmpy" } } */
double
d_mul (double a, double b)
{
  return a * b;
}

/* { dg-final { scan-assembler "\\+= sfmpy" } } */
float
f_muladd (float a, float b, float c)
{
  return a + b * c;
}

/* { dg-final { scan-assembler "\\+= dfmpy" } } */
double
d_muladd (double a, double b, double c)
{
  return a + b * c;
}

/* { dg-final { scan-assembler "-= sfmpy" } } */
float
f_mulsub (float a, float b, float c)
{
  return a - b * c;
}

/* { dg-final { scan-assembler "-= dfmpy" } } */
double
d_mulsub (double a, double b, double c)
{
  return a - b * c;
}
