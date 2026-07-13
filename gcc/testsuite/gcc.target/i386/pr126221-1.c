/* PR middle-end/126221 */
/* { dg-do compile } */
/* { dg-options "-O2 -march=x86-64-v4" } */

int a[5];
int
foo (int t)
{
  for (int c = 0; c < 5; c++)
    if (a[c] && c == 0)
      return 0;
  return t;
}

