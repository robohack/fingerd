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

#ident	"@(#)$Name:  $:$Id: fingerd.c,v 1.10 1999/01/17 01:21:59 woods Exp $"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <netdb.h>
#include <stdio.h>

#include "fingerd.h"
#ifdef HAVE_LIBWRAP
# include <tcpd.h>
int allow_severity = LOG_DEBUG;
int deny_severity = LOG_WARNING;
#else
# include "tcpd.h"			/* XXX the local one, not any system one! */
#endif

char           *confdir = PATH_SYSCONFDIR;

extern char    *optarg;
extern int      optind;
extern int      opterr;

int
main(argc, argv)
	int             argc;
	char           *argv[];
{
	struct hostent *hp;
	struct sockaddr_in sin;
	struct sockaddr_in laddr;
	unsigned long	perm = ACCESS_GRANTED;
	FILE           *fp;
	char           *lp;
	int             logging = FALSE;
	int             forward = FALSE;
	int             nogecos = FALSE;
	int             nohome = FALSE;
	int             nolist = FALSE;
	int             noplan = FALSE;
	int             showhost = FALSE;
	int             doident = FALSE;
	int             forceident = FALSE;
	int             forceresolv = FALSE;
	int             forceshort = FALSE;
	int             defaultshort = FALSE;
	int             nomatch = FALSE;
	int             ac = 1;
	int             ch;
	int             sval;
	char           *pn_motd = NULL;
	char           *ap;
	char           *prog;
	char           *rhost;
	char           *ruser = NULL;
	char            line[BUFSIZ];
	char           *av[BUFSIZ];

	prog = PATH_FINGER;
	openlog("fingerd", LOG_PID, FINGERD_SYSLOG_FACILITY);
	opterr = 0;
	while ((ch = getopt(argc, argv, "bc:fghiIlmpP:rsSuV")) != EOF) {
		switch (ch) {
		case 'b':
			nohome = TRUE;
			break;
		case 'c':
			confdir = optarg;
			break;
		case 'f':
			forward = TRUE;
			break;
		case 'g':
			nogecos = TRUE;
			break;
		case 'h':
			showhost = TRUE;
			break;
		case 'i':
			doident = TRUE;
			break;
		case 'I':
			forceident = TRUE;
			break;
		case 'l':
			logging = TRUE;
			break;
		case 'm':
			nomatch = TRUE;
			break;
		case 'p':
			noplan = TRUE;
			break;
		case 'P':
			prog = optarg;
			break;
		case 'r':
			forceresolv = TRUE;
			break;
		case 's':
			defaultshort = TRUE;
			break;
		case 'S':
			forceshort = TRUE;
			break;
		case 'u':
			nolist = TRUE;
			break;
		case 'V':
			puts(version);
			exit(0);
		case '?':
		default:
			err("illegal option -- %c", ch);
		}
	}
	sval = sizeof(sin);
	if (getpeername(0, (struct sockaddr *) &sin, &sval) < 0)
		err("getpeername: %s", strerror(errno));
	if (getsockname(0, (struct sockaddr *) &laddr, &sval) < 0)
		err("getsockname: %s", strerror(errno));

	if (doident || forceident) {
		char idreply[STRING_LENGTH + 1];

		rfc931(&sin, &laddr, idreply);
		if (!(ruser = strdup(idreply)))
			err("strdup: no memory - %s", strerror(errno));
	} else {
		if (!(ruser = strdup(NO_IDENT_DONE)))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (!fgets(line, sizeof(line), stdin)) {
		exit(1);
		/* NOTREACHED */
	}
	ap = strtok(line, "\r\n");	/* trim EOL */

	if ((hp = gethostbyaddr((char *) &sin.sin_addr.s_addr,
				sizeof(sin.sin_addr.s_addr), AF_INET))) {
		/*
		 * The following is here to try and prevent people
		 * from putting in fake IN-ADDR records.
		 */
		if (!(rhost = strdup(hp->h_name)))
			err("strdup: no memory - %s", strerror(errno));
		if (!(hp = gethostbyname(rhost))) {
			if (forceresolv) {
				syslog(LOG_NOTICE,
				       "from=%s@[%s] to='%s'; Cannot re-resolve %s",
				       ruser, ap, inet_ntoa(sin.sin_addr), rhost);
				printf("Possible name server mis-configuration.\nSorry, I cannot resolve your hostname '%s' [%s].",
				       rhost, inet_ntoa(sin.sin_addr));
				exit(1);
				/* NOTREACHED */
			}
			if (!(rhost = strdup(inet_ntoa(sin.sin_addr))))
				err("strdup: no memory - %s", strerror(errno));
		} else {
			struct	in_addr *i;
			int match = FALSE;

			while ((i = (struct in_addr *) *hp->h_addr_list++)) {
				if (i->s_addr == sin.sin_addr.s_addr) {
					match = TRUE;
					break;
				}
			}
			if (!match) {
				syslog(LOG_NOTICE,
				       "from=%s@[%s] to='%s'; host %s does not resolve to [%s].",
				       ruser, ap, inet_ntoa(sin.sin_addr), rhost, inet_ntoa(sin.sin_addr));
				printf("Name server mis-configuration.\nYour host '%s' does not have the address [%s].\n",
				       rhost, inet_ntoa(sin.sin_addr));
				exit(1);
				/* NOTREACHED */
			}
		}
	} else {
		if (!(rhost = strdup(inet_ntoa(sin.sin_addr))))
			err("strdup: no memory - %s", strerror(errno));
		if (forceresolv) {
			syslog(LOG_NOTICE, "from=%s@[%s] to=[unknown] stat=Cannot resolve hostname",
			       ruser, rhost);
			printf("Sorry, I cannot resolve your hostname from your address [%s].",
			       rhost);
			exit(1);
			/* NOTREACHED */
		}
	}
	perm = access_check(ruser, rhost);
	if (perm & ACCESS_DENIED) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s stat=Refused",
			       ruser, rhost, (ap ? ap : "[user list]"));
		}
		puts("Access to finger service denied.");
		exit(1);
		/* NOTREACHED */
	}
	/*
	 * Now we adjust options based on potentially more specific refinements
	 * in the access file.  Note that the logic is designed in such a way
	 * as to make it impossible to override command-line options from the
	 * access file.
	 */
	if (perm & ACCESS_NOLIST)
		nolist = TRUE;
	if (perm & ACCESS_NOPLAN)
		noplan = TRUE;
	if (perm & ACCESS_FORWARD)
		forward = TRUE;
	if (perm & ACCESS_NOGECOS)
		nogecos = TRUE;
	if (perm & ACCESS_NOHOME)
		nohome = TRUE;
	if (perm & ACCESS_NOMATCH)
		nomatch = TRUE;
	if (perm & ACCESS_FORCEIDENT)
		forceident = TRUE;
	if (perm & ACCESS_SHOWHOST)
		showhost = TRUE;
	if (perm & ACCESS_FORCESHORT)
		forceshort = TRUE;
	if (perm & ACCESS_DEFAULTSHORT)
		defaultshort = TRUE;
	if (forceident && (strcmp(ruser, STRING_UNKNOWN) == 0 ||
			   strcmp(ruser, NO_IDENT_DONE) == 0)) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s; no identification available",
			       ruser, rhost, (ap ? ap : "[user list]"));
		}
		if (strcmp(ruser, NO_IDENT_DONE) == 0)
			puts("Access to finger services denied.  Valid identification not available.");
		else
			puts("Access to finger services denied.  Valid identification reply required.");
		exit(1);
		/* NOTREACHED */
	}
	if (asprintf(&pn_motd, "%s/fingerd.motd", confdir) < 0)
		err("asprintf: no memory - %s", strerror(errno));
	if ((fp = fopen(pn_motd, "r"))) {
		char	mline[BUFSIZ];

		while (fgets(mline, sizeof(line), fp))
			fputs(mline, stdout);
		puts("----");
		fclose(fp);
	}
	lp = ap;			/* keep pointer to first token */
	ap = strtok(lp, " ");		/* split on possible space separator */
	if (forceshort) {
		/* ignore any '/W' request for verbose output */
		if (!(av[ac++] = strdup("-s")))
			err("strdup: no memory - %s", strerror(errno));
	} else if (ap && ap[0] == '/' && toupper(ap[1]) == 'W') {
		if (!(av[ac++] = strdup("-l")))
			err("strdup: no memory - %s", strerror(errno));
		ap = strtok((char *) NULL, " "); /* look for more tokens */
	} else {
		if (defaultshort) {
			if (!(av[ac++] = strdup("-s")))
				err("strdup: no memory - %s", strerror(errno));
		}
	}
	if (nomatch) {
		if (!(av[ac++] = strdup("-m")))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (nogecos) {
		if (!(av[ac++] = strdup("-g")))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (nohome) {
		if (!(av[ac++] = strdup("-b")))	/* not in all finger(1)s */
			err("strdup: no memory - %s", strerror(errno));
	}
	if (noplan) {
		if (!(av[ac++] = strdup("-p")))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (showhost) {
		if (!(av[ac++] = strdup("-h")))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (!forward && (ap && strchr(ap, '@'))) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s stat=Refused Forwarding",
			       ruser, rhost, ap);
		}
		puts("Finger fowarding service denied.");
		exit(1);
		/* NOTREACHED */
	}
	if (nolist && (!ap || (ap && !*ap))) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s stat=Refused",
			       ruser, rhost, "[user list]");
		}
		puts("Finger of user list denied.");
		exit(1);
		/* NOTREACHED */
	}
	if (logging) {
		syslog(LOG_NOTICE, "from=%s@%s to=%s stat=OK",
		       ruser, rhost, (ap ? ap : "[user list]"));
	}
	if (execute_user_cmd(ap, ruser, rhost)) {
		exit(0);
		/* NOTREACHED */
	} else {
		if (ap) {
			if (!(av[ac++] = strdup(ap)))
				err("strdup: no memory - %s", strerror(errno));
		}
	}
	av[ac] = NULL;			/* no more arguments!  ;-) */
	lp = (lp = strrchr(prog, '/')) ? (lp + 1) : prog;
	if (!(av[0] = strdup(lp)))
		err("strdup: no memory - %s", strerror(errno));
	if (!execute(prog, av))
		err("execute: %s", strerror(errno));

	exit(0);
	/* NOTREACHED */
}
