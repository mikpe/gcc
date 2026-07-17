/* Verify that a TLS sequence insn and its paired call are not duplicated by
   the pre-RA passes.  Each movdi_er_tlsgd carries a sequence number that must
   stay in 1-1 correspondence with the !lituse_tlsgd of its call; duplicating
   the loop body used to copy both halves and make the assembler reject the
   result with "duplicate !tlsgd!1".  */
/* { dg-do assemble } */
/* { dg-require-effective-target tls_native } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-O2 -funroll-loops -fno-move-loop-invariants -fPIC -ftls-model=global-dynamic" } */

extern __thread int tv;
extern int cond (int);

int
f (int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    if (cond (i))
      s += tv;
  return s;
}
