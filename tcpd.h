 /*
  * hacked version that's useful with just rfc931.c
  */

#ident	"@(#)fingerd:$Name:  $:$Id: tcpd.h,v 1.1 1997/09/15 00:40:34 woods Exp $"

#define RFC931_TIMEOUT	10		/* seconds */
#define STRING_LENGTH	128		/* as per tcp_wrappers 7.6 */

#define STRN_CPY(d,s,l) { strncpy((d),(s),(l)); (d)[(l)-1] = 0; }

#define tcpd_warn(str)	syslog(LOG_ERR, str) /* XXX very limited form!!!! */

extern char unknown[];			/* defined in fingerd.c */

extern void rfc931();			/* client name from RFC 931 daemon */