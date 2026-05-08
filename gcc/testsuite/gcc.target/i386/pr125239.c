/* { dg-do compile } */
/* { dg-options "-O2 -march=x86-64 -fno-tree-dse -fno-tree-dce" } */

extern void a (void);
void
b (void)
{
  int *c, *d, *e[2][20] = {{c}, {c, d, d, d, c, c, d, c}};
  a ();
}
