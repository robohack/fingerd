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
 * fingerd main loop
 */
 
#ident	"@(#)fingerd:$Name:  $:$Id: fingerd.c,v 1.7 1999/01/15 00:22:10 woods Exp $"

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
#include "tcpd.h"			/* XXX the local one, not any system one! */

char unknown[] = NO_IDENT_REPLY;

extern	char	*optarg;
extern	int	optind, opterr;

int
main(argc, argv)
	int             argc;
	char           *argv[];
{
	struct hostent		*hp;
	struct sockaddr_in	sin, laddr;
	unsigned long	perm = ACCESS_GRANTED;
	FILE		*fp;
	char		*lp;
	int		logging = FALSE,
			nolist = FALSE,
			noforward = FALSE,
			doident = FALSE,
			forceident = FALSE,
			forceresolv = FALSE,
			forceshort = FALSE,
			defaultshort = FALSE,
			nomatch = FALSE,
			ac = 1,
			ch,
			sval;
	char		*ap,
			*prog,
			*rhost,
			*ruser = NULL,
			line[BUFSIZ],
			*av[BUFSIZ];

	prog = FINGER_PATH;
	openlog("fingerd", LOG_PID, FINGER_SYSLOG);
	opterr = 0;
	while ((ch = getopt(argc, argv, "lp:ufiIsSmbrV")) != EOF) {
		switch (ch) {
		case 'l':			
			logging = TRUE;
			break;
		case 'p':			
			prog = optarg;
			break;
		case 'u':			
			nolist = TRUE;
			break;
		case 'f':			
			noforward = TRUE;
			break;		
		case 'i':
			doident = TRUE;
			break;
		case 'I':			
			forceident = TRUE;
			break;
		case 's':
			defaultshort = TRUE;
			break;
		case 'S':
			forceshort = TRUE;
			break;
		case 'm':
			nomatch = TRUE;
			break;
		case 'b':
			av[ac++] = strdup("-b");
			break;
		case 'r':
			forceresolv = TRUE;
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
		char idreply[STRING_LENGTH];

		rfc931(&sin, &laddr, idreply);
		if (!(ruser = strdup(idreply)))
			err("strdup: no memory - %s", strerror(errno));
	} else { 
		if (!(ruser = strdup(NO_IDENT_DONE)))
			err("strdup: no memory - %s", strerror(errno));
	}
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
				       "from=%s@[%s] to=[unknown] stat=Cannot re-resolve %s",
				       ruser, inet_ntoa(sin.sin_addr), rhost);
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
				       "from=%s@[%s] to=[unknown] stat=host %s does not resolve to [%s].",
				       ruser, inet_ntoa(sin.sin_addr), rhost, inet_ntoa(sin.sin_addr));
				printf("Name server mis-configuration.\nSorry, your host '%s' does not have the address [%s].\n",
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
	if (!fgets(line, sizeof(line), stdin)) {
		exit(1);
		/* NOTREACHED */
	}
	ap = strtok(line, "\r\n");

	perm = access_check(ruser, rhost); /* only after resolving is done */
	if (perm & ACCESS_DENIED) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s stat=Refused",
			       ruser, rhost, (ap ? ap : "[user list]"));
		}
		puts("Access to finger services denied.");
		exit(1);
		/* NOTREACHED */
	}
	if (perm & ACCESS_NOLIST)
		nolist = TRUE;
	if (perm & ACCESS_NOFORWARD)
		noforward = TRUE;
	if (perm & ACCESS_NOMATCH)
		nomatch = TRUE;
	if (perm & ACCESS_FORCESHORT)
		forceshort = TRUE;
	if (perm & ACCESS_DEFAULTSHORT)
		defaultshort = TRUE;
	if (perm & ACCESS_FORCEIDENT)
		forceident = TRUE;
	if (forceident && (strcmp(ruser, NO_IDENT_REPLY) == 0 ||
			   strcmp(ruser, NO_IDENT_DONE) == 0)) {
		if (logging) {
			syslog(LOG_NOTICE,
			       "from=%s@%s to=%s stat=No ident server",
			       ruser, rhost, (ap ? ap : "[user list]"));
		}
		if (strcmp(ruser, NO_IDENT_DONE) == 0)
			puts("Access to finger services denied.  Valid 'ident' not available.");
		else
			puts("Access to finger services denied.  Valid 'ident' reply required.");
		exit(1);	
		/* NOTREACHED */
	}
	lp = ap;
	ap = strtok(lp, " ");
	if (forceshort) {
		if (!(av[ac++] = strdup("-s")))
			err("strdup: no memory - %s", strerror(errno));
	} else if (ap && ap[0] == '/' && toupper(ap[1]) == 'W') {
		av[ac++] = strdup("-l");
		ap = strtok((char *) NULL, " ");
	} else {
		if (defaultshort) {
			if (!(av[ac++] = strdup("-s")))
				err("strdup: no memory - %s", strerror(errno));
		}
	}
	if (nomatch)
		av[ac++] = strdup("-m");
	if (noforward && (ap && strchr(ap, '@'))) {
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
	if ((fp = fopen(FINGER_MOTD, "r"))) {
		char	mline[BUFSIZ];

		while (fgets(mline, sizeof(line), fp))
			fputs(mline, stdout);
		puts("----");
		fclose(fp);
	}
	if (execute_user_cmd(ap, ruser, rhost)) {
		exit(0);
		/* NOTREACHED */
	} else {
		if (ap)
			av[ac++] = strdup(ap);
	}
	av[ac] = NULL;
	if ((lp = strrchr(prog, '/')) != NULL) {
		if (!(av[0] = strdup(++lp)))
			err("strdup: no memory - %s", strerror(errno));
	} else {
		if (!(av[0] = strdup(prog)))
			err("strdup: no memory - %s", strerror(errno));
	}
	if (!execute(prog, av))
		err("execute: %s", strerror(errno));

	exit(0);
	/* NOTREACHED */
}
