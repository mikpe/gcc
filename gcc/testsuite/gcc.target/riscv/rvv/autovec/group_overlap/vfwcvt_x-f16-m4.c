/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m4,
  vfloat16m4_t,
  vint32m8_t,
  __riscv_vle16_v_f16m4,
  __riscv_vfwcvt_x_f_v_i32m8,
  __riscv_vse32_v_i32m8,
  vfwcvt_x,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v8,v12} 1 } } */
