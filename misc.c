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

#ident	"@(#)fingerd:$Name:  $:$Id: misc.c,v 1.12 2000/12/02 03:53:54 woods Exp $"

/*
 * misc. routines
 */
 
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
#else
extern void exit ();
extern char *getenv();
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined(HAVE_STRING_H) || defined(STDC_HEADERS)
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
# include <strings.h>
extern char *strchr(), *strrchr(), *strtok();
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#else
# ifndef errno
extern int errno;
# endif /* !errno */
#endif /* HAVE_ERRNO_H */

#include <stdio.h>
#include <syslog.h>

#include "fingerd.h"

/*
 * this should be selected by GNU autoconf....
 */
#ifndef HAVE_ERR

#if defined(__STDC__)
# if (__STDC__ - 0) > 0				/* just to be sure! */
#  define HAVE_VPRINTF		1
# endif
#else
# define HAVE_VPRINTF		1		/* use GNU autoconf! */
#endif

#ifdef HAVE_VPRINTF
# if (__STDC__ - 0) > 0
#  include	<stdarg.h>
#  define VA_START(args, lastarg)	va_start(args, lastarg)
# else
#  include	<varargs.h>
#  define VA_START(args, lastarg)	va_start(args)
# endif
#else
# define va_alist	a1, a2, a3, a4, a5, a6, a7, a8, a9
# define va_dcl		char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, a9;
#endif

/* VARARGS */
#if defined(HAVE_VPRINTF) && ((__STDC__ - 0) > 0)
void
err(const char *fmt, ...)
#else
void
err(fmt, va_alist)
	char *fmt;
	va_dcl
#endif
{
	char	buf[BUFSIZ*2];
#ifdef HAVE_VPRINTF
	va_list ap;
#endif

#ifdef HAVE_VPRINTF
	VA_START(ap, fmt);
	(void) vsprintf(buf, fmt, ap);
	va_end(ap);
#else
	sprintf(buf, fmt, va_alist);
#endif
	(void) syslog(LOG_ERR, buf);
	exit(1);
}
#endif /* HAVE_ERR */

char *
conf_file_path(cf)
	char	*cf;
{
	char	*pn;

	if (!(pn = malloc(strlen(cf) + 1 + strlen(confdir))))
		err("malloc(): %s", strerror(errno));

	return (strcat(strcat(strcpy(pn, confdir), "/"), cf));
}

int
execute(program, args)
	char	*program;
	char	**args;
{
	int	p[2],
		ch;
	FILE	*fp;
	
	if (pipe(p) < 0) 
		err("pipe: %s", strerror(errno));
	switch (fork()) {
	case 0:
		(void) close(p[0]);
		if (p[1] != 1) {
			(void) dup2(p[1], 1);
			(void) close(p[1]);
		}
		execv(program, args);
		err("execv: %s: %s", program, strerror(errno));
	case -1:
		err("fork: %s", strerror(errno));
	}
	(void) close(p[1]);
	if (!(fp = fdopen(p[0], "r")))
		err("fdopen: %s", strerror(errno));
	while ((ch = getc(fp)) != EOF) {
		if (ch == '\n')
			putchar('\r');
		putchar(ch);
	}
	return 1;
}

int
execute_user_cmd(name, ruser, rhost)
	char           *name;
	char           *ruser;
	char           *rhost;
{
	FILE           *fp;
	int             ac = 0;
	char            buf[BUFSIZ];
	char           *cp;
	char           *prog;
	char           *line = NULL;
	char           *av[BUFSIZ];

	if (name == NULL) {
		if (!(name = strdup(NO_USER_SPECIFIED)))
			err("strdup: no memory - %s", strerror(errno));
	}
	if ((fp = fopen(conf_file_path(FINGERD_USERS_FILE), "r")) == NULL) {
#ifdef DEBUG
		if (debug)
			fprintf(stderr, "%s: open(%s) failed: %s.\n", argv0, pn_acl, strerror(errno));
#endif
		return 0;
	}
	while (fgets(buf, sizeof(buf) -1, fp)) {
		if (buf[0] == NULL || buf[0] == '#') /* ignore comments */
			continue;
		if ((cp = strtok(buf, "\n")) == NULL) /* trim newlines */
			continue;
		if (line != NULL)
			free(line);
		if (!(line = strdup(cp)))
			err("strdup: %s", strerror(errno));
		if ((cp = strtok(line, ":")) == NULL)
			continue;
		if (strcasecmp(cp, name))
			continue;
		else {
			if ((cp = strtok((char *) NULL, " \t")) == NULL)
				continue;
			prog = strdup(cp);
			av[ac++] = strdup(cp);
			while ((cp = strtok((char *) NULL, " ")) != NULL) {
				if (!strncmp(cp, "%%", 2)) {
					if (!(av[ac++] = strdup(cp+1)))
						err("strdup: %s", strerror(errno));
					continue;
				}
				if (!strncmp(cp, "%H", 2)) {
					if (!(av[ac++] = strdup(rhost)))
						err("strdup: %s", strerror(errno));
					continue;
				}
				if (!strncmp(cp, "%U", 2)) {
					if (!(av[ac++] = strdup(ruser)))
						err("strdup: %s", strerror(errno));
					continue;
				}
				if (!(av[ac++] = strdup(cp)))
					err("strdup: %s", strerror(errno));
			}
			av[ac++] = NULL;
			return execute(prog, av);
		}
	}
	return 0;
}

