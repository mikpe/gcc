/* { dg-do compile } */
/* { dg-options "-O2" } */

volatile int *v;

int
foo (void)
{
  int i;
  for (i = 0; i < 100; i++)
    *v = i;
}

/* { dg-final { scan-assembler-times "loop0\\(\.\*#100\\)" 1 } } */
/* { dg-final { scan-assembler-times ":endloop0" 1 } } */
