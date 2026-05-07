/* { dg-do compile { target rv64 } } */
/* { dg-additional-options "-march=rv64gc_zicond -mabi=lp64d" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og"} } */

typedef signed char int8_t;
int8_t f(int8_t x)
{
    int8_t sh = 1 << x;
    return sh & 1;
}

/* { dg-final { scan-assembler-not "li\t" } } */
/* { dg-final { scan-assembler-not "sllw\t" } } */
/* { dg-final { scan-assembler-times "andi\t" 1 } } */
/* { dg-final { scan-assembler-times "seqz\t" 1 } } */
