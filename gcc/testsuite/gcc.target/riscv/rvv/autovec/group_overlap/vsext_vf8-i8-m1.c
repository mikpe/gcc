/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8m1_t,
  vint64m8_t,
  __riscv_vle8_v_i8m1,
  __riscv_vsext_vf8_i64m8,
  __riscv_vse64_v_i64m8,
  vsext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vsext\.vf8\s+v0,v7([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf8\s+v8,v15} 1 } } */
