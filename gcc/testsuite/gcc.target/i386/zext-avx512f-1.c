/* PR target/126231  */
/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -march=x86-64-v4" } */
/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "" "" { target "*-*-*" } {^\t?\.}  } } */

#include <x86intrin.h>

/*
**func1:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2sil	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func1 (__m128 x)
{
  return (unsigned int) _mm_cvttss_i32 (x);
}

/*
**func2:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2sil	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func2 (__m128 x)
{
  return (unsigned int) _mm_cvtt_roundss_i32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func3:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtss2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func3 (__m128 x)
{
  return (unsigned int) _mm_cvtss_u32 (x);
}

/*
**func4:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func4 (__m128 x)
{
  return (unsigned int) _mm_cvttss_u32 (x);
}

/*
**func5:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2usi	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func5 (__m128 x)
{
  return (unsigned int) _mm_cvtt_roundss_u32 
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func6:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func6 (__m128d x)
{
  return (unsigned int) _mm_cvttsd_u32 (x);
}

/*
**func7:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2usi	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func7 (__m128d x)
{
  return (unsigned int) _mm_cvtt_roundsd_u32 
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func8:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsd2usi	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func8 (__m128d x)
{
  return (unsigned int) _mm_cvtsd_u32 (x);
}

/*
**func9:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsd2usi	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func9 (__m128d x)
{
  return (unsigned int) _mm_cvt_roundsd_u32 
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}


/*
**func10:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsd2sil	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func10 (__m128d x)
{
  return (unsigned int) _mm_cvt_roundsd_si32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func11:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtsd2sil	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func11 (__m128d x)
{
  return (unsigned int) _mm_cvt_roundsd_i32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func12:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtss2usi	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func12 (__m128 x)
{
  return (unsigned int) _mm_cvt_roundss_u32 
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func13:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtss2sil	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func13 (__m128 x)
{
  return (unsigned int) _mm_cvt_roundss_si32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func14:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvtss2sil	\{rn-sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func14 (__m128 x)
{
  return (unsigned int) _mm_cvt_roundss_i32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}
