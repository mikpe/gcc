/* { dg-do compile { target fpic } } */
/* { dg-options "-O2 -fPIC" } */

long _HMAC_SHA256_Init_Klen;
char _crypt_HMAC_SHA256_Init_pad[64];
char _crypt_HMAC_SHA256_Init_pad_0, _crypt_HMAC_SHA256_Init_K_0;
void _crypt_HMAC_SHA256_Init_i() {
  if (_HMAC_SHA256_Init_Klen)
    _HMAC_SHA256_Init_Klen = 2;
  long __trans_tmp_1 =
      __builtin_dynamic_object_size(_crypt_HMAC_SHA256_Init_pad, 0);
  __builtin___memset_chk(_crypt_HMAC_SHA256_Init_pad, 2, 64, __trans_tmp_1);
  for (; _HMAC_SHA256_Init_Klen;)
    _crypt_HMAC_SHA256_Init_pad_0 ^= _crypt_HMAC_SHA256_Init_K_0;
}
