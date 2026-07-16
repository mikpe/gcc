/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvfh -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m4,
  vfloat16m4_t,
  vuint32m8_t,
  __riscv_vle16_v_f16m4,
  __riscv_vfwcvt_xu_f_v_u32m8,
  __riscv_vse32_v_u32m8,
  vfwcvt_xu,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vfwcvt\.xu\.f\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vfwcvt\.xu\.f\.v\s+v8,v12} 1 } } */
