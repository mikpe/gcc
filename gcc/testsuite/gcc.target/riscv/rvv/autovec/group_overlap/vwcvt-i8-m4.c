/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m4,
  vint8m4_t,
  vint16m8_t,
  __riscv_vle8_v_i8m4,
  __riscv_vwcvt_x_x_v_i16m8,
  __riscv_vse16_v_i16m8,
  vwcvt,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vwcvt\.x\.x\.v\s+v0,v4} 1 } } */
/* { dg-final { scan-assembler-times {vwcvt\.x\.x\.v\s+v8,v12} 1 } } */
