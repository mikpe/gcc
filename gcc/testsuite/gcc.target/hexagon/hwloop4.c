/* { dg-do compile } */
/* { dg-options "-O2" } */

volatile unsigned int *v;

int
foo (void)
{
  unsigned int i;
  for (i = 0; i < 0xffffffffu; i++)
    *v = i;
}

/* { dg-final { scan-assembler-times "loop0\\(\[^,\]\*,r\.\*\\)" 1 } } */
/* { dg-final { scan-assembler-times ":endloop0" 1 } } */
