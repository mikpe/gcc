/* { dg-do compile } */
/* { dg-options "-O2 -march=x86-64" } */

extern void a(int[]);
int b;
int d(int e, volatile int f) {
  b = f - e;
  int t[2] = {b, b};
  a(t);
}
void g(int h[1]) {
  if (d(0, 1))
    h[0] = 0;
  d(0, 1);
}

/* { dg-final { scan-assembler-times "movabsq\[ \\t\]+\\\$4294967297, %r\[a-z0-9\]+" 2 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-not "xmm" { target { ! ia32 } } } } */
