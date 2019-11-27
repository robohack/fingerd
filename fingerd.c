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

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_LIBWRAP
# include <tcpd.h>			/* libwrap, for rfc931() */
#endif

#include "fingerd.h"

char           *argv0 = PACKAGE_NAME;
char           *confdir = _PATH_SYSCONFDIR;

extern char    *optarg;
extern int      optind;
extern int      opterr;

static void usage __P((void));

int main __P((int, char *[]));

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
	bool            eightbit = false;
	bool            logging = false;
	bool            forward = false;
	bool            nogecos = false;
	bool            nohome = false;
	bool            nolist = false;
	bool            noplan = false;
	bool            showhost = false;
	bool            doident = false;
	bool            forceident = false;
	bool            forceresolv = false;
	bool            forceshort = false;
	bool            defaultshort = false;
	bool            nomatch = false;
	int             ac = 1;
	int             ch;
	socklen_t       sval;
	char           *ap;
	char           *prog_finger;
	char           *rhost;
	char           *ruser = NULL;
	char            line[BUFSIZ];
	char           *av[BUFSIZ];

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
	prog_finger = _PATH_FINGER;
	openlog(argv0, LOG_PID | LOG_PERROR, FINGERD_SYSLOG_FACILITY);
	optind = 1;		/* Start options parsing */
	opterr = 0;
	while ((ch = getopt(argc, argv, "8bc:fghiIlmpP:rsSuV")) != EOF) {
		switch (ch) {
		case '8':
			eightbit = true;
			break;
		case 'b':
			nohome = true;
			break;
		case 'c':
			confdir = optarg;
			break;
		case 'f':
			forward = true;
			break;
		case 'g':
			nogecos = true;
			break;
		case 'h':
			showhost = true;
			break;
		case 'i':
			doident = true;
#ifndef HAVE_LIBWRAP
			syslog(LOG_WARN, "libwrap not supported -- ident impossible!");
#endif
			break;
		case 'I':
#ifndef HAVE_LIBWRAP
			syslog(LOG_ERR, "libwrap not supported -- ident impossible!");
#endif
			forceident = true;
			break;
		case 'l':
			logging = true;
			break;
		case 'm':
			nomatch = true;
			break;
		case 'p':
			noplan = true;
			break;
		case 'P':
			prog_finger = optarg;
			break;
		case 'r':
			forceresolv = true;
			break;
		case 's':
			defaultshort = true;
			break;
		case 'S':
			forceshort = true;
			break;
		case 'u':
			nolist = true;
			break;
		case 'V':
			puts(version);
			exit(0);
		case '?':
			usage();
			/* NOTREACHED */
		default:
			syslog(LOG_ERR, "illegal option -- %c", ch);
			usage();
			/* NOTREACHED */
		}
	}
	if (optind != argc) {
		usage();
		/* NOTREACHED */
	}
	sval = sizeof(sin);
	if (getpeername(0, (struct sockaddr *) &sin, &sval) < 0) {
		syslog(LOG_ERR, "getpeername: %m");
		exit(1);
		/* NOTREACHED */
	}
	if (getsockname(0, (struct sockaddr *) &laddr, &sval) < 0) {
		syslog(LOG_ERR, "getsockname: %m");
		exit(1);
		/* NOTREACHED */
	}

#ifdef HAVE_LIBWRAP
	if (doident || forceident) {
		char idreply[STRING_LENGTH + 1];

		rfc931((struct sockaddr *) &sin, (struct sockaddr *) &laddr, idreply); /* XXX should use ident(3) et al */
		if ((ruser = strdup(idreply)) == NULL) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	} else
#endif
	{
		if ((ruser = strdup(NO_IDENT_DONE)) == NULL) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (fgets(line, sizeof(line), stdin) == NULL) { /* do not use getline(3)! */
#ifdef DEBUG
		/* can be too noisy for regular use? */
		/* usually just indicates EOF? */
		syslog(LOG_ERR, "fgets: %m");
#endif
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
		if (!(rhost = strdup(hp->h_name))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
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
			if (!(rhost = strdup(inet_ntoa(sin.sin_addr)))) {
				syslog(LOG_ERR, "strdup: %m");
				exit(1);
				/* NOTREACHED */
			}
		} else {
			struct	in_addr *i;
			int match = false;

			while ((i = (struct in_addr *) *hp->h_addr_list++)) {
				if (i->s_addr == sin.sin_addr.s_addr) {
					match = true;
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
		if (!(rhost = strdup(inet_ntoa(sin.sin_addr)))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
		if (forceresolv) {
			syslog(LOG_NOTICE, "from=%s@[%s] to=[unknown] stat=Cannot resolve hostname",
			       ruser, rhost);
			printf("Sorry, I cannot resolve your hostname from your address [%s].",
			       rhost);
			exit(1);
			/* NOTREACHED */
		}
	}
	/* N.B.:  Note we expect your inetd(8) to have already called hosts_access(3)!!! */
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
		nolist = true;
	if (perm & ACCESS_NOPLAN)
		noplan = true;
	if (perm & ACCESS_FORWARD)
		forward = true;
	if (perm & ACCESS_NOGECOS)
		nogecos = true;
	if (perm & ACCESS_NOHOME)
		nohome = true;
	if (perm & ACCESS_NOMATCH)
		nomatch = true;
	if (perm & ACCESS_FORCEIDENT)
		forceident = true;
	if (perm & ACCESS_SHOWHOST)
		showhost = true;
	if (perm & ACCESS_FORCESHORT)
		forceshort = true;
	if (perm & ACCESS_DEFAULTSHORT)
		defaultshort = true;
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
	if ((fp = fopen(conf_file_path(FINGERD_MOTD_FILE), "r"))) {
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
		if (!(av[ac++] = strdup("-s"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	} else if (ap && ap[0] == '/' && toupper(ap[1]) == 'W') {
		if (!(av[ac++] = strdup("-l"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
		ap = strtok((char *) NULL, " "); /* look for more tokens */
	} else {
		if (defaultshort) {
			if (!(av[ac++] = strdup("-s"))) {
				syslog(LOG_ERR, "strdup: %m");
				exit(1);
				/* NOTREACHED */
			}
		}
	}
	if (eightbit) {
		if (!(av[ac++] = strdup("-8"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (nomatch) {
		if (!(av[ac++] = strdup("-m"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (nogecos) {
		if (!(av[ac++] = strdup("-g"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (nohome) {
		if (!(av[ac++] = strdup("-b"))) { /* not in all finger(1)s */
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (noplan) {
		if (!(av[ac++] = strdup("-p"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
	}
	if (showhost) {
		if (!(av[ac++] = strdup("-h"))) {
			syslog(LOG_ERR, "strdup: %m");
			exit(1);
			/* NOTREACHED */
		}
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
	if (run_user_cmd(ap, ruser, rhost)) {
		exit(0);
		/* NOTREACHED */
	} else {
		if (ap) {
			if (!(av[ac++] = strdup(ap))) {
				syslog(LOG_ERR, "strdup: %m");
				exit(1);
				/* NOTREACHED */
			}
		}
	}
	av[ac] = NULL;			/* no more arguments!  ;-) */
	lp = (lp = strrchr(prog_finger, '/')) ? (lp + 1) : prog_finger;
	if (!(av[0] = strdup(lp))) {
		syslog(LOG_ERR, "strdup: %m");
		exit(1);
		/* NOTREACHED */
	}
	run_program(prog_finger, av);
	/* NOTREACHED */
}

static void
usage()
{
#ifdef HAVE_LIBWRAP
	syslog(LOG_WARNING, "Usage:  fingerd [-c -confdir] [-P -finger] [-bfglmpru] [-h | -H] [-i | -I] [-s | -S]");
#else
	syslog(LOG_WARNING, "Usage:  fingerd [-c -confdir] [-P -finger] [-bfglmpru] [-h | -H] [-s | -S]");
#endif
	exit(2);
	/* NOTREACHED */
}
