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
 * read config file for user to program mappings.
 */
 

#ifndef lint
static char rcsid[] = "$Id: execute.c,v 1.1 1995/08/09 18:35:20 woods Exp $";
#endif /* not lint */

#include	<errno.h>
#include	<stdio.h>

/*
 * Execute program with specified arguments
 */
execute(char *program, char **args)
{
	int	p[2],
		ch;
	FILE	*fp;
	
	if (pipe(p) < 0) 
		err("pipe: %s", strerror(errno));
	switch (vfork()) {
	case 0:
		(void) close(p[0]);
		if (p[1] != 1) {
			(void) dup2(p[1], 1);
			(void) close(p[1]);
		}
		execv(program, args);
		err("execv: %s: %s", program, strerror(errno));
	case -1:
		err("fork: %s", strerror(errno));
	}
	(void) close(p[1]);
	if (!(fp = fdopen(p[0], "r")))
		err("fdopen: %s", strerror(errno));
	while ((ch = getc(fp)) != EOF) {
		if (ch == '\n')
			putchar('\r');
		putchar(ch);
	}
}
