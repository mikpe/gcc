/* 
 *------------------------------------------------------------------
 * $Id: cmtypes10.h 562abd07bbb7 2008/03/04 03:29:39 Martin Chaney <chaney@xkl.com> $ 
 *
 * (c) Copyright XKL, LLC. 2002 
 *------------------------------------------------------------------
 */

/* 
 * Defines common data types.
 *
 */

#ifndef __COMMON_TYPES_H_
#define __COMMON_TYPES_H_

#define DEFINE
#define FUNCTION

/* keyword definitions */

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
#define 

/* Data type definitions */
typedef void            VOID;
typedef   signed long   LONG;
typedef unsigned short  HALF;
typedef unsigned long   WORD;


/* TOPS20 Definitions */

#ifdef TOPS20

#define UNIT_BITS       36
#define WORD_BITS       36
#define WORD_MASK       0777777777777
#define ADDR_BITS       30
#define ADDR_MASK       0007777777777
#define BP_ADJUST       0010000000000

#define	BP7		LONG
#define	BP70		0610000000000
#define	BP71		0620000000000

#define STATIC

typedef   signed long   UNS32;
typedef float           FLOAT;

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

#ifdef __COMPILER_KCC__

#include <jsys.h>

typedef unsigned _KCCtype_char8 UNS8;

typedef union
{
  struct {
    WORD                hi;
    WORD                lo;
  }                     ac;
} HUGE;

#else /* __COMPILER_KCC__ */

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

#endif /* __COMPILER_KCC__ */

/*#include "mapping.h"*/

/*FUNCTION LONG            sprintf(LONG, char*, ...);*/

#if 0
#define getuid()         0
#define char             __UNUSED__
#define int              __UNUSED__
#define long             __UNUSED__
#define short            __UNUSED__
#endif

#else /* TOPS20 */

#define TOPS20           0

#define UNIT_BITS       8
#define WORD_BITS       32
#define WORD_MASK       0xFFFFFFFF
#define ADDR_BITS       32
#define ADDR_MASK       0xFFFFFFFF
#define BP_ADJUST       0x00000001

#define STATIC          static

typedef unsigned long   UNS32;
typedef signed long     FLOAT;

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

typedef unsigned char   UNS8;

#define	BP7		UNS8*
#define	BP70		0
#define	BP71		0

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

#endif /* TOPS20 */

#define LOWBIT          (1<<(WORD_BITS-32))

#define HALF_BITS       (WORD_BITS/2)

#define WORD_SIGN_BIT   (1<<(WORD_BITS-1))
#define HALF_SIGN_BIT   (1<<(HALF_BITS-1))

#define INTEGER_MIN     WORD_SIGN_BIT
#define INTEGER_MAX     (~INTEGER_MIN)

#define TRUE            1
#define FALSE           0
#define ZERO            0
#define NONE           -1

#define DONE            0
#define MORE            WORD_SIGN_BIT

#undef  NULL
#define NULL            0
#define isNULL(a)       (((signed)(a)) <= ZERO)
#define NotNULL(a)      (((signed)(a)) >  ZERO)
#define IsZERO(a)       ((a) == 0)
#define NotZERO(a)      ((a) != 0)

#define WORD_ADDRESS(a) (((LONG)(a))/(WORD_BITS/UNIT_BITS))
#define ADJBP(a,b)      (((LONG)(a))+(BP_ADJUST*(b)))
#define	FirstBP7(a)	(((BP7)(a)+(~ADDR_MASK)))

#define MASK(a)         ((1<<(a))-1)

#define UNSLH(a)        (((ULHRH*)(LONG)&(a))->lh)
#define UNSRH(a)        (((ULHRH*)(LONG)&(a))->rh)
#define INTLH(a)        (((SLHRH*)(LONG)&(a))->lh)
#define INTRH(a)        (((SLHRH*)(LONG)&(a))->rh)

#define PACKHILO(a,b)   (((a)&0xFFFF)<<16|((b)&0xFFFF))
#define UNPACKHI(a)     (((a)>>16)&0xFFFF)
#define UNPACKLO(a)     (((a)>>00)&0xFFFF)

typedef	LONG	BP;

#define	DECBP(a)	(a-=010000000000)
#define	SUBBP(a,b)	((((((BP)(a))-((BP)(b)))&ADDR_MASK)<<2)+\
			  ((((BP)(a))-((BP)(b)))>>30))

typedef LONG            JFN;



#endif /* __COMMON_TYPES_H_ */
