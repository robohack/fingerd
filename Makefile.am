#
#	Makefile.am - input for automake
#
#ident	"@(#)fingerd:$Name:  $:$Id: Makefile.am,v 1.1 1997/04/05 23:45:55 woods Exp $"

AUTOMAKE_OPTIONS = gnits no-dependencies cvs
MAINT_CHARSET = latin1

SUBDIRS = samples

DIST_OTHER = README.wildcard

sbin_PROGRAMS = fingerd

fingerd_SOURCES = fingerd.h fingerd.c access.c wildcard.c rfc931.c misc.c \
		pathnames.h version.h version.c

fingerd_LDADD = @LIBOBJS@

man_MANS = fingerd.8

INCLUDES = -I.

FLAGS_TO_PASS = \
	'CC=$(CC)' \
	'CFLAGS=$(CFLAGS)' \
	'LDFLAGS=$(LDFLAGS)'

##
##	local extensions to automake
##

# build a version.h file....
stamp-v: Makefile fingerd.c
	rm -f t-version.h
	echo '#define UPDATED "'"`cd $(srcdir) && $(SHELL) mdate-sh fingerd.c`"'"' > t-version.h
	echo '#define VERSION "$(VERSION)"' >> t-version.h
	if cmp -s t-version.h version.h; then	\
	  rm t-version.h;			\
	else					\
	  mv t-version.h version.h;		\
	fi
	echo timestamp > $@

version.h: stamp-v
version.o: version.h

MOSTLYCLEANFILES = t-version.h

DISTCLEANFILES = stamp-v version.h

#
# special rules for safely replacing vendor version with a symlink....
#

syslibexecdir = @syslibexecdir@

install-symlinks: install-symlink-sbinPROGRAMS

install-symlink-sbinPROGRAMS:
	for p in $(sbin_PROGRAMS); do					\
	  if test -f $$p; then						\
	    pname=`echo "$$p" | sed '$(transform)'` ;			\
	    if [ ! -h $(syslibexecdir)/$$pname -a ! -f $(syslibexecdir)/$${pname}-ORIG ] ; then	\
	      mv $(syslibexecdir)/$$pname $(syslibexecdir)/$${pname}-ORIG ; \
	    fi ;							\
	    rm -f $(syslibexecdir)/$$pname ;				\
	    ln -s $(sbindir)/$$pname $(syslibexecdir)/$$pname ;		\
	  fi ;								\
	done

# hard-coded dependencies

$(PROGRAMS): Makefile

$(man_MANS): config.status

