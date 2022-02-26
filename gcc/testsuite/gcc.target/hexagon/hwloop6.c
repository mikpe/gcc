/* { dg-do compile } */
/* { dg-options "-O2 -funsafe-loop-optimizations" } */

volatile unsigned int *v;

int
foo (unsigned int i)
{
  /* We can't prove that this won't execute 2^32 times (which loopN/endloopN
     constructs do not support), so -funsafe-loop-optimizations must be used
     in order to generate hardware loops in this case.  */
  do
    {
      *v = i;
      i++;
    }
  while (i != 0);
}

/* { dg-final { scan-assembler-times "loop0\\(\[^r\]\*r\.\*\\)" 1 } } */
/* { dg-final { scan-assembler-times ":endloop0" 1 } } */
