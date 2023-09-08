/* <STDLIB.H> - General Utility decls and defs (draft proposed ANSI C)
**
**	(c) Copyright Ken Harrenstien 1989
**	(c) Copyright Ken Harrenstien, SRI International 1987
**
*/

#ifndef _STDLIB_INCLUDED
#define _STDLIB_INCLUDED
#ifndef __STDC__	/* Canonicalize this indicator to prevent err msgs */
#define __STDC__ 0
#endif

/****************** Type Definitions ****************/

#ifndef _WCHAR_T_DEFINED	/* Prevent conflict with other headers */
#define _WCHAR_T_DEFINED
typedef char wchar_t;		/* Type of "wide" chars */
#endif

#ifndef _SIZE_T_DEFINED		/* Prevent conflict with other headers */
#define _SIZE_T_DEFINED
typedef unsigned size_t;	/* Type of sizeof() (must be unsigned, ugh) */
#endif

typedef struct { int quot; int rem; } div_t;
typedef struct { long quot; long rem; } ldiv_t;


/****************** Macro Definitions ****************/

#define NULL 0			/* Benign redefinition */

#define EXIT_FAILURE 1		/* Args to exit() */
#define EXIT_SUCCESS 0

#define RAND_MAX 32767		/* 2^15-1 Max value from rand() */

#define MB_CUR_MAX 1		/* Current max size of multibyte char */

#if __STDC__			/* ANSI prototype decls */

/* String Conversion functions */
double	atof(const char *);
/*
int	atoi(const char *);
long	atol(const char *);
*/
double	strtod(const char *, char **);
long	strtol(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);

/* Pseudo-random sequence generation functions */
int rand(VOID);
VOID srand(unsigned int);

/* For BSD compatibility */
long random(VOID);
int srandom(int);
char *setstate(char *);
char *initstate(unsigned, char *, int);

/* Memory management functions */
LONG* calloc(size_t, size_t);		/* (nmemb, size) */
VOID  free(LONG*);			/* (ptr) */
LONG* malloc(size_t);			/* (size) */
LONG* realloc(LONG*, size_t);		/* (ptr, size) */

/* Environment functions */
VOID abort(VOID);
int atexit(VOID (*)(VOID));		/* (func) */
VOID exit(int);				/* (status) */
char *getenv(const char *);		/* (name) */
int system(const char *);		/* (string) */

/* Integer Arithmetic functions */
int abs(int);
div_t div(int, int);			/* (numer, denom) */
long labs(long);
ldiv_t ldiv(long, long);		/* (numer, denom) */

/* Multibyte Character functions */
int mblen(const char *, size_t);	/* (s, n) */
int mbtowc(wchar_t *, const char *, size_t);	/* (pwc, s, n) */
int wctomb(char *, wchar_t);		/* (s, wchar) */

/* Multibyte String functions */
size_t mbstowcs(wchar_t *, const char *, size_t);	/* (pwcs, s, n) */
size_t wcstombs(char *, const wchar_t *, size_t);	/* (s, pwcs, n) */

#ifndef _ANSI_SOURCE
int      putenv (const char *);
int      setenv (const char *, const char *, int);
#endif /* not ANSI */

/* KCC Internal implementation declarations */
extern int _n_exit_func;		/* # of registered exit functions */
extern VOID (*_exit_func[])(VOID);	/* registered exit functions ptrs */

/* Public getopt */
extern	 char *optarg;			/* getopt(3) external variables */
extern	 int optind;
extern	 int opterr;
int	 getopt (int, char * const *, const char *);

#endif

#endif /* ifndef _STDLIB_INCLUDED */
