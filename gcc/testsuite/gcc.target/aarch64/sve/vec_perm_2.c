/* { dg-options "-O2 -msve-vector-bits=2048" } */
/* { dg-final { check-function-bodies "**" "" } } */

#include <arm_sve.h>

/*
** test8:
**	tbl	z0\.b, {z0\.b}, z2\.b
**	ret
*/
svint8_t
test8 (svint8_t x, svint8_t y, svint8_t z)
{
  return __builtin_shuffle (x, y, z);
}

svint16_t
test16 (svint16_t x, svint16_t y, svint16_t z)
{
  return __builtin_shuffle (x, y, z);
}

/* { dg-final { scan-assembler-times {\tand\t} 1 } } */
/* { dg-final { scan-assembler-times {\tsub\t} 1 } } */
/* { dg-final { scan-assembler-times {\ttbl\t} 3 } } */
/* { dg-final { scan-assembler-times {\torr\t} 1 } } */
