/* { dg-do compile { target rv64 } } */
/* { dg-options "-march=rv64gc_zalasr -mabi=lp64d" } */
/* { dg-final { check-function-bodies "**" "" } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Os" "-Oz" } } */

typedef struct
{
  char pad;
  unsigned char repr;
} qi_s_t;

typedef struct
{
  short pad;
  unsigned short repr;
} hi_s_t;

typedef struct
{
  int pad;
  unsigned repr;
} si_s_t;


/*
**atomic_store_qi:
**	sb	[a-z][0-9]+,1\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_qi (qi_s_t *a, unsigned char val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELAXED);
}


/*
**atomic_store_hi:
**	sh	[a-z][0-9]+,2\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_hi (hi_s_t *a, unsigned short val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELAXED);
}


/*
**atomic_store_si:
**	sw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_si (si_s_t *a, unsigned val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELAXED);
}


typedef struct
{
  int pad;
  unsigned long long repr;
} di_s_t;


/*
**atomic_store_di:
**	sd	[a-z][0-9]+,8\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_di (di_s_t *a, unsigned long long val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELAXED);
}


typedef struct
{
  int pad;
  unsigned repr;
} s_mm;


/*
**atomic_store_relaxed:
**	sw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_relaxed (s_mm *a, unsigned val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELAXED);
}


/*
**atomic_store_release:
**	addi	[a-z][0-9]+,[a-z][0-9]+,4
**	sw.rl	[a-z][0-9]+,0\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_release (s_mm *a, unsigned val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_RELEASE);
}


/*
**atomic_store_seqcst:
**	addi	[a-z][0-9]+,[a-z][0-9]+,4
**	sw.rl	[a-z][0-9]+,0\([a-z][0-9]+\)
**	ret
*/
void
atomic_store_seqcst (s_mm *a, unsigned val)
{
  __atomic_store (&a->repr, &val, __ATOMIC_SEQ_CST);
}
