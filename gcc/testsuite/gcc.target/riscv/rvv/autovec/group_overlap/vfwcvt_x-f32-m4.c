/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e32m4,
  vfloat32m4_t,
  vint64m8_t,
  __riscv_vle32_v_f32m4,
  __riscv_vfwcvt_x_f_v_i64m8,
  __riscv_vse64_v_i64m8,
  vfwcvt_x,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v8,v12} 1 } } */
