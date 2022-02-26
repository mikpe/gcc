/* { dg-do compile } */
/* { dg-options "-O1" } */
/* { dg-skip-if "subword comparisons require >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

/* { dg-final { scan-assembler-times "cmph\\." 20 } } */
/* { dg-final { scan-assembler-times "cmpb\\." 20 } } */
/* { dg-final { scan-assembler-not "zxth" } } */
/* { dg-final { scan-assembler-not "sxth" } } */
/* { dg-final { scan-assembler-not "zxtb" } } */
/* { dg-final { scan-assembler-not "sxtb" } } */

#define IMM(NAME, TYPE, CAST, OP) \
  int NAME##imm (TYPE c) \
  { \
    return CAST c OP 5; \
  }

IMM(eq, int, (short), ==)
IMM(ne, int, (short), !=)
IMM(gt, int, (short), >)
IMM(ge, int, (short), >=)
IMM(lt, int, (short), <)
IMM(le, int, (short), <=)
IMM(gtu, unsigned int, (unsigned short), >)
IMM(geu, unsigned int, (unsigned short), >=)
IMM(ltu, unsigned int, (unsigned short), <)
IMM(leu, unsigned int, (unsigned short), <=)

IMM(eqb, int, (signed char), ==)
IMM(neb, int, (signed char), !=)
IMM(gtb, int, (signed char), >)
IMM(geb, int, (signed char), >=)
IMM(ltb, int, (signed char), <)
IMM(leb, int, (signed char), <=)
IMM(gtub, unsigned int, (unsigned char), >)
IMM(geub, unsigned int, (unsigned char), >=)
IMM(ltub, unsigned int, (unsigned char), <)
IMM(leub, unsigned int, (unsigned char), <=)

#define REG(NAME, TYPE, CAST, OP) \
  int NAME##reg (TYPE c, TYPE d) \
  {  \
    return CAST c OP CAST d; \
  }

REG(eq, int, (short), ==)
REG(ne, int, (short), !=)
REG(gt, int, (short), >)
REG(ge, int, (short), >=)
REG(lt, int, (short), <)
REG(le, int, (short), <=)
REG(gtu, unsigned int, (unsigned short), >)
REG(geu, unsigned int, (unsigned short), >=)
REG(ltu, unsigned int, (unsigned short), <)
REG(leu, unsigned int, (unsigned short), <=)

REG(eqb, int, (signed char), ==)
REG(neb, int, (signed char), !=)
REG(gtb, int, (signed char), >)
REG(geb, int, (signed char), >=)
REG(ltb, int, (signed char), <)
REG(leb, int, (signed char), <=)
REG(gtub, unsigned int, (unsigned char), >)
REG(geub, unsigned int, (unsigned char), >=)
REG(ltub, unsigned int, (unsigned char), <)
REG(leub, unsigned int, (unsigned char), <=)
