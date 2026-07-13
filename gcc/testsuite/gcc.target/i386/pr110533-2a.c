/* { dg-do compile } */
/* { dg-options "-O0" } */

struct X {
  int l1;
  long long l2;
};

__attribute__((naked))
void fn(struct X a) {
    asm("ret");
}

/* { dg-final { scan-assembler-not "mov" } } */
