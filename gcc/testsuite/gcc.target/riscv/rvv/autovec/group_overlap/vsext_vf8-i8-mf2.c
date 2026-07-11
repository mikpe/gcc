/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8mf2_t,
  vint64m4_t,
  __riscv_vle8_v_i8mf2,
  __riscv_vsext_vf8_i64m4,
  __riscv_vse64_v_i64m4,
  vsext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-not {vsext\.vf8\s+v0,v7([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vsext\.vf8\s+v8,v15} } } */
/* { dg-final { scan-assembler-not {vsext\.vf8\s+v16,v23} } } */
/* { dg-final { scan-assembler-not {vsext\.vf8\s+v24,v31} } } */
