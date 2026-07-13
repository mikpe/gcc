/* { dg-do compile } */
/* { dg-options "-O1" } */

#include "pr110533-2a.c"

/* { dg-final { scan-assembler-not "mov" } } */
