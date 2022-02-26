/* Verify that multiple memory references within a global object ends up
   CSE'ing the constant extender instead of using the expensive absolute
   or absolute plus scaled register addressing modes.  */

/* { dg-do compile } */
/* { dg-options "-Os" } */
/* { dg-skip-if "extenders require >= hexagonv4" { "*-*-*" } { "*" } { "-march=hexagonv[45]" } } */

struct S {
  char ca[156];
  void *a;
  void *b;
  char c;
  void *x;
  void (*d)(void *);
  char e;
} s;

enum {
  ZERO = 0,
  ONE,
  TWO
};

void f(void *);

void init(void){
  struct S *p = &s;
  p->c = 0;
  p->a = 0;
  p->b = &p->ca[144];
  p->e = ZERO;
  p->d = &f;
}

/* { dg-final { scan-assembler-times "##s" 1 } } */
