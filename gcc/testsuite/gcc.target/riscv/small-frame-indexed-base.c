/* { dg-do compile } */
/* { dg-options "-O2 -fno-stack-protector -march=rv64gc_zba -mabi=lp64d" } */

struct item
{
  struct item *a;
  long b;
  long c;
  long d;
};

extern void init (struct item **);

static __attribute__ ((always_inline)) inline void
update (long *index, struct item **perm, struct item *value, long x)
{
  ++*index;
  perm[*index]->a = value;
  perm[*index]->b = x;
  perm[*index]->c = x < 0 ? -x : x;
  perm[*index]->d = 0;
}

__attribute__ ((noinline, noclone))
void
small_frame_indexed_access (long *index, struct item *value, long x)
{
  struct item *perm[64];

  init (perm);
  update (index, perm, value, x);
}

/* { dg-final { scan-assembler-not {\tli\t} } } */
/* { dg-final { scan-assembler-times {\tsh3add\t} 3 } } */
