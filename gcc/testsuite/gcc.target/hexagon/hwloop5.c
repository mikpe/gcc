/* { dg-do compile } */
/* { dg-options "-O2" } */

volatile unsigned int *v;

int
foo (unsigned int i)
{
  /* We can't prove that this won't loop 2^32 times (which loopN/endloopN
     constructs do not support).  Make sure loopN/endloopN are not generated
     when -funsafe-loop-optimizations is not used.  */
  do
    {
      *v = i;
      i++;
    }
  while (i != 0);
}

/* { dg-final { scan-assembler-not "loop0\\(" } } */
/* { dg-final { scan-assembler-not ":endloop0" } } */
