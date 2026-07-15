/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e32m4,
  vuint32m4_t,
  vuint64m8_t,
  __riscv_vle32_v_u32m4,
  __riscv_vwcvtu_x_x_v_u64m8,
  __riscv_vse64_v_u64m8,
  vwcvtu,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v8,v12} 1 } } */
