#ifdef TOPS_PORT
#ifndef UNS8_DEFINED

#ifdef TOPS20
#ifdef __COMPILER_KCC__
typedef unsigned _KCCtype_char8 UNS8;
#else
typedef unsigned UNS8 __attribute__ ((size(8)));
#endif
#else
typedef unsigned char UNS8;
#endif

/* utilities for using in routines ported to TOPS20 - defined in buffer.c */
void memcpyUNS8( UNS8 *dest, const UNS8 *src, int n);
void memsetUNS8( UNS8 *dest, const int v, int n);
int memcmpUNS8( const UNS8 *s1, const UNS8 *s2, int n);
int strlenUNS8( const UNS8 *s );
int strncmpUNS8( const UNS8 *s1, const UNS8 *s2, int n);
void strcpyUNS8( UNS8 *dest, const UNS8 *src);

/* copy native string to 8-bit string */
void stUNS( UNS8 *upat, const char *pat );

/* copy 8-bit string to native string */
void sfUNS( char *pat, const UNS8 *upat );

#define UNS8_DEFINED 1
#endif /* UNS8_DEFINED */
#endif
