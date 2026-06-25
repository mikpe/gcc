/* Verify that when the V extension is enabled, the TLSDESC resolver clobbers
   all vector registers per the psABI.  The compiler must save/restore any
   live vector registers across the TLSDESC call.  */

/* { dg-do compile } */
/* { dg-require-effective-target tls_native } */
/* { dg-options "-O2 -fpic -mtls-dialect=desc -march=rv64gcv -mabi=lp64d" } */
/* { dg-require-effective-target fpic } */

extern __thread int tls_var;

void
test_vector_reg_clobber (void)
{
  __rvv_int32m1_t v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12;

  /* Write two v-regs.  */
  asm volatile ("# def v1" : "=vr"(v1));
  asm volatile ("# def v2" : "=vr"(v2));
  asm volatile ("# def v3" : "=vr"(v3));
  asm volatile ("# def v4" : "=vr"(v4));
  asm volatile ("# def v5" : "=vr"(v5));
  asm volatile ("# def v6" : "=vr"(v6));
  asm volatile ("# def v7" : "=vr"(v7));
  asm volatile ("# def v8" : "=vr"(v8));
  asm volatile ("# def v9" : "=vr"(v9));
  asm volatile ("# def v10" : "=vr"(v10));
  asm volatile ("# def v11" : "=vr"(v11));
  asm volatile ("# def v12" : "=vr"(v12));

  /* TLSDESC call — clobbers all vector regs.  */
  asm volatile ("" ::: "memory");  /* Prevent scheduling...  */
  tls_var = 1;
  asm volatile ("" ::: "memory");  /* Prevent scheduling...  */

  /* Read the two v-regs; their live-range spans across the TLSDESC call,
     so the compiler must emit vector store/load pairs to preserve them.  */
  asm volatile ("# use v1" : : "vr"(v1));
  asm volatile ("# use v2" : : "vr"(v2));
  asm volatile ("# use v3" : : "vr"(v3));
  asm volatile ("# use v4" : : "vr"(v4));
  asm volatile ("# use v5" : : "vr"(v5));
  asm volatile ("# use v6" : : "vr"(v6));
  asm volatile ("# use v7" : : "vr"(v7));
  asm volatile ("# use v8" : : "vr"(v8));
  asm volatile ("# use v9" : : "vr"(v9));
  asm volatile ("# use v10" : : "vr"(v10));
  asm volatile ("# use v11" : : "vr"(v11));
  asm volatile ("# use v12" : : "vr"(v12));
}

/* { dg-final { scan-assembler-times {jalr\tt0,} 1 } } */

/* Vector stores before and loads after the TLSDESC call.  */
/* { dg-final { scan-assembler-times {\tvs[0-9]+r\.v\t} 12 } } */
/* { dg-final { scan-assembler-times {\tvl[0-9]+re[0-9]+\.v\t} 12 } } */
