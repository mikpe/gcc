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
large_frame_indexed_loop (long *indices, struct item **values, long count)
{
  struct item *perm[4096];

  init (perm);
  for (long i = 0; i < count; ++i)
    update (&indices[i], perm, values[i], i);
}

/* { dg-final { scan-assembler-times {\tli\t[^\n]+,-32768} 1 } } */
/* { dg-final { scan-assembler-times {\tli\t[^\n]+,32768} 1 } } */
/* { dg-final { scan-assembler {\tsh3add\t} } } */
