# -*- makefile-bsdmake -*-
#
#	Makefile.inc - Common/shared setup for BSD Makefiles
#
#	This should be included at the top of all your Makefiles
#
# Copyright (C) 2025 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# WARNING!!!  Do not put any targets in here!

# N.B.:  Nothing in here should be changed just to build this project.
#
# (Some settings can be overridden on the command-line or from environment
# variables though...)

# XXX see SJG's SSLrsh release for simple portability hacks via Make
# e.g. in particular the use of .-include "$Makefile.${MACHINE}.inc" etc.

# NOTICE:  Nothing below here _should_ be project dependent

# For testing and debugging -- at least on sane systems, or with 'bmake'
#
# (note these can also be overridden on the command line)
#
# (note:  bmake puts optimizer flags in DBG)
#
#DBG ?= -g
DBG ?=		# set to '-g' or '-g2', etc. for debugging
#OPTIM ?= -O0
OPTIM ?=	# set to '-O0', etc. for debugging

LDSTATIC ?=	# set to '-static' for static linking

# If you are making changes and want to be sure to clean up all warnings....
#
#CWARNFLAGS += -Werror
# else
NOGCCERROR = 1
WARNS = 0

# Installation directories
#
# N.B.:  ${DESTDIR} if set on the 'install' command line, it is also prepended
# to each of these target directories.
#
DESTDIR ?=	# set to the installation prefix
#
INSTALL_OPTS =	# for specific ownerships
# these will be over-ridden by pkgsrc
BINDIR ?= /bin
INCSDIR ?= /include${INCSUBDIR}
LIBDIR ?= /lib
DEBUGDIR ?= /libdata/debug
LINTLIBDIR ?= /libdata/lint
SHAREDIR ?= /share
DOCDIR ?= ${SHAREDIR}/doc
MANDIR ?= ${SHAREDIR}/man

# xxx all this otherwise some systems won't allow unprivileged installs
.if ${USER} != "root"
USER_GRP !=	id -g
BINOWN =	${USER}
BINGRP =	${USER_GRP}
LIBOWN =	${USER}
LIBGRP =	${USER_GRP}
SHAREOWN =	${USER}
SHAREGRP =	${USER_GRP}
FILESOWN =	${USER}
FILESGRP =	${USER_GRP}
NOBINMODE =	644		# else OSX bsdmake can't install!
DEBUGOWN =	${USER}
DEBUGGRP =	${USER_GRP}
MANOWN =	${USER}
MANGRP =	${USER_GRP}
DOCOWN =	${USER}
DOCGRP =	${USER_GRP}
# xxx adapt for typo in mk-20150901 through at least mk-20170418
DOGGRP =	${USER_GRP}
.endif

# more modern mk-files install a ${PROG}.debug file in libdata or whatever
#
STRIPFLAG =	# empty, for bootstrap-mk-files and older bmakes
STRIP =		# empty, for bootstrap-mk-files and older bmakes

# more hacks to avoid missing system dependencies with DESTDIR set
DPLIBC =		# empty
LIBC =			# empty
LIBCRT0 =		# empty
LIBCRTI =		# empty
LIBCRTBEGIN =		# empty
LIBCRTEND =		# empty
SHLIB_LDSTARTFILE =	# empty
SHLIB_LDENDFILE =	# empty

# note MAKECONF may override
MKHTML ?=	yes	# set to no to avoid building HTML manual pages

# Finally some common settings
#
# N.B.:  all sub-directory Makefiles must set ${bmake_topdir} to be a relative
# path to the top of the source tree.  This would not be necessary if/when all
# supported BSDMake versions have the ${.MAKE.LEVEL} feature.  ${.MAKE.LEVEL}
# was first found in NetBSD-current after 2009/09/08 but it is not found in any
# release until 6.0.  Unfortunately there is no easy way to know definitively
# what version of BSDMake is being used.  Simon Gerraty's bmake has
# MAKE_VERSION, as does FreeBSD's make though they are entirely and totally
# different and unrelated.  In NetBSD MAKE_VERSION was only briefly present
# since it represented the build date, not the release date, and it was removed
# in order to make it possible to do reproducible builds.
#
#bmake_topdir ?= # empty
#bmake_srcdir =	${.CURDIR}/${bmake_topdir}
#
#CPPFLAGS +=	-I${bmake_srcdir}

# yes, always -- we use this because it's safer when using a shared network
# directory for building/testing/etc.  (though it is sometimes somewhat
# annoying)
#
MKOBJ =			yes
OBJMACHINE ?=		yes
OBJMACHINE_ARCH ?=	yes
# XXX ideally this would be "auto", but that is not yet very widely supported.
# See Makefile.end.
MKOBJDIRS =		yes

# Default build targets -- see Makefile.end
#
# Yes, "make obj" is forced -- it is stupid to build in the source directory)
# (forcing "make depend" is also good, though a bit of a waste for pkgsrc.
#
# Note with some versions of "bmake" this will cause obj* directories to be
# created in the existing obj* directories the second time around...
#
BUILDTARGETS =	bmake-do-obj
BUILDTARGETS +=	.WAIT
BUILDTARGETS +=	bmake-do-depend
BUILDTARGETS +=	.WAIT

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -p); mkdir -p ${BUILD_DIR}; MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static obj dependall")
# End:
#
