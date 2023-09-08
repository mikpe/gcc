/* 
 *------------------------------------------------------------------
 * $Id: tops20.h 562abd07bbb7 2008/03/04 03:29:39 Martin Chaney <chaney@xkl.com> $
 *
 * (c) Copyright XKL, LLC. 2002 
 *------------------------------------------------------------------
 */

#ifndef HEADER_TOPS20_H
#define HEADER_TOPS20_H

#define if              if
#define else            else
#define for             for
#define do              do
#define while           while
#define continue        continue
#define break           break
#define switch          switch
#define case            case
#define default         default
#define return          return
#define goto            goto
#define typedef         typedef
#define union           union
#define struct          struct
#define REG             register
#define extern          extern
#define const           const
#define DEFINE
#define FUNCTION
#define ROUTINE		extern

/* Data type definitions */

typedef void            VOID;
typedef   signed long   LONG;
typedef unsigned short  HALF;
typedef unsigned long   WORD;
typedef	unsigned long	UNSIGNED;
/* TOPS20 Definitions */

#ifdef TOPS20
/* #include "sixbit.h" */

/* #pragma	no_convert_voidp */

#define UNIT_BITS       36
#define WORD_BITS       36
#define WORD_MASK       0777777777777
#define ADDR_BITS       30
#define ADDR_MASK       0007777777777

#define STATIC

typedef   signed long   UNS32;

typedef struct ULHRHStruct ULHRH;
DEFINE struct ULHRHStruct
{
  HALF                  lh;
  HALF                  rh;
};

typedef struct SLHRHStruct SLHRH;
DEFINE struct SLHRHStruct
{
  signed short          lh;
  signed short          rh;
};

#define LH              0777777000000
#define RH              0000000777777

typedef union
{ 
  struct {
    WORD                hi;
    WORD                lo;
  }                     ac;
  unsigned long long    dw;
} HUGE;

typedef unsigned UNS8   __attribute__ ((size(8)));
typedef	UNS8*		UNS8P;
typedef	UNS8*		VOIDP;

#define	char		UNS8

#define UDIV(a,b)	((((int)a)/((int)b))&0xFFFFFFFF)
#define UMOD(a,b)	((((int)(a))%((int)(b)))&0xFFFFFFFF)
#define	DECBP(a)	(a-=010000000000)
#define SUBBP(a,b)	((((((int)a)-((int)b))*4)+((((int)a)-((int)b))>>30)) & 0xFFFF)
#define closesocket(a)	shutdown(a,2)

#define eprintf(format,args...) printf(format, ## args)

#else /* TOPS20 */

#define UNIT_BITS       8
#define WORD_BITS       32
#define WORD_MASK       0xFFFFFFFF
#define ADDR_BITS       32
#define ADDR_MASK       0xFFFFFFFF

#define STATIC          static

typedef unsigned long   UNS32;

typedef struct ULHRHStruct ULHRH;
DEFINE struct ULHRHStruct
{
  HALF                  rh;
  HALF                  lh;
};

typedef struct SLHRHStruct SLHRH;
DEFINE struct SLHRHStruct
{
  signed short          rh;
  signed short          lh;
};

#define	RH		0xFFFFFFFF
#define	LH		0x00000000

typedef union
{ 
  struct {
    WORD                lo;
    WORD                hi;
  }                     ac;
  unsigned long long    dw;
} HUGE;

typedef unsigned char   UNS8;
typedef	unsigned char*	UNS8P;
typedef	unsigned char*	VOIDP;

#define UDIV(a, b)	((a)/(b))
#define UMOD(a, b)	((a)%(b))
#define	DECBP(a)	(a-=1)
#define SUBBP(a, b)	((a)-(b))
#define closesocket(a)	close(a)

#define eprintf(format, args...) printf(format, ## args)

#endif /* TOPS20 */

#define HALF_BITS       (WORD_BITS/2)

#define WORD_SIGN_BIT   (1<<(WORD_BITS-1))
#define HALF_SIGN_BIT   (1<<(HALF_BITS-1))

#define INTEGER_MIN     WORD_SIGN_BIT
#define INTEGER_MAX     (~INTEGER_MIN)
#define	MAX_INT		INTEGER_MAX

#define TRUE            1
#define FALSE           0
#define ZERO            0
#define NONE           -1

#undef  NULL
#define NULL            0
#define IsNULL(a)       (((signed)(a)) <= ZERO)
#define NotNULL(a)      (((signed)(a)) >  ZERO)
#define IsZERO(a)       ((a) == 0)
#define NotZERO(a)      ((a) != 0)

#define MASK(a)         ((1<<(a))-1)

#define UNSLH(a)        (((ULHRH*)(LONG)&(a))->lh)
#define UNSRH(a)        (((ULHRH*)(LONG)&(a))->rh)
#define INTLH(a)        (((SLHRH*)(LONG)&(a))->lh)
#define INTRH(a)        (((SLHRH*)(LONG)&(a))->rh)

#define	memcpy(d,s,n)		_mmcpy((long)d,(long)s,n)
#define	memset(d,v,n)		_mmset((long)d,v,n)
#define	memcmp(s1,s2,n)		_mmcmp((long)s1,(long)s2,n)

typedef struct LongLongStruct LLONG;
DEFINE  struct LongLongStruct
{
  LONG highWord;
  LONG lowWord;
};

#endif
