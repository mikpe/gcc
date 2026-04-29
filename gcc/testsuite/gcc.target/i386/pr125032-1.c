/* { dg-do compile { target { ia32 && pie } } } */
/* { dg-options "-O2 -march=i686 -mmmx -fPIE" } */

typedef int __m64 __attribute__((__vector_size__(8)));
typedef short __v4hi __attribute__((__vector_size__(8)));
typedef char __v8qi __attribute__((__vector_size__(8)));
int mmx_composite_over_n_8_0565_info_0, mmx_composite_over_n_8_0565_w;
long long mmx_composite_over_n_8_0565_m3;
__m64 mmx_composite_over_n_8_0565_v2, mmx_composite_over_n_8_0565_v3;
__m64 in_over(__m64 src)
{
  __m64 __m1 = src, __m2;
  return (__m64)__builtin_ia32_pmullw((__v4hi)__m1, (__v4hi)__m2);
}
__m64 load8888()
{
  __m64 __m2, __m1;
  return (__m64)__builtin_ia32_punpcklbw((__v8qi)__m1, (__v8qi)__m2);
}
void mmx_composite_over_n_8_0565()
{
  __m64 vsrc = load8888();
  mmx_composite_over_n_8_0565_w = mmx_composite_over_n_8_0565_info_0;
  while (mmx_composite_over_n_8_0565_info_0)
    if (mmx_composite_over_n_8_0565_m3)
      {
	mmx_composite_over_n_8_0565_v2 = in_over(vsrc);
	mmx_composite_over_n_8_0565_v3 = in_over(vsrc);
      }
}
