/* PR lto/107936 */
/* { dg-lto-do link } */
/* { dg-lto-additional-options "-Wstringop-overflow" } */
void foo (char x[2]);

int
main ()
{
  char x[2];
  /* It would be nice to also confirm that the warning is issued in the
     absence of the pragma, but the LTO testsuite is not currently set up to
     test for warnings emitted during the compile stage.  But pr80922
     testcases do test this functionality.  */
  #pragma GCC diagnostic ignored "-Wstringop-overflow"
  foo (x + 1);
}
