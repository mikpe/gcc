
/* This function is a roundabout way of calculating MAX_UINT MOD mod + 1 */
unsigned calc_mp(unsigned mod)
{
      unsigned a,b,c;
      c=-1;
      a=c/mod;
      b=0-a*mod;
      /* the following test should never be true */
      if (b > mod) { a += 1; b-=mod; }
      return b;
}

int main(int argc, char *argv[])
{
      unsigned x = 1234;
      unsigned y = calc_mp(x);

#ifdef __PDP10__
/* The expected value for y is (((2^b-1) % 1234) + 1) where b is the     */
/* bitsize of an unsigned. On the PDP10 this should be 36 or 18 instead  */
/* 32 or 16 as the original test assumes.                                */
      if ((sizeof (y) == 4 && y != 1008)
	  || (sizeof (y) == 2 && y != 536))
#else
      if ((sizeof (y) == 4 && y != 680)
	  || (sizeof (y) == 2 && y != 134))
#endif
	abort ();
      exit (0);
}


