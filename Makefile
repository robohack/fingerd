# -*- makefile-bsdmake -*-
#
#	Makefile - input for BSD Make
#
# Copyright (C) 2025 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake, and
# Simon Gerraty's Bmake & Mk-files from http://www.crufty.net/FreeWare/.
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm

# N.B.:  The main rules for this project are in Makefile.main
#
# You can easily use this and the related Makefile sections to wrap any BSD
# Makefile and use the result to build a simple project outside of the main BSD
# source tree, e.g. as an add-on package, perhaps on a non-BSD machine, using
# Simon's Bmake and Mk-files.  Simply rename the original Makefile to
# Makefile.main, then copy this file, Makefile.inc, Makefile.compiler, and
# Makefile.end to your project.  If your code is portable enough then no
# "configure" step will be necessary!

# BUILD:
#
#	mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build bsdmake obj all
#
# INSTALL:
#
#	MAKEOBJDIRPREFIX=$(pwd)/build bsdmake DESTDIR=/usr/local install
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it does not get in the way of pkgsrc either.)
#
# N.B.:  Do not specify DESTDIR for the build phase!
#
# HELP:
#
#	bsdmake help

# Notes:
#
# MAKEOBJDIRPREFIX may also be anywhere outside the source tree, but with some
# mk files, e.g. on NetBSD, it must exist beforehand.
#
# Except on FreeBSD you can also just run "make", twice, and a local object
# directory will be made and used.

# For the purposes of "make help"
#
# Note:  If your platform does have libwrap (and tcpd.h), but they're not in a
# system directory searched by default then you can pass appropriate -I and -L
# flags by setting CPPFLAGS and LDFLAGS in the environment.
#
CPPFLAGS ?=	# Additional preprocessor flags, e.g. -I/usr/local/include (in the environment!)
LDFLAGS ?=	# Additional linker flags, e.g. -I/usr/local/lib (in the environment!)
#
# N.B.:  You CANNOT set make variables on the command line if they must be
# adjusted within a Makefile -- they can only be set in the environment!
# Variables set on the command line are effectively always set last, after all
# Makefiles have been read and processed.

# N.B.:  Normally an override of BINDIR is set in the Makefile.inc in the parent
# directory, e.g. for this package it would be in /usr/src/libexec/Makefile.inc
#
BINDIR =		/libexec	# install location (prefixed with DESTDIR)

# Wrap the basic BSD Makefile.main with header and footer files for stand-alone
# builds (on non-BSD systems), and for and development work.
#
.include "${.CURDIR}/Makefile.inc"
#
# This must be the first target seen by make.
#
# Depending on "bmake-test-obj-again" is a workaround for versions of make which
# do not fully support MKOBJDIRS=auto.
#
all: .PHONY .MAKE bmake-test-obj-again .WAIT ${BUILDTARGETS}

# Now the main project Makefile
#
.include "${.CURDIR}/Makefile.main"	# xxx should rename to Makefile.project?

# This must be after <bsd.prog.mk> (normally included above via Makefile.main)
#
.include "${.CURDIR}/Makefile.end"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -p); mkdir -p ${BUILD_DIR}; MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static obj depend all")
# End:
#
