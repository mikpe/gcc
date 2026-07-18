/* { dg-do compile } */
/* { dg-options "-march=rv64gcb -mabi=lp64d" { target rv64 } } */
/* { dg-options "-march=rv32gcb -mabi=ilp32" { target rv32 } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Os" "-Oz" } } */

#define TEST_STORE_NARROW_SHIFT(TYPE, WTYPE, NAME)               \
void                                                             \
NAME (TYPE *data, unsigned int lo_bit)                           \
{                                                                \
  WTYPE mask = (((WTYPE) 1UL << 1) - 1) << lo_bit;               \
  *data = (*data & ~mask) | ((1U << lo_bit) & mask);             \
}

#define TEST_RET_NARROW_SHIFT(TYPE, WTYPE, NAME)                 \
TYPE                                                             \
NAME (TYPE data, unsigned int lo_bit)                            \
{                                                                \
  WTYPE mask = (((WTYPE) 1 << 1) - 1) << lo_bit;                 \
  return (data & ~mask) | ((1U << lo_bit) & mask);               \
}

/* WTYPE = unsigned int */
TEST_STORE_NARROW_SHIFT(unsigned char,  unsigned int, store_uc_ui_narrow)
TEST_STORE_NARROW_SHIFT(unsigned short, unsigned int, store_us_ui_narrow)

TEST_RET_NARROW_SHIFT(unsigned char,    unsigned int, ret_uc_ui_narrow)
TEST_RET_NARROW_SHIFT(unsigned short,   unsigned int, ret_us_ui_narrow)

/* WTYPE = unsigned long */
TEST_STORE_NARROW_SHIFT(unsigned char,  unsigned long, store_uc_ul_narrow)
TEST_STORE_NARROW_SHIFT(unsigned short, unsigned long, store_us_ul_narrow)
TEST_STORE_NARROW_SHIFT(unsigned int,   unsigned long, store_ui_ul_narrow)

TEST_RET_NARROW_SHIFT(unsigned char,    unsigned long, ret_uc_ul_narrow)
TEST_RET_NARROW_SHIFT(unsigned short,   unsigned long, ret_us_ul_narrow)
TEST_RET_NARROW_SHIFT(unsigned int,     unsigned long, ret_ui_ul_narrow)

/* WTYPE = unsigned long long */
TEST_STORE_NARROW_SHIFT(unsigned char,  unsigned long long, store_uc_ull_narrow)
TEST_STORE_NARROW_SHIFT(unsigned short, unsigned long long, store_us_ull_narrow)
TEST_STORE_NARROW_SHIFT(unsigned int,   unsigned long long, store_ui_ull_narrow)

TEST_RET_NARROW_SHIFT(unsigned char,    unsigned long long, ret_uc_ull_narrow)
TEST_RET_NARROW_SHIFT(unsigned short,   unsigned long long, ret_us_ull_narrow)
TEST_RET_NARROW_SHIFT(unsigned int,     unsigned long long, ret_ui_ull_narrow)

/* { dg-final { scan-assembler-times "\\tbset\\t" 14 { target rv64 } } } */
/* Current remaining missed SI-mode cases.  */
/* { dg-final { scan-assembler-times "\\tsllw\\t" 4 { target rv64 } } } */
/* { dg-final { scan-assembler-times "\\tbinv\\t" 2 { target rv64 } } } */
/* { dg-final { scan-assembler-times "\\tsext.w\\t" 4 { target rv64 } } } */


/* { dg-final { scan-assembler-times "\\tbset\\t" 16 { target rv32 } } } */
/* Testcases with ULL. */
/* { dg-final { scan-assembler-times "\\tbinv\\t" 2 { target rv32 } } } */
/* { dg-final { scan-assembler-times "\\tand\\t" 8 { target rv32 } } } */
/* { dg-final { scan-assembler-times "\\tsrai\\t" 6 { target rv32 } } } */

