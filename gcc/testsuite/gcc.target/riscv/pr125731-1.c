/* { dg-do compile } */
/* { dg-additional-options "-march=rv64gcbv_zicond -mabi=lp64d -mbranch-cost=4" { target rv64 } } */
/* { dg-additional-options "-march=rv32gcbv_zicond -mabi=ilp32 -mbranch-cost=4" { target rv32 } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Os" "-Oz" "-Og" } } */



extern long lm, lr;

void
test_nez ()
{
  if (lm != 0)
    lr <<= (1 << 4);
  lr += lm;
}

void
test_eqz ()
{
  if (lm == 0)
    lr >>= (1 << 2);
  lr += lm;
}

/* { dg-final { scan-assembler-not "\tczero.eqz\t" } } */
/* { dg-final { scan-assembler-not "\tczero.nez\t" } } */
/* { dg-final { scan-assembler-times "\tslli\t" 2 } } */
/* { dg-final { scan-assembler-times "\tsll\t" 1 } } */
/* { dg-final { scan-assembler-times "\tsra\t" 1 } } */
