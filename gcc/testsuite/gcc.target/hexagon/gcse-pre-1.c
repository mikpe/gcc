/* { dg-do run } */
/* { dg-options "-O2" } */

/* This test is derived from the minimised testcase in #14738.  */

#include <stdio.h>
#include <stdlib.h>

struct xpvhv_aux {
  int xhv_riter;
};

typedef struct {
  int xhv_max;
} XPVHV;

typedef struct HE_tag {
  struct HE_tag *hent_next;
  struct {
    void *hent_val;
  } he_valu;
} HE;

typedef struct {
  union {
    void *svu_hash[10];
  } sv_u;
} HV;

static void *PL_sv_placeholder;

HE * __attribute__((noinline))
foo (HE *entry, HV *hv, XPVHV *xhv, int flags, struct xpvhv_aux *iter)
{
  while (!entry)
    {
      __asm__ __volatile__ ("nop;nop;");
      iter->xhv_riter++;
      if (iter->xhv_riter > (int)xhv->xhv_max)
        {
          iter->xhv_riter = -1;
	  break;
	}

      entry = (hv->sv_u.svu_hash)[iter->xhv_riter];

      if (!(flags & 0x01))
	while (entry && entry->he_valu.hent_val == &PL_sv_placeholder)
	  entry = entry->hent_next;
    }
  return entry;
}

int main (int argc, char *argv)
{
  HE ent1, ent2;
  struct xpvhv_aux iter;
  XPVHV xhv;
  HV hv;
  
  ent1.hent_next = &ent2;
  ent1.he_valu.hent_val = &PL_sv_placeholder;
  ent2.hent_next = 0;
  ent2.he_valu.hent_val = 0;
  
  iter.xhv_riter = 0;
  xhv.xhv_max = 2;
  
  hv.sv_u.svu_hash[0] = 0;
  hv.sv_u.svu_hash[1] = 0;
  hv.sv_u.svu_hash[2] = 0;
  hv.sv_u.svu_hash[3] = 0;
  hv.sv_u.svu_hash[4] = &ent1;
  hv.sv_u.svu_hash[5] = 0;
  hv.sv_u.svu_hash[6] = 0;
  
  if (foo (0, &hv, &xhv, 0, &iter) != 0)
    abort ();

  return 0;
}
