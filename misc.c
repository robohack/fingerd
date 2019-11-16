/*
 * Copyright 1994 Michael Shanzer.  All rights reserved.
 * Copyright 1999-2019 Greg A. Woods <woods@planix.com.  All rights reserved.
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
 * misc. routines
 */

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "fingerd.h"

char *
conf_file_path(cf)
	char	*cf;
{
	char	*pn;

	if (!(pn = malloc(strlen(cf) + 1 + strlen(confdir)))) {
		syslog(LOG_ERR, "conf_file_path(): malloc(): %m");
		exit(1);
	}

	return (strcat(strcat(strcpy(pn, confdir), "/"), cf));
}

void
run_program(program, args)
	char	*program;
	char	**args;
{
	int	p[2],
		ch;
	FILE	*fp;

	if (pipe(p) < 0) {
		syslog(LOG_ERR, "run_program(): pipe(): %m");
		exit(1);
		/* NOTREACHED */
	}
	switch (fork()) {
	case 0:
		(void) close(p[0]);
		if (p[1] != 1) {
			(void) dup2(p[1], 1);
			(void) close(p[1]);
		}
		execv(program, args);
		syslog(LOG_ERR, "run_program(): execv(): %s: %m", program);
		exit(1);
		/* NOTREACHED */
	case -1:
		syslog(LOG_ERR, "run_program(): fork(): %m");
		exit(1);
		/* NOTREACHED */
	}
	(void) close(p[1]);
	if (!(fp = fdopen(p[0], "r"))) {
		syslog(LOG_ERR, "run_program(): fdopen(): %m");
		exit(1);
		/* NOTREACHED */
	}
	while ((ch = getc(fp)) != EOF) {
		if (ch == '\n')
			putchar('\r');
		putchar(ch);
	}
	exit(0);
	/* NOTREACHED */
}

int
run_user_cmd(name, ruser, rhost)
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
		if (!(name = strdup(NO_USER_SPECIFIED))) {
			syslog(LOG_ERR, "run_user_cmd(): strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if ((fp = fopen(conf_file_path(FINGERD_USERS_FILE), "r")) == NULL) {
#ifdef DEBUG
		if (debug)
			syslog(LOG_ERR, "run_user_cmd(): open(%s) failed: %m", argv0, pn_acl);
#endif
		return 0;
	}
	while (fgets(buf, sizeof(buf) -1, fp)) {     /* xxx use getline(3)? */
		if (buf[0] == '\0' || buf[0] == '#') /* ignore comments */
			continue;
		if ((cp = strtok(buf, "\n")) == NULL) /* trim newlines */
			continue;
		if (line != NULL)
			free(line);
		if (!(line = strdup(cp))) {
			syslog(LOG_ERR, "run_user_cmd(): strdup(): %m");
			exit(1);
			/* NOTREACHED */
		}
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
					if (!(av[ac++] = strdup(cp+1))) {
						syslog(LOG_ERR, "run_user_cmd(): strdup(): %m");
						exit(1);
						/* NOTREACHED */
					}
					continue;
				}
				if (!strncmp(cp, "%H", 2)) {
					if (!(av[ac++] = strdup(rhost))) {
						syslog(LOG_ERR, "run_user_cmd(): strdup(): %m");
						exit(1);
						/* NOTREACHED */
					}
					continue;
				}
				if (!strncmp(cp, "%U", 2)) {
					if (!(av[ac++] = strdup(ruser))) {
						syslog(LOG_ERR, "run_user_cmd(): strdup(): %m");
						exit(1);
						/* NOTREACHED */
					}
					continue;
				}
				if (!(av[ac++] = strdup(cp))) {
					syslog(LOG_ERR, "run_user_cmd(): strdup(): %m");
					exit(1);
					/* NOTREACHED */
				}
			}
			av[ac++] = NULL;
			run_program(prog, av);
			/* NOTREACHED */
		}
	}
	return 0;
}
