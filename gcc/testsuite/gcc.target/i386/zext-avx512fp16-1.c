/* PR target/126231  */
/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -march=x86-64-v4 -mavx512fp16" } */
/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "" "" { target "*-*-*" } {^\t?\.}  } } */

#include <x86intrin.h>

/*
**func1:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsh2si	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func1 (__m128h x)
{
  return (unsigned int) _mm_cvtsh_i32 (x);
}

/*
**func2:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsh2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func2 (__m128h x)
{
  return _mm_cvtsh_u32 (x);
}

/*
**func3:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsh2si	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func3 (__m128h x)
{
  return (unsigned int) _mm_cvttsh_i32 (x);
}

/*
**func4:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsh2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func4 (__m128h x)
{
  return _mm_cvttsh_u32 (x);
}

/*
**func5:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsh2usi	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func5 (__m128h x)
{
  return _mm_cvt_roundsh_u32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func6:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsh2si	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func6 (__m128h x)
{
  return (unsigned int) _mm_cvt_roundsh_i32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func7:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsh2usi	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func7 (__m128h x)
{
  return _mm_cvtt_roundsh_u32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func8:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsh2si	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func8 (__m128h x)
{
  return (unsigned int) _mm_cvtt_roundsh_i32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}
