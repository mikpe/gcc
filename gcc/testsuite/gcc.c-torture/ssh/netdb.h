/* <NETDB.H> - definitions for BSD network library routines
**
**	(c) Copyright Ken Harrenstien 1989
**
**	The BSD network routines are only partially supported.
*/

#ifndef _NETDB_INCLUDED
#define _NETDB_INCLUDED

/* gethost family */
#define gethostent	gthent	/* linker disambiguation */
#define gethostbyname	gthnam
#define gethostbyaddr	gthadr

#define gethostid	gthsid
#define gethostname	gthstn

#define sethostent	sthste

struct hostent {
    char *h_name;		/* official name of host */
    char **h_aliases;		/* alias list */
    int h_addrtype;		/* address type */
    int h_length;		/* length of address */
    char **h_addr_list;		/* list of addresses from name server */
};
#define h_addr  h_addr_list[0]  /* address, for backward compatibility */

#define AF_INET		2	/* internet address type */
#define INET_ADDR_SIZE	4	/* size (in chars) of one */

#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif

/* cgen:gethst.c */
extern struct hostent *gethostbyname P_((char *name));
extern struct hostent *gethostbyaddr P_((char *addr,int len,int type));
extern struct hostent *gethostent P_((VOID));
extern VOID sethostent P_((int stayopen));
extern VOID endhostent P_((VOID));
extern long gethostid P_((VOID));
extern int gethostname P_((char *name,int namelen));

/* getnetent family */
#define setnetent	stnete
#define endnetent	ennete
#define getnetent	gtnete
#define getnetbyname	gtnetn
#define getnetbyaddr	gtneta

#define _PATH_NETWORKS	"/etc/networks"

struct  netent {
        char            *n_name;        /* official name of net */
        char            **n_aliases;    /* alias list */
        int             n_addrtype;     /* net address type */
        unsigned long   n_net;          /* network # */
      };

/* getnetent.c */
extern VOID setnetent P_((int f));
extern VOID endnetent P_((VOID));
extern struct netent *getnetent P_((VOID));
extern struct netent *getnetbyname P_((const char *name));
extern struct netent *getnetbyaddr P_((long net,int type));

/* getserv family */
#define setservent	stsrve
#define endservent	ensrve
#define getservent	gtsrve
#define getservbyname	gtsrvn
#define getservbyport	gtsrvp

#define _PATH_SERVICES  "/etc/services"

struct  servent {
        char    *s_name;        /* official service name */
        char    **s_aliases;    /* alias list */
        int     s_port;         /* port # */
        char    *s_proto;       /* protocol to use */
      };

/* getserv.c */
extern VOID setservent P_((int f));
extern VOID endservent P_((VOID));
extern struct servent *getservent P_((VOID));
extern struct servent *getservbyname P_((const char *name,const char *proto));
extern struct servent *getservbyport P_((int port,const char *proto));

/* getproto family */
#define getprotobynumber	gtprnm
#define setprotoent	stprot
#define endprotoent	enprot
#define getprotoent	gtprot
#define getprotobyname	gtpron

#define _PATH_PROTOCOLS  "/etc/protocols"

struct  protoent {
        char    *p_name;        /* official protocol name */
        char    **p_aliases;    /* alias list */
        int     p_proto;        /* protocol # */
      };

/* getproto.c */
extern struct protoent *getprotobynumber P_((int proto));
extern VOID setprotoent P_((int f));
extern VOID endprotoent P_((VOID));
extern struct protoent *getprotoent P_((VOID));
extern struct protoent *getprotobyname P_((const char *name));

#undef P_

#endif /* ifndef _NETDB_INCLUDED */
