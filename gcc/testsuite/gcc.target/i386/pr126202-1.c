/* { dg-do compile } */
/* { dg-options "-march=x86-64 -mavx -O2 -fno-strict-overflow" } */

typedef long __m128i __attribute__ ((__vector_size__ (16)));
__m128i _mm_store_si128___B;
typedef long __m256i __attribute__ ((__vector_size__ (32)));
int vpx_quantize_b_avx___trans_tmp_6, vpx_quantize_b_avx_index;
void
_mm256_store_si256 (__m256i *__P, __m256i __A)
{
  *__P = __A;
}
void
store_tran_low (int *b)
{
  *(__m128i *)b = _mm_store_si128___B;
}
void
vpx_quantize_b_avx (int, long, int, int *qcoeff_ptr, int, short,
                    short *eob_ptr)
{
  __m256i __trans_tmp_5 = {};
  if (vpx_quantize_b_avx___trans_tmp_6)
    _mm256_store_si256 ((__m256i *)(qcoeff_ptr
				    + vpx_quantize_b_avx_index + 8),
                        __trans_tmp_5);
  store_tran_low (qcoeff_ptr + vpx_quantize_b_avx_index + 8);
  *eob_ptr = 0;
}
