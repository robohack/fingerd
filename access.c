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

/*
 * fingerd access check routines
 */
 
#ident	"@(#)fingerd:$Name:  $:$Id: access.c,v 1.12 2000/12/02 03:53:53 woods Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <stdio.h>

#ifdef HAVE_ERRNO_H
# include <errno.h>
#else
# ifndef errno
extern int errno;
# endif /* !errno */
#endif /* HAVE_ERRNO_H */

#ifdef STDC_HEADERS
# include <stdlib.h>
#else
extern void exit ();
extern char *getenv();
#endif

#include <syslog.h>

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

#include "fingerd.h"

#ifdef DEBUG
static int debug = 1;
#endif

unsigned long
access_check(user, host)
	char           *user;
	char           *host;
{
	FILE           *fp;
	char            buf[BUFSIZ];
	char           *line = NULL;
	char           *cp;
	char           *tp;
	unsigned long   ret = ACCESS_GRANTED;
	int             match = FALSE;

	if (!host) {
#ifdef DEBUG
		if (debug)
			fprintf(stderr, "%s: access_check(): no host specified.\n", argv0);
#endif
		return ACCESS_DENIED;
	}
	if (!(fp = fopen(conf_file_path(FINGERD_ACCESS_FILE), "r"))) {
#ifdef DEBUG
		if (debug)
			fprintf(stderr, "%s: open(%s) failed: %s.\n", argv0, pn_acl, strerror(errno));
#endif
		return ACCESS_GRANTED;
	}
	while (fgets(buf, sizeof(buf) - 1, fp)) {
		if (!buf[0] || buf[0] == '#')
			continue;
		if (!(cp = strtok(buf, "\r\n")))
			continue;
		if (line)
			free(line);
		if (!(line = strdup(cp))) {
#ifdef DEBUG
			if (debug)
				fprintf(stderr, "%s: strdup(%s) failed: %s.\n", argv0, cp, strerror(errno));
#endif
			continue;	/* XXX maybe the next line will be shorter? */
		}
		if (!(cp = strtok(line, ":\t ")))
			continue;
		if ((tp = strchr(cp, '@'))) {
			*tp = '\0';
			if (wc_comp(cp)) /* XXX should use glob(3) */
				continue;
			if (!wc_exec(user))
				continue;
			cp = ++tp;
		}
		if (wc_comp(cp))	/* XXX should use glob(3) */
			continue;
		if (!wc_exec(host))
			continue;
		match = TRUE;
		break;
	}
	(void) fclose(fp);
	if (!match) {			/* Must always have '*' for default */
#ifdef DEBUG
		if (debug)
			fprintf(stderr, "%s: no match found for %s@%s.\n", argv0, user, host);
#endif
		return ACCESS_DENIED;
	}

	while ((cp = strtok((char *) NULL, ",:\t "))) {
		if (strcasecmp(cp, "none") == 0)
			return ACCESS_DENIED;
		if (strcasecmp(cp, "all") == 0)
			return ACCESS_GRANTED;
		if (strcasecmp(cp, "nolist") == 0) {
			ret |= ACCESS_NOLIST;
			continue;
		}
		if (strcasecmp(cp, "forward") == 0) {
			ret |= ACCESS_FORWARD;
			continue;
		}
		if (strcasecmp(cp, "forceident") == 0) {
			ret |= ACCESS_FORCEIDENT;
			continue;
		}
		if (strcasecmp(cp, "nomatch") == 0) {
			ret |= ACCESS_NOMATCH;
			continue;
		}
		if (strcasecmp(cp, "forceshort") == 0) {
			ret |= ACCESS_FORCESHORT;
			continue;
		}
		if (strcasecmp(cp, "defaultshort") == 0) {
			ret |= ACCESS_DEFAULTSHORT;
			continue;
		}
		if (strcasecmp(cp, "showhost") == 0) {
			ret |= ACCESS_SHOWHOST;
			continue;
		}
		if (strcasecmp(cp, "noplan") == 0) {
			ret |= ACCESS_NOPLAN;
			continue;
		}
		if (strcasecmp(cp, "nogecos") == 0) {
			ret |= ACCESS_NOGECOS;
			continue;
		}
		if (strcasecmp(cp, "nohome") == 0) {
			ret |= ACCESS_NOHOME;
			continue;
		}
#ifdef DEBUG
		if (debug)
			fprintf(stderr, "%s: Unrecognized access option: %s.\n", argv0, cp);
#endif
	}
	if (line)
		free(line);

	return ret;
}
