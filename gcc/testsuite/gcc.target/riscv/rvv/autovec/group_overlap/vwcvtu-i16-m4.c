/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m4,
  vuint16m4_t,
  vuint32m8_t,
  __riscv_vle16_v_u16m4,
  __riscv_vwcvtu_x_x_v_u32m8,
  __riscv_vse32_v_u32m8,
  vwcvtu,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vwcvtu\.x\.x\.v\s+v8,v12} 1 } } */
