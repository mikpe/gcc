/* { dg-do compile } */
/* { dg-options "-O2" } */

volatile int *v;

int
foo (void)
{
  int i, j;
  for (i = 0; i < 100; i++)
    {
      *v = i;
      for (j = 0; j < 100; j++)
        *v = j;
    }
}

/* { dg-final { scan-assembler-times "loop0\\(\.\*#100\\)" 1 } } */
/* { dg-final { scan-assembler-times ":endloop0" 1 } } */
/* { dg-final { scan-assembler-times "loop1\\(\.\*#100\\)" 1 } } */
/* { dg-final { scan-assembler-times ":endloop1" 1 } } */
