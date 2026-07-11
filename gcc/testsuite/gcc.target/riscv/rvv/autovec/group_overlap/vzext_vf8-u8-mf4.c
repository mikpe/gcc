/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vuint8mf4_t,
  vuint64m2_t,
  __riscv_vle8_v_u8mf4,
  __riscv_vzext_vf8_u64m2,
  __riscv_vse64_v_u64m2,
  vzext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vzext\.vf8\s+v0,v7([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vzext\.vf8\s+v8,v15} } } */
/* { dg-final { scan-assembler-not {vzext\.vf8\s+v16,v23} } } */
/* { dg-final { scan-assembler-not {vzext\.vf8\s+v24,v31} } } */
