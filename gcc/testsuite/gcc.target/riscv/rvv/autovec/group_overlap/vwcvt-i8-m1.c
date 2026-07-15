/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8m1_t,
  vint16m2_t,
  __riscv_vle8_v_i8m1,
  __riscv_vwcvt_x_x_v_i16m2,
  __riscv_vse16_v_i16m2,
  vwcvt,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-times {vwcvt\.x\.x\.v\s+v0,v1([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vwcvt\.x\.x\.v\s+v2,v3([^0-9]|$)} 1 } } */
