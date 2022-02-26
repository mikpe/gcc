/* Check mem-ops work */

/* { dg-do compile } */
/* { dg-options "-O1" } */
/* { dg-skip-if "mem-ops require >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */
/* { dg-skip-if "hexagon_address_cost tells fwprop that forming (structure + 8) is too expensive at -G0, as a result the memop patterns are not matched" { "*-*-*" } { "-G0" } { "" } } */

/* { dg-final { scan-assembler-times {\+=} 15 } } */
/* { dg-final { scan-assembler-times {-=} 8 } } */
/* { dg-final { scan-assembler-times {&=} 8 } } */
/* { dg-final { scan-assembler-times {\|=} 8 } } */

extern char c, d;
extern short s, t;
extern long l, m;

extern struct {
  int a;
  int b;
  int c;
} structure;

void
add_const ()
{
  c += 31;
  s += 31;
  l += 31;
  structure.c += 31;
}

void
sub_const ()
{
  c -= 31;
  s -= 31;
  l -= 31;
  structure.c -= 31;
}

void
add_reg (char carg, short sarg, long larg, int iarg)
{
  c += carg;
  s += sarg;
  l += larg;
  structure.b += iarg;
  d = carg + d;
  t = sarg + t;
  m = larg + m;
  structure.c = iarg + structure.c;
}

void
sub_reg (char carg, short sarg, long larg, int iarg)
{
  c -= carg;
  s -= sarg;
  l -= larg;
  structure.b -= iarg;
  d = carg - d;
  t = sarg - t;
  m = larg - m;
  structure.c = iarg - structure.c;
}

void
and_reg (char carg, short sarg, long larg, int iarg)
{
  c &= carg;
  s &= sarg;
  l &= larg;
  structure.b &= iarg;
  d = carg & d;
  t = sarg & t;
  m = larg & m;
  structure.c = iarg & structure.c;
}

void
ior_reg (char carg, short sarg, long larg, int iarg)
{
  c |= carg;
  s |= sarg;
  l |= larg;
  structure.b |= iarg;
  d = carg | d;
  t = sarg | t;
  m = larg | m;
  structure.c = iarg | structure.c;
}


void
pointer_arith (char *ci, short *si, long *li, int i)
{
  *(ci+i) += 1;
  *(si+i) += 1;
  *(li+i) += 1;
}
