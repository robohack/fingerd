/*
 * Copyright 1994 Michael Shanzer.  All rights reserved.
 * Copyright 1999 Greg A. Woods <woods@planix.com.  All rights reserved.
 *
 * Permission is granted to anyone to use this software for any purpose on any
 * computer system, and to alter it and redistribute it freely, subject to the
 * following restrictions:
 *
 * Use of this software constitutes acceptance for use in an AS IS condition.
 * There are NO warranties with regard to this software.  In no event shall the
 * authors be held liable for any damages whatsoever arising out of, or in
 * connection with, the use or performance of this software.  Any use of this
 * software is strictly at the user's own risk.
 *
 * The origin of this software must not be misrepresented, either by explicit
 * claim or by omission.  Since few users ever read sources, credits must also
 * appear in the documentation.
 *
 * If you make modifications to this software that you feel increases it
 * usefulness for the rest of the community, please email the changes,
 * enhancements, bug fixes as well as any and all ideas to the authors.  This
 * software is going to be maintained and enhanced as deemed necessary by the
 * community.
 *
 *              Michael S. Shanzer
 *  		shanzer@foobar.com
 *
 *              Greg A. Woods
 *  		woods@planix.com
 */

#ident	"@(#)$Name:  $:$Id: fingerd.h,v 1.8 1999/01/17 01:15:27 woods Exp $"

#ifndef FINGERD_SYSLOG_FACILITY
# define FINGERD_SYSLOG_FACILITY LOG_DAEMON /* default syslog facility */
#endif
#ifndef	IDENT_TIMEOUT
# define IDENT_TIMEOUT		30	/* ident timeout */
#endif

#ifndef FALSE
# define FALSE	0
#endif

#ifndef TRUE
# define TRUE	1
#endif

#define ACCESS_GRANTED		0x0000	/* acl file missing */
#define ACCESS_DENIED		0x0001
#define	ACCESS_FORWARD		0x0002
#define	ACCESS_NOLIST		0x0004
#define	ACCESS_NOGECOS		0x0008
#define	ACCESS_NOHOME		0x0010
#define	ACCESS_NOMATCH		0x0020
#define	ACCESS_NOPLAN		0x0040
#define	ACCESS_FORCEIDENT	0x0080
#define	ACCESS_FORCESHORT	0x0100
#define	ACCESS_DEFAULTSHORT	0x0200
#define	ACCESS_SHOWHOST		0x0400

extern char    *confdir;
extern char     version[];

#define NO_IDENT_DONE		"[no-ident]"

#include <sys/cdefs.h>

__BEGIN_DECLS
#ifndef HAVE_ERR
void		err __P((const char *, ...));
#endif
unsigned long	access_check __P((char *name, char *host));
int		execute __P((char *program, char **args));
int		execute_user_cmd __P((char *user, char *ruser, char *rhost));
char		*get_ident __P((int fd, int timeout));
__END_DECLS
