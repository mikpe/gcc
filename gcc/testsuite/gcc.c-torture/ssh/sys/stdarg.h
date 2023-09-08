/* <STDARG.H> - Variable argument facility (draft proposed ANSI C)
**
**	(c) Copyright Ken Harrenstien 1989
**
** These macros are only valid for the KCC PDP-10 C compiler.
*/

#ifndef _STDARG_INCLUDED
#define _STDARG_INCLUDED

#ifdef	__COMPILER_KCC__

typedef int *va_list;		/* Type for holding pointer to arg list */

#define va_start(ap, pn) (ap = (va_list)&(pn))	/* Init va_list from parmN */
#define	va_end(ap)
#define va_arg(ap, type) \
    (sizeof(type) >= sizeof(int) \
	? *(type *)(ap -= sizeof(type)/sizeof(int)) \
	: (type)(*--(ap)))

#else

typedef __builtin_va_list va_list;

#define va_start(v,l)   __builtin_stdarg_start((v),l)
#define va_end          __builtin_va_end
#define va_arg          __builtin_va_arg

#endif

#endif /* ifndef _STDARG_INCLUDED */
