/* { dg-do compile { target { x86_64-*-mingw* } } } */
/* { dg-options "-O2" } */

/* PR78799: verify Win64 __int128 return uses an indirect sret-style path:
   RCX points to the return slot, RDX points to the argument object.  */
__attribute__((ms_abi, noinline, noclone))
__int128
ret_i128 (__int128 a)
{
  return a;
}

/* { dg-final { scan-assembler "movdqa\t\\(%rdx\\), %xmm0" } } */
/* { dg-final { scan-assembler "movq\t%rcx, %rax" } } */
/* { dg-final { scan-assembler "movaps\t%xmm0, \\(%rcx\\)" } } */
