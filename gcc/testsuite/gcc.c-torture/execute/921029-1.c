typedef unsigned long long ULL;
ULL back;
ULL hpart, lpart;
ULL

#ifdef __PDP10__
build(long h, long l)
{
  /* hpart uses 35 bits, lpart uses 36 bits */
  hpart = h;
  hpart <<= 36;  /* Lop off high order bit in h */
  lpart = l;
  lpart &= 0xFFFFFFFFFLL;
  back = hpart | lpart;
  return back;
}

main()
{
  if (build(0, 1) != 0x000000000000000001LL)
    abort();
  if (build(0, 0) != 0x000000000000000000LL)
    abort();
  if (build(0, 0xFFFFFFFFF) != 0x000000000FFFFFFFFFLL)
    abort();
  if (build(0, 0xFFFFFFFFE) != 0x000000000FFFFFFFFELL)
    abort();
  if (build(1, 1) != 0x000000001000000001LL)
    abort();
  if (build(1, 0) != 0x000000001000000000LL)
    abort();
  if (build(1, 0xFFFFFFFFF) != 0x000000001FFFFFFFFFLL)
    abort();
  if (build(1, 0xFFFFFFFFE) != 0x000000001FFFFFFFFELL)
    abort();
  if (build(0x7FFFFFFFF, 1) != 0x7FFFFFFFF000000001LL)
    abort();
  if (build(0x7FFFFFFFF, 0) != 0x7FFFFFFFF000000000LL)
    abort();
  if (build(0x7FFFFFFFF, 0xFFFFFFFFF) != 0x7FFFFFFFFFFFFFFFFFLL)
    abort();
  if (build(0x7FFFFFFFF, 0xFFFFFFFFE) != 0x7FFFFFFFFFFFFFFFFELL)
    abort();
  exit(0);
}

#else

build(long h, long l)
{
  hpart = h;
  hpart <<= 32;
  lpart = l;
  lpart &= 0xFFFFFFFFLL;
  back = hpart | lpart;
  return back;
}

main()
{
  if (build(0, 1) != 0x0000000000000001LL)
    abort();
  if (build(0, 0) != 0x0000000000000000LL)
    abort();
  if (build(0, 0xFFFFFFFF) != 0x00000000FFFFFFFFLL)
    abort();
  if (build(0, 0xFFFFFFFE) != 0x00000000FFFFFFFELL)
    abort();
  if (build(1, 1) != 0x0000000100000001LL)
    abort();
  if (build(1, 0) != 0x0000000100000000LL)
    abort();
  if (build(1, 0xFFFFFFFF) != 0x00000001FFFFFFFFLL)
    abort();
  if (build(1, 0xFFFFFFFE) != 0x00000001FFFFFFFELL)
    abort();
  if (build(0xFFFFFFFF, 1) != 0xFFFFFFFF00000001LL)
    abort();
  if (build(0xFFFFFFFF, 0) != 0xFFFFFFFF00000000LL)
    abort();
  if (build(0xFFFFFFFF, 0xFFFFFFFF) != 0xFFFFFFFFFFFFFFFFLL)
    abort();
  if (build(0xFFFFFFFF, 0xFFFFFFFE) != 0xFFFFFFFFFFFFFFFELL)
    abort();
  exit(0);
}

#endif
