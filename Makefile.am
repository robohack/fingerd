#
#	Makefile.am - input for automake
#
#ident	"@(#)fingerd:$Name:  $:$Id: Makefile.am,v 1.4 1997/09/14 03:46:57 woods Exp $"

AUTOMAKE_OPTIONS = gnits no-dependencies cvs cvs-testbuild dist-tarZ
MAINT_CHARSET = latin1

SUBDIRS = samples

DIST_OTHER = README.wildcard

EXTRA_DIST = fingerd.txt

sbin_PROGRAMS = fingerd

fingerd_SOURCES = \
	fingerd.h fingerd.c access.c wildcard.c rfc931.c misc.c version.c

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

NROFF = nroff
TROFF = troff
ROFFDEV = -Tps
PIC = pic
TBL = tbl
SOELIM = soelim

SUFFIXES = .8 .ps .txt

.8.txt:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(NROFF) -man > $@

.8.ps:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(TROFF) -man $(ROFFDEV) > $@

#
# hard-coded dependencies
#

$(PROGRAMS): Makefile

$(man_MANS): config.status

fingerd.o: fingerd.h config.h
access.o: fingerd.h config.h
misc.o: fingerd.h config.h
rfc931.o: fingerd.h config.h
wildecard.o: config.h
version.o: config.h
