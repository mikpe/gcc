/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8mf2_t,
  vint16m1_t,
  __riscv_vle8_v_i8mf2,
  __riscv_vwcvt_x_x_v_i16m1,
  __riscv_vse16_v_i16m1,
  vwcvt,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v0,v1([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v2,v3([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v4,v5} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v6,v7} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v8,v9} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v10,v11} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v12,v13} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v14,v15} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v16,v17} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v18,v19} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v20,v21} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v22,v23} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v24,v25} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v26,v27} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v28,v29} } } */
/* { dg-final { scan-assembler-not {vwcvt\.x\.x\.v\s+v30,v31} } } */
