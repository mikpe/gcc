/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2" } */

int foo(int a) {
  long long t = a;
  return t >> 4;
}

/* { dg-final { scan-assembler-not "movslq" } } */
/* { dg-final { scan-assembler-not "sarq" } } */
