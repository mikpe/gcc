/* Verify we don't ICE on the following test case.  */

/* { dg-do compile } */
/* { dg-require-effective-target tls_native } */
/* { dg-options "-O -fpic -mtls-dialect=desc -march=rv64gcv -mabi=lp64d" } */
/* { dg-require-effective-target fpic } */

extern __thread int x;
extern int bar(int);

int f(int id) {
  x = id * 1000;
  bar(0);
  if (x != id * 1000) return bar(x);
  return 0;
}
