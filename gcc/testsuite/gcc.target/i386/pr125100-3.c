/* { dg-do compile { target fpic } } */
/* { dg-options "-mtune=generic -O2 -fPIC" } */
/* { dg-additional-options "-march=pentiumpro" { target ia32 } } */

struct desc {
  char c1;
  char c2;
};
void
foo (struct desc *list, int n, int l)
{
  int j;
  for (j = 0; j < l; j++)
    list[j].c1 = list[j].c2 = 1;
}

/* { dg-final { scan-assembler-times "movl\[ \\t\]+\\\$257, %\[a-z0-9\]+" 1 } } */
/* { dg-final { scan-assembler-not "__x86.get_pc_thunk" { target ia32 } } } */
