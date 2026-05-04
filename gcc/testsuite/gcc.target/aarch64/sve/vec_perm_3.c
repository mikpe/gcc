/* { dg-options "-O2 -msve-vector-bits=1024" } */

#include <arm_sve.h>

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

/* test8 does not need an AND.  In principle, its subtraction of 128
   from the selector can be rendered as a SUB, an ADD, or an EOR.  */
/* { dg-final { scan-assembler-times {\tand\t} 1 } } */
/* { dg-final { scan-assembler-times {\t(?:sub|add|eor)\t} 2 } } */
/* { dg-final { scan-assembler-times {\ttbl\t} 4 } } */
/* { dg-final { scan-assembler-times {\torr\t} 2 } } */
