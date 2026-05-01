/* { dg-do compile } */
/* { dg-options "-O2 -march=rv64gc_zicond -mabi=lp64d" } */
/* { dg-skip-if "" { *-*-* } { "-flto" } } */

/* Verify that power-of-2 boundary comparisons use shift instead of
   materializing large constants (vrull/gcc#253).  */

long long high_byte (long long a, long long b)
{
  return (a & (0xffULL << 56)) ? b : 0;
}

long long high_half (long long a, long long b)
{
  return (a & (0xffffULL << 48)) ? b : 0;
}

/* { dg-final { scan-assembler-times "srli\t" 2 } } */
/* { dg-final { scan-assembler-times "czero\\.eqz\t" 2 } } */
/* { dg-final { scan-assembler-not "bseti\t" } } */
