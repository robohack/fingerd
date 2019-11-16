/*
 * Copyright 1994 Michael Shanzer.  All rights reserved.
 * Copyright 2000-2019 Greg A. Woods <woods@planix.com.  All rights reserved.
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
 *		Michael S. Shanzer
 *		shanzer@foobar.com
 *
 *		Greg A. Woods
 *		woods@planix.com
 */

/*
 * fingerd access check routines
 */

#include <errno.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>

#include "fingerd.h"

#ifdef DEBUG
static int debug = 1;
#endif

access_e
access_check(user, host)
	char           *user;
	char           *host;
{
	FILE           *fp;
	char            buf[BUFSIZ];
	char           *line = NULL;
	char           *cp;
	char           *tp;
	access_e        ret = ACCESS_GRANTED;
	bool            match = false;

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
	while (fgets(buf, sizeof(buf) - 1, fp)) { /* XXX use getline(?)? */
		regex_t re;
		int ec;
		char eb[BUFSIZ];

		memset(&re, 0, sizeof(re));
		memset(&eb, 0, sizeof(eb));
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
			if ((ec = regcomp(&re, cp, REG_BASIC|REG_ICASE))) { /* XXX should use glob(3) */
				regerror(ec, &re, eb, sizeof(eb));
				syslog(LOG_ERR, "regcomp(%s): %s", cp, eb);
				regfree(&re);
				continue;
			}
			if (regexec(&re, user, 0, NULL, 0) != 0) {
				regfree(&re);
				continue;
			}
			regfree(&re);
			cp = ++tp;
		}
		if ((ec = regcomp(&re, cp, REG_BASIC|REG_ICASE))) {	/* XXX should use glob(3) */
			regerror(ec, &re, eb, sizeof(eb));
			syslog(LOG_ERR, "regcomp(%s): %s", cp, eb);
			regfree(&re);
			continue;
		}
		if (regexec(&re, host, 0, NULL, 0) != 0) {
			regfree(&re);
			continue;
		}
		regfree(&re);
		match = true;
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
