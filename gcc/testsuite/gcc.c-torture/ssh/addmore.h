#ifndef _ADDMORE_H_
#define _ADDMORE_H_ 1

VOID ERR_PortProblem( char *file, int line, char *message );

int
makeTelnetConnection();

VOID
setproctitle(const char *fmt, ... );

char *get_progname(char *argv0);

size_t
strlcpy(
	char *dst,
	const char *src,
	size_t siz
	);

size_t
strlcat(
	char *dst,
	const char *src,
	size_t siz
	);


unsigned int arc4random(VOID);

VOID arc4random_stir(VOID);

#endif

