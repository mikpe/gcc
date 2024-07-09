/* Test _Float32x complex arithmetic.  */
/* { dg-do run } */
/* { dg-options "" } */
/* { dg-add-options float32x } */
/* { dg-require-effective-target float32x_runtime } */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

#define WIDTH 32
#define EXT 1
#include "floatn-complex.h"
