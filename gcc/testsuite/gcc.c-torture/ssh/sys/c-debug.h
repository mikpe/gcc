/* Default to debugging on for now */
#ifndef _SYS_C_DEBUG_H_
#define _SYS_C_DEBUG_H_

#ifndef NODEBUG
#define _KCC_DEBUG
#endif

/* Debug flags */
extern int `$DEBUG`;

/* Heritage string */
extern char *`$HERIT`;

/* Debugging flags */

/* Parse.c */
#define _KCC_DEBUG_PARSE_RES	0000000000001 /* Print successful path */
#define _KCC_DEBUG_PARSE_INP	0000000000002 /* Print input path */
#define _KCC_DEBUG_PARSE_PAR	0000000000004 /* Print parse values */
#define _KCC_DEBUG_PARSE_TRY	0000000000010 /* Print parse paths tried */
#define _KCC_DEBUG_PARSE_EXP	0000000000020 /* Print logname expansion */
#define _KCC_DEBUG_FORK_CREATE	0000000000040 /* Debug fork creation */
#define _KCC_DEBUG_UIO_FDSHR	0000000000100 /* Debug FD sharing */
#define _KCC_DEBUG_FORK_EXEC	0000000000200 /* Debug file determination */
#define _KCC_DEBUG_FORK_ENV	0000000000400 /* Debug environment passing */
#define _KCC_DEBUG_INET		0000000001000 /* Debug internet routines */
#define _KCC_DEBUG_MALLOC	0000000002000 /* Debug malloc routines */

#define _KCC_DEBUG_CORE_DUMP	0400000000000 /* Debug write core dump file */

/* Debug routines from debug.c */

#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif

/* debug.c */
extern void _dbgs P_((const char *str));
extern void _dbgd P_((long num));
extern void _dbgo P_((unsigned long num));
extern void _dbgdn P_((unsigned long dirno));
extern void _dbgj P_((unsigned long jfn));
extern void _dbgh P_((unsigned long addr));
extern void _dbgl P_((const char *label));

#undef P_

#endif /* _SYS_C_DEBUG_H_ */
