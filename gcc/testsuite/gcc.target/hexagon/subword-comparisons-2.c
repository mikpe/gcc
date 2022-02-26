/* These functions should NOT use subword comparisons, since function arguments
   will have valid high-order bits, so full-word comparisons can be used
   instead.  */

/* { dg-do compile } */
/* { dg-options "-O1" } */
/* { dg-skip-if "subword comparisons require >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

/* { dg-final { scan-assembler-times "cmp\\." 40 } } */
/* { dg-final { scan-assembler-not "cmpb\\." } } */
/* { dg-final { scan-assembler-not "cmpb\\." } } */
/* { dg-final { scan-assembler-not "zxth" } } */
/* { dg-final { scan-assembler-not "sxth" } } */
/* { dg-final { scan-assembler-not "zxtb" } } */
/* { dg-final { scan-assembler-not "sxtb" } } */

#define IMM(NAME, TYPE, OP) \
  int NAME##imm (TYPE c) \
  { \
    return c OP 5; \
  }

IMM(eq, short, ==)
IMM(ne, short, !=)
IMM(gt, short, >)
IMM(ge, short, >=)
IMM(lt, short, <)
IMM(le, short, <=)
IMM(gtu, unsigned short, >)
IMM(geu, unsigned short, >=)
IMM(ltu, unsigned short, <)
IMM(leu, unsigned short, <=)

IMM(eqb, signed char, ==)
IMM(neb, signed char, !=)
IMM(gtb, signed char, >)
IMM(geb, signed char, >=)
IMM(ltb, signed char, <)
IMM(leb, signed char, <=)
IMM(gtub, unsigned char, >)
IMM(geub, unsigned char, >=)
IMM(ltub, unsigned char, <)
IMM(leub, unsigned char, <=)

#define REG(NAME, TYPE, OP) \
  int NAME##reg (TYPE c, TYPE d) \
  {  \
    return c OP d; \
  }

REG(eq, short, ==)
REG(ne, short, !=)
REG(gt, short, >)
REG(ge, short, >=)
REG(lt, short, <)
REG(le, short, <=)
REG(gtu, unsigned short, >)
REG(geu, unsigned short, >=)
REG(ltu, unsigned short, <)
REG(leu, unsigned short, <=)

REG(eqb, signed char, ==)
REG(neb, signed char, !=)
REG(gtb, signed char, >)
REG(geb, signed char, >=)
REG(ltb, signed char, <)
REG(leb, signed char, <=)
REG(gtub, unsigned char, >)
REG(geub, unsigned char, >=)
REG(ltub, unsigned char, <)
REG(leub, unsigned char, <=)
