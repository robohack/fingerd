/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ident	"@(#)fingerd:$Name:  $:$Id: strdup.c,v 1.3 1999/01/17 02:01:16 woods Exp $"

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)strdup.c	8.1 (Berkeley) 6/4/93";
static char NetBSD_rcsid[] = "@(#)strdup.c,v 1.9 1998/10/13 20:27:55 kleink Exp ";
#endif /* LIBC_SCCS and not lint */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_SYS_CDEFS_H
# include <sys/cdefs.h>
#endif

#ifdef STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#else
char *malloc();
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
# endif
#endif

char *
strdup(str)
	const char *str;
{
	size_t len;
	char *copy;

	len = strlen(str) + 1;
	if (!(copy = malloc(len)))
		return (NULL);
	memcpy(copy, str, len);
	return (copy);
}
