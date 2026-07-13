/* PR target/126231  */
/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -march=x86-64-v4 -mavx10.2" } */
/* Keep labels and directives ('.cfi_startproc', '.cfi_endproc').  */
/* { dg-final { check-function-bodies "**" "" "" { target "*-*-*" } {^\t?\.}  } } */

#include <x86intrin.h>

/*
**func1:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2usis	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func1 (__m128 x)
{
  return _mm_cvtts_ss_epu32 (x);
}

/*
**func2:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2usis	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func2 (__m128 x)
{
  return _mm_cvtts_roundss_epu32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func3:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2sis	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func3 (__m128 x)
{
  return (unsigned int) _mm_cvtts_ss_epi32 (x);
}

/*
**func4:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttss2sis	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func4 (__m128 x)
{
  return (unsigned int) _mm_cvtts_roundss_epi32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func5:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2usis	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func5 (__m128d x)
{
  return _mm_cvtts_sd_epu32 (x);
}

/*
**func6:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2usis	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func6 (__m128d x)
{
  return _mm_cvtts_roundsd_epu32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}

/*
**func7:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2sis	%xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func7 (__m128d x)
{
  return (unsigned int) _mm_cvtts_sd_epi32 (x);
}

/*
**func8:
**.LFB[0-9]+:
**	.cfi_startproc
**	vcvttsd2sis	\{sae\}, %xmm0, %eax
**	ret
**	.cfi_endproc
**...
*/

unsigned long long
func8 (__m128d x)
{
  return (unsigned int) _mm_cvtts_roundsd_epi32
    (x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
}
