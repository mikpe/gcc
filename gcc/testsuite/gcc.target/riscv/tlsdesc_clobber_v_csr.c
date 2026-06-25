/* Verify that when the V extension is enabled, the TLSDESC resolver clobbers
   vector CSRs (vl, vtype, vxrm, vxsat) per the psABI.  The compiler must
   re-emit a vsetvli after the TLSDESC call to recover the vector status. */

/* { dg-do compile } */
/* { dg-require-effective-target tls_native } */
/* { dg-options "-O2 -fpic -mtls-dialect=desc -march=rv64gcv -mabi=lp64d" } */
/* { dg-require-effective-target fpic } */

typedef int v4si __attribute__ ((vector_size (16)));

extern __thread int tls_var;

void
test_vector_csr_clobber (v4si *in, v4si *out)
{
  v4si vec;

  /* A pair of local load/store whose live-range does not span accross
     the TLSDESC.  It has a special size format so it needs a vsetvli.  */
  vec = *in;
  *out = vec;

  /* TLSDESC call — clobbers all vector CSRs including vl/vtype.  */
  asm volatile ("" ::: "memory");  /* Prevent scheduling...  */
  tls_var = 1;
  asm volatile ("" ::: "memory");  /* Prevent scheduling...  */

   /* Another pair of local load/store. But since TLSDESC clobbers vl/vtype, we
     must re-emit a vsetvli here.  */
  vec = *in;
  *out = vec;
}

/* { dg-final { scan-assembler-times {jalr\tt0,} 1 } } */

/* Two vsetvli: one before each vector segment.  The second is needed because
   TLSDESC clobbers vl/vtype.  */
/* { dg-final { scan-assembler-times {vsetvli|vsetivli} 2 } } */
