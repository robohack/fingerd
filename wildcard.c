/*
 * Copyright (c) 1994 Powerdog Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *	This product includes software developed by Powerdog Industries.
 * 4. The name of Powerdog Industries may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY POWERDOG INDUSTRIES ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE POWERDOG INDUSTRIES BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1994 Powerdog Industries.  All rights reserved.";
static char sccsid[] = "@(#)wildcard.c	1.1 (Powerdog) 94/11/08";
#endif /* not lint */

#include <string.h>

#ifdef REGEX_COMPAT
#define wc_comp	re_comp
#define wc_exec	re_exec
#endif /* REGEX_COMPAT */

#define WC_MAX	1024

#define CEND	1
#define CANY	2
#define CANYC	3
#define CRANGE	4
#define CNRANGE	5
#define CTEXT	6

static char	*longerr = "Wildcard expression too long",
		buf[WC_MAX] = { CEND };

char	*
wc_comp(s)
	char	*s;
{
	char	*p,
		*x,
		*ep;
	int	i;

	if (s == 0 || *s == 0) {
		if (buf[0] == CEND)
			return "No previous wildcard expression";
		return 0;
	}

	ep = &buf[WC_MAX - 2];	/* must fit at least CEND */

	p = buf;

	while (*s != 0)
		switch (*s++) {
		case '*':
			while (*s == '*')
				s++;
			if (p == ep) {
				buf[0] = CEND;
				return longerr;
			}
			*p++ = CANY;
			break;

		case '?':
			if (p == ep) {
				buf[0] = CEND;
				return longerr;
			}
			*p++ = CANYC;
			break;

		case '[':
			if (p == ep - 1) {
				buf[0] = CEND;
				return longerr;
			}
			if (*s == '^') {
				s++;
				*p++ = CNRANGE;
			} else
				*p++ = CRANGE;
			x = p++;
			if (*s == ']') {
				i = 1;
				if (p == ep) {
					buf[0] = CEND;
					return longerr;
				}
				*p++ = *s++;
			} else
				i = 0;
			while (i < 255 && *s != 0 && *s != ']') {
				if (p == ep) {
					buf[0] = CEND;
					return longerr;
				}
				*p++ = *s++;
				i++;
			}
			if (i == 0) {
				buf[0] = CEND;
				return "Premature end of range";
			}
			*x = (char) i;
			if (*s++ != ']') {
				buf[0] = CEND;
				return "Missing ]";
			}
			break;

		default:
			if (p == ep - 1) {
				buf[0] = CEND;
				return longerr;
			}
			*p++ = CTEXT;
			x = p++;
			s--;
			i = 0;
			while (i < 255 && *s != 0 &&
			       *s != '*' && *s != '?' && *s != '[') {
				if (p == ep) {
					buf[0] = CEND;
					return longerr;
				}
				if (*s == '\\' && *++s == 0)
					break;
				*p++ = *s++;
				i++;
			}
			*x = (char) i;
			break;
		}

	*p = CEND;
	return 0;
}

static int
wc_subexec(s, p)
	char	*s;
	char	*p;
{
	int	i,
		n;

	while (*s != 0 && *p != CEND)
		switch (*p++) {
		case CTEXT:
			i = (int) *p++;
			if (strncmp(s, p, i) != 0)
				return 0;
			s += i;
			p += i;
			break;

		case CANY:
			if (*p == CEND)
				return 1;
			while (*s != 0) {
				if (wc_subexec(s, p) == 1)
					return 1;
				s++;
			}
			return 0;

		case CANYC:
			s++;
			break;

		case CRANGE:
		case CNRANGE:
			n = (int) *(p - 1);
			i = (int) *p++;
			if (*p == '-' && *s != '-') {
				p++;
				i--;
			}
			while (i > 0) {
				if (*p == *s)
					break;
				if (*p == '-' && i > 1)
					if (*s > *(p - 1) && *s < *(p + 1))
						break;
				p++;
				i--;
			}
			if ((n == CRANGE && i == 0) ||
			    (n == CNRANGE && i != 0))
				return 0;
			s++;
			p += i;
			break;
		}

	return *s == 0 && *p == CEND ? 1 : 0;
}

int
wc_exec(s)
	char *s;
{
	if (buf[0] == CEND)
		return -1;

	return wc_subexec(s, buf);
}
