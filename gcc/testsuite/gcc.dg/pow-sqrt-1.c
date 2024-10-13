/* { dg-do run } */
/* { dg-options "-O2 -ffast-math --param max-pow-sqrt-depth=5" } */
/* { dg-require-effective-target double64plus } */
/* { dg-skip-if "too large" { cdp1802-*-* } } */

#define EXPN (-6 * (0.5*0.5*0.5*0.5))

#include "pow-sqrt.x"
