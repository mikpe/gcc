/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e32m1,
  vuint32m1_t,
  vuint64m2_t,
  __riscv_vle32_v_u32m1,
  __riscv_vwcvtu_x_x_v_u64m2,
  __riscv_vse64_v_u64m2,
  vwcvtu,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v0,v1([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v2,v3([^0-9]|$)} 1 } } */
