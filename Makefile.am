#
#	Makefile.am - input for automake
#
#ident	"@(#)$Name:  $:$Id: Makefile.am,v 1.9 1999/01/17 01:49:15 woods Exp $"

AUTOMAKE_OPTIONS = gnits no-dependencies cvs cvs-testbuild dist-tarZ
MAINT_CHARSET = latin1

SUBDIRS = samples

DIST_OTHER = README.wildcard

EXTRA_DIST = fingerd.txt

libexec_PROGRAMS = fingerd

fingerd_SOURCES = \
	fingerd.h fingerd.c access.c wildcard.c misc.c version.c \
	rfc931.c tcpd.h

fingerd_LDADD = @LIBOBJS@

man_MANS = fingerd.8

INCLUDES = -I.

FLAGS_TO_PASS = \
	'CC=$(CC)' \
	'CFLAGS=$(CFLAGS)' \
	'LDFLAGS=$(LDFLAGS)'

# Mention here files which are created using '--add-missing'
MAINTAINERCLEANFILES = install-sh missing mkinstalldirs INSTALL README.developers

##
##	local extensions to automake
##

#
# Used below in special install-symlinks hooks
#
syslibexecdir = @syslibexecdir@

#
# special rules for safely replacing vendor version with a symlink....
#

install-symlinks: install-symlink-sbinPROGRAMS

install-symlink-sbinPROGRAMS:
	for p in $(sbin_PROGRAMS); do					\
	  if test -f $$p; then						\
	    pname=`echo "$$p" | sed '$(transform)'` ;			\
	    if [ ! -h $(syslibexecdir)/$$pname -a ! -f $(syslibexecdir)/$${pname}-ORIG ] ; then	\
	      mv $(syslibexecdir)/$$pname $(syslibexecdir)/$${pname}-ORIG ; \
	    fi ;							\
	    rm -f $(syslibexecdir)/$$pname ;				\
	    ln -s $(libexecdir)/$$pname $(syslibexecdir)/$$pname ;		\
	  fi ;								\
	done

NROFF = nroff
TROFF = troff
ROFFDEV = -Tps
PIC = pic
TBL = tbl
SOELIM = soelim

SUFFIXES = .8 .ps .txt

.8.txt:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(NROFF) -mdoc > $@

.8.ps:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(TROFF) -mdoc $(ROFFDEV) > $@

#
# hard-coded dependencies
#

$(PROGRAMS): Makefile

$(man_MANS): config.status

fingerd.o: fingerd.h tcpd.h config.h
access.o: fingerd.h config.h
misc.o: fingerd.h config.h
rfc931.o: fingerd.h tcpd.h config.h
wildecard.o: config.h
version.o: config.h
