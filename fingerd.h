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

#ifndef FINGERD_SYSLOG_FACILITY
# define FINGERD_SYSLOG_FACILITY LOG_DAEMON /* default syslog facility */
#endif

typedef enum access_e {
	ACCESS_GRANTED =	0,	/* acl file missing */
	ACCESS_DENIED =		(1U <<  1),
	ACCESS_FORWARD =	(1U <<  2),
	ACCESS_NOLIST =		(1U <<  3),
	ACCESS_NOGECOS =	(1U <<  4),
	ACCESS_NOHOME =		(1U <<  5),
	ACCESS_NOMATCH =	(1U <<  6),
	ACCESS_NOPLAN =		(1U <<  7),
	ACCESS_FORCEIDENT =	(1U <<  8),
	ACCESS_FORCESHORT =	(1U <<  9),
	ACCESS_DEFAULTSHORT =	(1U << 10),
	ACCESS_SHOWHOST =	(1U << 11),
} access_e;

extern char    *confdir;
extern char     version[];

/*
 * these files are found in confdir, by default _PATH_SYSCONFDIR.
 * See misc.c:conf_file_path() for more info.
 */
#define FINGERD_ACCESS_FILE	"fingerd.acl"
#define FINGERD_MOTD_FILE	"fingerd.motd"
#define FINGERD_USERS_FILE	"fingerd.users"

#define NO_IDENT_DONE		"[no-ident]"
#define NO_USER_SPECIFIED	"[userlist]"

#include <sys/cdefs.h>

#ifndef __dead
# define __dead
#endif

__BEGIN_DECLS
access_e        access_check __P((char *name, char *host));
void            run_program __P((char *program, char **args)) __dead;
int             run_user_cmd __P((char *user, char *ruser, char *rhost));
char           *conf_file_path __P((const char *));
char           *get_ident __P((int fd, int timeout));
__END_DECLS
