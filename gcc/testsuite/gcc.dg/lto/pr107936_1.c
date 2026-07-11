void
foo (char x[2])
{
  asm volatile ("" : : "r" (&x[0]) : "memory");
}
