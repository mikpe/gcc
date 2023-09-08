/* <STRING.H> and <STRINGS.H> - string routine declarations
**
**	(c) Copyright Ken Harrenstien 1989
**
** NOTE: the two files string.h and strings.h are identical copies.
** Draft Proposed ANSI C (and CARM) require <string.h>, but BSD uses
** <strings.h>.  To aVOID problems with filename lengths longer than 6 chars,
** neither includes the other; they are simply identical, so it is OK if
** something asks for <strings.h> and gets <string.h> instead.
**
** Eventually, when BSD supports ANSI C, we can flush <strings.h>.
**
** The mem*() functions used to be declared in <memory.h> but ANSI has
** moved them here, so <memory.h> now just includes <string.h>.
**
**	All of these functions are documented in
**	CARM II (H&S v2), chap 15 and 16.
*/

#ifndef _STRING_INCLUDED
#define _STRING_INCLUDED

#include "c-env.h"

#ifndef _SIZE_T_DEFINED		/* AVOID conflict with other headers */
#define _SIZE_T_DEFINED
typedef unsigned size_t;	/* Type of sizeof() (must be unsigned, ugh) */
#endif

VOID _mmset();
VOID _mmcpy();
int  _mmcmp();

char *strerror(int e);
unsigned strlen(const char *s);
char *strncat(char *s1, const char *s2, unsigned n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned n);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, unsigned n);
int strcoll(const char *s1, const char *s2);
unsigned strxfrm(char *s1, const char *s2, unsigned n);
char *strcpy(char *s1, const char *s2);
char *strncpy(char *s1, const char *s2, unsigned n);
char *strchr(const char *s, int c);
char *index(const char *s, int c);
int strpos(const char *s, int c);
char *strrchr(const char *s, int c);
char *rindex(const char *s, int c);
int strrpos(const char *s, int c);
unsigned strspn(const char *s, const char *set);
unsigned strcspn(const char *s, const char *set);
char *strpbrk(const char *s, const char *set);
char *strrpbrk(const char *s, const char *set);
char *strstr(const char *src, const char *sub);
char *strtok(char *str, const char *set);

#endif /* ifndef _STRING_INCLUDED */
