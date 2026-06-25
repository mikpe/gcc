/* Verify that the TLSDESC resolver only clobbers a0 and t0 while assuming
   all other registers as preserved per its custom ABI (no vector case),
   which is different from a normal call.  */

/* { dg-do compile } */
/* { dg-require-effective-target tls_native } */
/* { dg-options "-O2 -fpic -mtls-dialect=desc -march=rv64gc -mabi=lp64d" } */
/* { dg-require-effective-target fpic } */

extern __thread int tls_var;

long
test_clobber (long a, long b, long c, long d)
{
  /* a=a0, b=a1, c=a2, d=a3.
     TLSDESC clobbers a0 and t0 only, so the compiler must save/restore
     a0 (by stack or mv or whatever), but should not do the same thing
     for a1-a3 like a normal call.  */
  tls_var = 1;
  return a + b + c + d;
}

/* The TLSDESC call should be present.  */
/* { dg-final { scan-assembler-times {jalr\tt0,} 1 } } */

/* ra/a1-a3 should NOT be moved to s-regs or saved to stack.  */
/* { dg-final { scan-assembler-not {mv\ts[0-9]+,a[0-9]+} } } */
/* { dg-final { scan-assembler-not {sd\ta[0-9]+,.*\(sp\)} } } */
/* { dg-final { scan-assembler-not {sd\tra,.*\(sp\)} } } */
