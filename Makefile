# -*- makefile-bsdmake -*-
#
#	Makefile - common wrapper makefile for BSD Make
#
# Copyright (C) 2025 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# This Makefile (and its associated include files) works with NetBSD Make, and
# Simon Gerraty's (sjg's) latest BMake from http://www.crufty.net/FreeWare/
# (with some caveats), and with FreeBSD make.  For many other systems the BMake
# included in pkgsrc will also work (see https://pkgsrc.org/).  It is as yet
# untested on OpenBSD.
#
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm

# N.B.:  The main rules for this project are in Makefile.main
#
# You can easily use this and the related Makefile sections and associated
# include files to wrap any BSD Makefile and use the result to build a simple
# project outside of the main BSD source tree, e.g. as an add-on package,
# perhaps on a non-BSD machine, using Simon's Bmake and Mk-files.  Simply rename
# the original Makefile to Makefile.main, then copy this file, Makefile.inc,
# Makefile.compiler, and Makefile.end to your project.  If your code is portable
# enough then no "configure" step will be necessary!  Simple system dependencies
# can be managed with an optional Makefile.${.MAKE.OS}, etc.

# BUILD:
#
#	mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build bsdmake obj all
#
# (if you skip the "obj", or just run plain "make", you may need to run it twice
# if the build directory is empty)
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
#
# (where "bsdmake" is the native BSD Make, or is BMake)

# Notes:
#
# MAKEOBJDIRPREFIX may also be anywhere outside the source tree, but it always
# must exist beforehand, except on FreeBSD, or Bmake with MKOBJDIRS=auto.
#
# Note a cross-platform build dir should be something more specific:
#
#	BUILD_DIR=build-$(uname -s)-$(uname -p)
#
# or possibly even
#
#	BUILD_DIR=build-$(uname -s)-$(uname -p)-$(uname -r)
#
# On FreeBSD you can just run the "make", twice, and a the object directory will
# be made and then used (and the first time it will complain about not using it).
#
# MAKEOBJDIR can be used instead, though if the project has subdirectories then
# setting it is far more complex.  Note on older FreeBSD releases the default
# MAKEOBJDIR was /usr/obj (contrary to what the manual suggested), which is
# entirely unhelpful for projects independent of the system source tree!

# On NetBSD the default without MAKEOBJDIRPREFIX or MAKEOBJDIR allows you to get
# away with just "make obj .WAIT dependall" (the .WAIT is necessary in case you
# add a '-j N' option), provided the project has no subdirectories.

# Some comments on default settings, mostly for the purposes of "make help":
#
# Note:  If your platform does have libwrap (and tcpd.h), but they're not in a
# system directory searched by default then you can pass appropriate -I and -L
# flags by setting CPPFLAGS and LDFLAGS in the environment.
#
CPPFLAGS ?=	# Additional preprocessor flags, e.g. -I/usr/local/include (in env!)
LDFLAGS ?=	# Additional linker flags, e.g. -I/usr/local/lib (in env!)
#
# N.B.:  You CANNOT set make variables on the command line if they must be
# adjusted, e.g. ap|pre-pended to, within a Makefile -- they can only be set in
# the environment!
#
# Variables set on the command line are effectively always set last, after all
# Makefiles have been read and processed.  Variables set in the environment are
# set first, before any Makefiles have been read.  Makefiles use "?=" to provide
# defaults for variables that can be customised in environment variables.

# On FreeBSD (at least since 14.0), static-linking requires setting
# NO_SHARED=yes in the environment.  On NetBSD set LDSTATIC=-static.

# Wrap the basic BSD Makefile.main with header and footer files for stand-alone
# builds (and builds on non-BSD systems), and for and development work.
#
.include "${.CURDIR}/Makefile.inc"
#
# This must be the first target seen by make.
#
# Depending on "bmake-test-obj-again" is a workaround for versions of make which
# do not fully support MKOBJDIRS=auto (usually set with MK_AUTO_OBJ).
#
all: .PHONY .MAKE bmake-test-obj-again .WAIT ${BUILDTARGETS}

# Now fetch the main project Makefile
#
.include "${.CURDIR}/Makefile.main"

# This must be after <bsd.prog.mk> or <bsd.lib.mk> (normally included above via
# Makefile.main)
#
.include "${.CURDIR}/Makefile.end"

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -p)-$(uname -r); mkdir -p ${BUILD_DIR}; MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} LDSTATIC=-static " (default-value 'compile-command) " -j 8 all")
# End:
#
