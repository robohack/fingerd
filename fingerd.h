/*
 * Copyright 1994 Michael Shanzer.  All rights reserved.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * Use of this software constitutes acceptance for use in an AS IS
 * condition. There are NO warranties with regard to this software.
 * In no event shall the author be liable for any damages whatsoever
 * arising out of or in connection with the use or performance of this
 * software.  Any use of this software is at the user's own risk.
 *
 * The origin of this software must not be misrepresented, either by
 * explicit claim or by omission.  Since few users ever read sources,
 * credits must appear in the documentation.
 *
 * If you make modifications to this software that you feel
 * increases it usefulness for the rest of the community, please
 * email the changes, enhancements, bug fixes as well as any and
 * all ideas to me. This software is going to be maintained and
 * enhanced as deemed necessary by the community.
 *
 *              Michael S. Shanzer
 *  		shanzer@foobar.com
 *
 */

#define	FINGER_PATH		"/usr/ucb/finger"
#define FINGER_MOTD		"/usr/local/lib/finger/motd"
#define	FINGER_ACL		"/usr/local/lib/finger/access"
#define	FINGER_USERS		"/usr/local/lib/finger/users"
#define FINGER_SYSLOG		LOG_LOCAL3
#define	IDENT_TIMEOUT		30		/* ident timeout */

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#define		ACCESS_DENIED		0x0000
#define		ACCESS_GRANTED		0x0001
#define		ACCESS_NOLIST		0x0002
#define		ACCESS_NOFORWARD	0x0004
#define		ACCESS_FORCEIDENT	0x0008
#define		ACCESS_NOMATCH		0x0010

void		err(const char *, ...);
unsigned long	access_check(char *name, char *host);
long		execute(char *program, char **args);
long		execute_user_cmd(char *user, char *ruser, char *rhost);
char		*get_ident(int fd, int timeout);
