#ident	"@(#)fingerd:$Name:  $:$Id: acconfig.h,v 1.7 2000/12/02 03:52:29 woods Exp $"

@TOP@

/* The name of the package. */
#define PACKAGE "fingerd"

/* The package release identifier. */
#define VERSION	"Pre-release"

/* path to the finger program */
#define PATH_FINGER		"/usr/bin/finger"

/* path to the system config directory */
#define PATH_SYSCONFDIR		"/etc"

/* do we have inet_ntoa()? */
#undef HAVE_INET_NTOA

/* do we have socket()? */
#undef HAVE_SOCKET

/* do we have syslog()? */
#undef HAVE_SYSLOG

@BOTTOM@
/*
 * End Of File.
 */
