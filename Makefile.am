#
#	Makefile.am - input for automake
#
#ident	"@(#)fingerd:$Name:  $:$Id: Makefile.am,v 1.12 2000/12/02 17:35:05 woods Exp $"

AUTOMAKE_OPTIONS = gnu no-dependencies cvs cvs-testbuild dist-tarZ
MAINT_CHARSET = latin1

SUBDIRS = samples

libexec_PROGRAMS = fingerd

fingerd_SOURCES = \
	fingerd.h fingerd.c access.c wildcard.c misc.c version.c \
	rfc931.c tcpd.h

fingerd_LDADD = @LIBOBJS@

man_MANS = fingerd.8
catman_MANS = fingerd.cat8

INCLUDES = -I.

# things that exist in the repository
DIST_OTHER = \
	README.developers ToDo \
	README.wildcard fingerd.txt

# things that must be made during 'make dist'
DIST_MAKE = $(catman_MANS) Makefile.BSD-dist

EXTRA_DIST = $(DIST_OTHER) $(DIST_MAKE)

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

SUFFIXES = .8 .cat8 .ps .txt

.8.cat8:
	@rm -f $@
	$(NROFF) -mandoc $(srcdir)/$< > $@ || (rm -f $@; exit 1)

.8.txt:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(NROFF) -mdoc > $@

.8.ps:
	$(SOELIM) $(srcdir)/$? | $(TBL) | $(TROFF) -mdoc $(ROFFDEV) > $@

CLEANFILES = $(catman_MANS) $(man_MANS)

#
# hard-coded dependencies
#

$(PROGRAMS): Makefile Makefile.BSD

$(man_MANS): config.status

Makefile.BSD-dist: Makefile.BSD
	@rm -f $@
	cp $? $@

Makefile.BSD: Makefile.BSD.in config.status

fingerd.o: fingerd.h tcpd.h config.h
access.o: fingerd.h config.h
misc.o: fingerd.h config.h
rfc931.o: fingerd.h tcpd.h config.h
wildecard.o: config.h
version.o: config.h
