/* PR middle-end/106823 */
/* { dg-lto-do link } */
__attribute__((__warning__("w"))) void f (int) {}
void g (int i)
{
  /* It would be nice to also confirm that the warning is issued in the
     absence of the pragma, but the LTO testsuite is not currently set up to
     test for warnings emitted during the compile stage.  But pr80922
     testcases do test this functionality.  */
  #pragma GCC diagnostic ignored "-Wattribute-warning"
  f (i);
}

int main ()
{
  g (1);
}
