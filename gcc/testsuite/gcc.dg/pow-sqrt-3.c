/* { dg-do run } */
/* { dg-options "-O2 -ffast-math --param max-pow-sqrt-depth=3" } */
/* { dg-require-effective-target double64plus } */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

#define EXPN (1.25)
#include "pow-sqrt.x"
