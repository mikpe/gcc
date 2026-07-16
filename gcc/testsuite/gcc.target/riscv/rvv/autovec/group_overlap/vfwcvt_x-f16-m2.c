/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vfloat16m2_t,
  vint32m4_t,
  __riscv_vle16_v_f16m2,
  __riscv_vfwcvt_x_f_v_i32m4,
  __riscv_vse32_v_i32m4,
  vfwcvt_x,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v0,v2([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.x\.f\.v\s+v4,v6} 1 } } */
