@TOP@

#ident	"@(#)fingerd:$Name:  $:$Id: acconfig.h,v 1.1 1997/04/05 23:46:01 woods Exp $"

/* these are really just for automake */
#undef PACKAGE
#undef VERSION

/* HAVE_SYSLOG HAVE_SOCKET -- tricks to fool autoheader 2.7 which should
 * supposedly not check for these symbols when there's an AC_CHECK_FUNCS
 */

/* probably only used in Makefile: syslibexecdir */
