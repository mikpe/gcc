/* { dg-do compile { target rv64 } } */
/* { dg-options "-march=rv64gc -mabi=lp64d" } */
/* { dg-final { check-function-bodies "**" "" } } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Os" "-Og" "-Oz" } } */

typedef struct
{
  char pad;
  unsigned char repr;
} qi_zero_t;

typedef struct
{
  char pad;
  signed char repr;
} qi_sign_t;

typedef struct
{
  short pad;
  unsigned short repr;
} hi_zero_t;

typedef struct
{
  short pad;
  short repr;
} hi_sign_t;

typedef struct
{
  int pad;
  int repr;
} si_sign_t;

typedef struct
{
  int pad;
  unsigned repr;
} si_zero_t;



/*
**atomic_load_qi_zero:
**	lbu	[a-z][0-9]+,1\([a-z][0-9]+\)
**	ret
*/
unsigned atomic_load_qi_zero (const qi_zero_t *a) {
  unsigned char r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_qi_sign:
**	lb	[a-z][0-9]+,1\([a-z][0-9]+\)
**	ret
*/
int atomic_load_qi_sign (const qi_sign_t *a) {
  signed char r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_hi_zero:
**	lhu	[a-z][0-9]+,2\([a-z][0-9]+\)
**	ret
*/
unsigned atomic_load_hi_zero (const hi_zero_t *a) {
  unsigned short r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_hi_sign:
**	lh	[a-z][0-9]+,2\([a-z][0-9]+\)
**	ret
*/
int atomic_load_hi_sign (const hi_sign_t *a) {
  short r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_si_sign:
**	lw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
int atomic_load_si_sign (const si_sign_t *a) {
  int r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_si_zero:
**	lw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
unsigned atomic_load_si_zero (const si_zero_t *a) {
  unsigned r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}



typedef struct
{
  int pad;
  unsigned long long repr;
} di_t;

typedef struct
{
  int pad;
  unsigned char repr;
} qi_di_t;


/*
**atomic_load_di:
**	ld	[a-z][0-9]+,8\([a-z][0-9]+\)
**	ret
*/
unsigned long long atomic_load_di (const di_t *a) {
  unsigned long long r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_qi_di:
**	lbu	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
unsigned long long atomic_load_qi_di (const qi_di_t *a) {
  unsigned char r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


typedef struct
{
  int pad;
  unsigned repr;
} t;


/*
**atomic_load_relaxed:
**	lw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	ret
*/
unsigned atomic_load_relaxed (const t *a) {
  unsigned r;
  __atomic_load (&a->repr, &r, __ATOMIC_RELAXED);
  return r;
}


/*
**atomic_load_acquire:
**	lw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	fence	r,rw
**	ret
*/
unsigned atomic_load_acquire (const t *a) {
  unsigned r;
  __atomic_load (&a->repr, &r, __ATOMIC_ACQUIRE);
  return r;
}


/*
**atomic_load_seqcst:
**	fence	rw,rw
**	lw	[a-z][0-9]+,4\([a-z][0-9]+\)
**	fence	r,rw
**	ret
*/
unsigned atomic_load_seqcst (const t *a) {
  unsigned r;
  __atomic_load (&a->repr, &r, __ATOMIC_SEQ_CST);
  return r;
}
