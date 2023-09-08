/*	$OpenBSD: pathnames.h,v 1.9 2001/06/23 02:34:30 markus Exp $	*/

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef ETCDIR
#define ETCDIR				"/etc"
#endif

/*
 * System-wide file containing host keys of known hosts.  This file should be
 * world-readable.
 */
#define _PATH_SSH_SYSTEM_HOSTFILE	ETCDIR "/ssh_known_hosts"
/* backward compat for protocol 2 */
#define _PATH_SSH_SYSTEM_HOSTFILE2	ETCDIR "/ssh_known_hosts2"

/*
 * Of these, ssh_host_key must be readable only by root, whereas ssh_config
 * should be world-readable.
 */
#define _PATH_SERVER_CONFIG_FILE	ETCDIR "/sshd_config"
#define _PATH_HOST_CONFIG_FILE		ETCDIR "/ssh_config"
#define _PATH_HOST_KEY_FILE		ETCDIR "/ssh_host_key"
#define _PATH_HOST_DSA_KEY_FILE		ETCDIR "/ssh_host_dsa_key"
#define _PATH_HOST_RSA_KEY_FILE		ETCDIR "/ssh_host_rsa_key"
#define _PATH_DH_MODULI			ETCDIR "/moduli"
/* Backwards compatibility */
#define _PATH_DH_PRIMES			ETCDIR "/primes"

#ifndef _PATH_SSH_PROGRAM
#define _PATH_SSH_PROGRAM		"/usr/bin/ssh"
#endif

/*
 * The directory in user\'s home directory in which the files reside. The
 * directory should be world-readable (though not all files are).
 */
#define _PATH_SSH_USER_DIR		".ssh"

/*
 * Per-user file containing host keys of known hosts.  This file need not be
 * readable by anyone except the user him/herself, though this does not
 * contain anything particularly secret.
 */
#define _PATH_SSH_USER_HOSTFILE		"~/.ssh/known_hosts"
/* backward compat for protocol 2 */
#define _PATH_SSH_USER_HOSTFILE2	"~/.ssh/known_hosts2"

/*
 * Name of the default file containing client-side authentication key. This
 * file should only be readable by the user him/herself.
 */
#define _PATH_SSH_CLIENT_IDENTITY	".ssh/identity"
#define _PATH_SSH_CLIENT_ID_DSA		".ssh/id_dsa"
#define _PATH_SSH_CLIENT_ID_RSA		".ssh/id_rsa"

/*
 * Configuration file in user\'s home directory.  This file need not be
 * readable by anyone but the user him/herself, but does not contain anything
 * particularly secret.  If the user\'s home directory resides on an NFS
 * volume where root is mapped to nobody, this may need to be world-readable.
 */
#define _PATH_SSH_USER_CONFFILE		".ssh/config"

/*
 * Relevant only when using builtin PRNG.
 */
#ifndef SSH_PRNG_SEED_FILE
# define SSH_PRNG_SEED_FILE      _PATH_SSH_USER_DIR"/prng_seed"
#endif /* SSH_PRNG_SEED_FILE */
#ifndef SSH_PRNG_COMMAND_FILE
# define SSH_PRNG_COMMAND_FILE   ETCDIR "/ssh_prng_cmds"
#endif /* SSH_PRNG_COMMAND_FILE */
