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
 * fingerd access check routines
 */
 
#ifndef lint
static char rcsid[] = "$Id: access.c,v 1.2 1995/08/11 21:25:18 woods Exp $";
#endif /* not lint */

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include "fingerd.h"

unsigned long
access_check(user, host)
	char           *user;
	char           *host;
{

	FILE		*fp;
	char		buf[BUFSIZ],
			*line = NULL,
			*cp,
			*tp;
	unsigned long	ret = ACCESS_DENIED;
	int		match = FALSE;
		

	if ((fp = fopen(FINGER_ACL, "r")) == NULL)
		return ACCESS_GRANTED;
	if (host == NULL) {
		fclose(fp);
		return ACCESS_DENIED;
	}
	while (fgets(buf, sizeof(buf) -1, fp)) {
		if (buf[0] == NULL || buf[0] == '#')
			continue;
		if ((cp = strtok(buf, "\r\n")) == NULL)
			continue;
		if (line != NULL)
			free(line);
		if ((line = strdup(cp)) == NULL)
			continue;
		if ((cp = strtok(line, ":\t ")) == NULL)
			continue;
		if ((tp = strchr(cp, '@')) != NULL) {
			*tp = NULL;
			if (wc_comp(cp) != NULL)
				continue;
			if (!wc_exec(user))
				continue;
			cp = ++tp;
		}
		if (wc_comp(cp) != NULL)
			continue;
		if (!wc_exec(host))
			continue;
		match = TRUE;
		break;
	}
	fclose(fp);
	if (!match)
		return ACCESS_DENIED;
	ret = ACCESS_DENIED;
	while ((cp = strtok((char *) 0, ",:\t ")) != NULL) {
		if (strcasecmp(cp, "none") == 0)
			return ACCESS_DENIED;
		if (strcasecmp(cp, "all") == 0)
			return ACCESS_GRANTED;
		if (strcasecmp(cp, "noforward") == 0) {
			ret = ret | ACCESS_NOFORWARD;
			continue;
		}
		if (strcasecmp(cp, "nolist") == 0) {
			ret = ret | ACCESS_NOLIST;
			continue;
		}
		if (strcasecmp(cp, "forceident") == 0) {
			ret = ret | ACCESS_FORCEIDENT;
			continue;
		}
		if (strcasecmp(cp, "nomatch") == 0) {
			ret = ret | ACCESS_NOMATCH;
			continue;
		}
	}
	if (line != NULL) free(line);
	return ret;
}
		

			
		
		
