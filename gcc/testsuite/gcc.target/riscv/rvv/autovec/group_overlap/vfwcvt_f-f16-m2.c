/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vfloat16m2_t,
  vfloat32m4_t,
  __riscv_vle16_v_f16m2,
  __riscv_vfwcvt_f_f_v_f32m4,
  __riscv_vse32_v_f32m4,
  vfwcvt_f,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vfwcvt\.f\.f\.v\s+v0,v2([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.f\.f\.v\s+v4,v6} 1 } } */
