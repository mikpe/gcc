/* { dg-do compile } */
/* { dg-options "-O2 -march=rv64gcv -mabi=lp64d -mrvv-vector-bits=zvl" } */

#include <riscv_vector.h>

vfloat32m1_t
foo (float *a)
{
    vfloat32m1_t a0 = __riscv_vle32_v_f32m1 (a, 4);
    return a0;
}

/* { dg-final { scan-assembler-not "vle32" } } */
/* { dg-final { scan-assembler-times "vl1re32.v" 1 } } */
