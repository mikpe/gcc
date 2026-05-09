/* { dg-do compile } */
/* { dg-options "-O1 -march=x86-64" } */

#include <stdint.h>

typedef int16_t v2i16 __attribute__((vector_size(4)));
int16_t g5;
_Bool g26;
void f15(v2i16, int16_t, void *);
void
f3(void)
{
  v2i16 vec6;
  goto lbl_br7;
lbl_br6:
  f15(vec6, 0, 0);
lbl_br7:
  vec6[0] = 0;
lbl_br10:
  g5 = vec6[0];
  vec6[1] = g5;
  if (g26) goto lbl_br6;
  g26 = 7;
  vec6[0] = 0;
  goto lbl_br10;
}
