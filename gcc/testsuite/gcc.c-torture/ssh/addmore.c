/* various utilities for the TOPS port */

#include "includes.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


#ifndef TOPS_HEADERS
#include "sys/types.h"
#include "sys/socket.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "sys/fcntl.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "errno.h"

typedef struct sockaddr_in SAddr; 
#endif /* TOPS_HEADERS */

#include "addmore.h"
#include "entropy.h"
#include "log.h"

/* various utilities for the TOPS port */

VOID ERR_PortProblem( char *file, int line, char *message )
{
   eprintf("%s - at %s:%d\r\n", message, file, line );
}

int
makeTelnetConnection()
{
   int fd2;
   SAddr him;

   fd2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (fd2 < 0)
   {
      printf("failed to get telnet socket\r\n");
      exit(0);
   }

   /* connect */
   memset(&him, 0, sizeof (him) );

   him.sin_family = AF_INET;

#ifdef TOPS20
   him.sin_port = (unsigned short)0x0017;
   him.sin_addr.s_addr = 0x7f000001;
#else
   him.sin_port = (unsigned short)0x1700;
   him.sin_addr.s_addr = 0x0100007f;
#endif

   if (connect(fd2, (struct sockaddr *)&him, sizeof (him) ) < 0)
   {
      printf("failed to do telnet connect %d\r\n", fd2);
      closesocket( fd2 );
      exit(0);
   }

   return fd2;
}

VOID
setproctitle(const char *fmt, ... )
{
}


/* copies from the openbsd-compat */
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
strlcpy(dst, src, siz)
   char *dst;
   const char *src;
   size_t siz;
{
   register char *d = dst;
   register const char *s = src;
   register size_t n = siz;
   size_t r = 0;

   /* Copy as many bytes as will fit */
   if (n != 0 && --n != 0) {
      do {
         if ((*d++ = *s++) == 0)
            break;
	 ++r;
      } while (--n != 0);
   }

   /* Not enough room in dst, add NUL and traverse rest of src */
   if (n == 0) {
      if (siz != 0)
         *d = '\0';     /* NUL-terminate dst */
      while (*s++)
	++r;
   }

   return(r); /* count does not include NUL */
}
/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t
strlcat(dst, src, siz)
   char *dst;
   const char *src;
   size_t siz;
{
   register char *d = dst;
   register const char *s = src;
   register size_t n = siz;
   size_t dlen = 0;
   size_t slen = 0;

   /* Find the end of dst and adjust bytes left but don't go past end */
   while (n-- != 0 && *d != '\0') {
      d++;
      ++dlen;
   }
   n = siz - dlen;

   if (n == 0)
      return(dlen + strlen(s));
   while (*s != '\0') {
      if (n != 1) {
         *d++ = *s;
         n--;
      }
      s++;
      ++slen;
   }
   *d = '\0';

   return(dlen + slen);  /* count does not include NUL */
}

/* perhaps one could port these to tops20 */
#ifdef TOPS20
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res)
{
  
  /* ERR_PortProblem( __FILE__, __LINE__, "Unimpl on TOPS20 "); */

  *res = (struct addrinfo *) malloc(sizeof(struct addrinfo));
  (*res)->ai_flags = 1;
  (*res)->ai_family = 2;
  (*res)->ai_protocol= 6;
  (*res)->ai_addrlen = 16;
  (*res)->ai_addr = (struct sockaddr *) malloc(sizeof(struct sockaddr));
  (*res)->ai_addr->sa_family = 2;
  (*res)->ai_addr->sa_data[0]=(5022>>8)&0xFF;
  (*res)->ai_addr->sa_data[1]=(5022)&0xFF;
  (*res)->ai_canonname = 0x0;
  (*res)->ai_next = NULL;
  
  return 0;
  
}
       
VOID freeaddrinfo(struct addrinfo *res)
{
   /* ERR_PortProblem( __FILE__, __LINE__, "Unimpl on TOPS20 "); */
}

const char *gai_strerror (int __ecode)
{
   /* ERR_PortProblem( __FILE__, __LINE__, "Unimpl on TOPS20 "); */
   return "Should not happen";
}

int getnameinfo(const struct sockaddr * __sa, socklen_t __salen, char * __host,
                socklen_t __hostlen, char * __serv, socklen_t __servlen, 
                unsigned int __flags)
{
   /* ERR_PortProblem( __FILE__, __LINE__, "Unimpl on TOPS20 "); */
   return -1;
}

#endif

#include "openssl/rand.h"
#include "openssl/rc4.h"
#include "openssl/err.h"

/* Size of key to use */
#define SEED_SIZE 20

/* Number of bytes to reseed after */
#define REKEY_BYTES  (1 << 24)

static int rc4_ready = 0;
static RC4_KEY rc4;

unsigned int arc4random(VOID)
{
   UNS8 r[4];
   static int first_time = 1;

   memset( r, 0, 4 );

   if (rc4_ready <= 0) {
      if (!first_time)
         seed_rng();
      first_time = 0;
      arc4random_stir();
   }
#if TOPS20
   /* ERR_PortProblem( __FILE__, __LINE__, "Implementation in ssl may not work on TOPS20 "); */
#endif
   RC4(&rc4, sizeof(r), (const UNS8  *)r, (UNS8 *)r);

   rc4_ready -= sizeof(r);
   
   return( (r[3]<<24)|(r[2]<<16)|(r[1]<<8)|r[0]    );
}

VOID arc4random_stir(VOID)
{
   UNS8 rand_buf[SEED_SIZE];

   memset((UNS8 *)&rc4, 0, sizeof(rc4));
   if (!RAND_bytes(rand_buf, sizeof(rand_buf)))
      fatal("Couldn't obtain random bytes (error %ld)",
          ERR_get_error());
   RC4_set_key(&rc4, sizeof(rand_buf), rand_buf);
   memset(rand_buf, 0, sizeof(rand_buf));

   rc4_ready = REKEY_BYTES;
}
