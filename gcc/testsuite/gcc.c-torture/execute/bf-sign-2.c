/* 
 This test checks promotion of bitfields.  Bitfields should be promoted
 very much like chars and shorts: 

 Bitfields (signed or unsigned) should be promoted to signed int if their
 value will fit in a signed int, otherwise to an unsigned int if their 
 value will fit in an unsigned int, otherwise we don't promote them (ANSI/ISO
 does not specify the behavior of bitfields larger than an unsigned int).

 We test the behavior by subtracting two from the promoted value: this will
 result in a negitive value for signed types, a positive value for unsigned
 types.  This test (of course) assumes that the compiler is correctly 
 implementing signed and unsigned arithmetic.
 */

struct X {
  unsigned int	     u3:3;
#ifdef __PDP10__
    signed long int  s35:35;
    signed long int  s36:36;
  unsigned long int  u35:35;
  unsigned long int  u36:36;
  unsigned long long ull3 :3;
  unsigned long long ull44:44;
#else
    signed long int  s31:31;
    signed long int  s32:32;
  unsigned long int  u31:31;
  unsigned long int  u32:32;
  unsigned long long ull3 :3;
  unsigned long long ull35:35;
#endif
  unsigned u15:15;
};

struct X x;

main ()
{
  if ((x.u3 - 2) >= 0)		/* promoted value should be signed */
    abort ();
#ifdef __PDP10__
  if ((x.s35 - 2) >= 0)		/* promoted value should be signed */
#else
  if ((x.s31 - 2) >= 0)		/* promoted value should be signed */
#endif
    abort ();

#ifdef __PDP10__
  if ((x.s36 - 2) >= 0)		/* promoted value should be signed */
#else
  if ((x.s32 - 2) >= 0)		/* promoted value should be signed */
#endif
    abort ();

  if ((x.u15 - 2) >= 0)		/* promoted value should be signed */
    abort ();

  /* Conditionalize check on whether integers are 4 bytes or larger, i.e.
     larger than a 31 bit bitfield.  */
  if (sizeof (int) >= 4)
    {
#ifdef __PDP10__
      if ((x.u35 - 2) >= 0)	/* promoted value should be signed */
#else
      if ((x.u31 - 2) >= 0)	/* promoted value should be signed */
#endif
	abort ();
    }
  else
    {
#ifdef __PDP10__
      if ((x.u35 - 2) < 0)	/* promoted value should be UNsigned */
#else
      if ((x.u31 - 2) < 0)	/* promoted value should be UNsigned */
#endif
	abort ();
    }

#ifdef __PDP10__
  if ((x.u36 - 2) < 0)		/* promoted value should be UNsigned */
#else
  if ((x.u32 - 2) < 0)		/* promoted value should be UNsigned */
#endif
    abort ();

  if ((x.ull3 - 2) >= 0)	/* promoted value should be signed */
    abort ();
#ifdef __PDP10__
  if ((x.ull44 - 2) < 0)	/* promoted value should be UNsigned */
#else
  if ((x.ull35 - 2) < 0)	/* promoted value should be UNsigned */
#endif
    abort ();

  exit (0);
}
