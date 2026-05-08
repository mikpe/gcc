/* { dg-do compile { target rv64} } */
/* { dg-additional-options "-march=rv64gc_zicond -mabi=lp64d" } */

typedef unsigned int uint32;

uint32 random(uint32 s1) {
    const uint32 mask = 0xffffffff;
    s1 = (((s1 & 0xFFFFFFFEU) << 12) & mask);
    return s1;
}

/* { dg-final { scan-assembler-not "slli\t" } } */
/* { dg-final { scan-assembler-not "srai\t" } } */
/* { dg-final { scan-assembler-times "slliw\t" 1 } } */
