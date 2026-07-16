/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vfloat16m2_t,
  vuint32m4_t,
  __riscv_vle16_v_f16m2,
  __riscv_vfwcvt_xu_f_v_u32m4,
  __riscv_vse32_v_u32m4,
  vfwcvt_xu,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vfwcvt\.xu\.f\.v\s+v0,v2([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.xu\.f\.v\s+v4,v6} 1 } } */
