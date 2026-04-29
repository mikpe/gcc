/* { dg-do compile } */
/* { dg-options "-mgfni -mavx512vl -mavx512bw -mavx512f -O3 -Wno-shift-count-negative -mtune=generic -march=x86-64 --param vect-epilogues-nomask=0" } */
/* { dg-final { scan-assembler-times "vgf2p8affineqb" 27 } } */

#include "shift-gf2p8affine-2.c"
