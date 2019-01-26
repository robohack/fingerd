#ident	"@(#)fingerd:$Name:  $:$Id: acconfig.h,v 1.8 2019/01/26 22:37:32 woods Exp $"

@TOP@

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
