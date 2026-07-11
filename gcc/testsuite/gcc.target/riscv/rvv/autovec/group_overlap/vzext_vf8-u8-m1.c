/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vuint8m1_t,
  vuint64m8_t,
  __riscv_vle8_v_u8m1,
  __riscv_vzext_vf8_u64m8,
  __riscv_vse64_v_u64m8,
  vzext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vzext\.vf8\s+v0,v7([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vzext\.vf8\s+v8,v15} 1 } } */
