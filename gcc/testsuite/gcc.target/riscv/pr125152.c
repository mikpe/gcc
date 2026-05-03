/* { dg-do compile } */
/* { dg-additional-options "-mmovcc -std=gnu99" } */

typedef __attribute__((__vector_size__(16))) char V;
V v;
char c;

void
foo()
{
  v |= (V){4} > c;
}
