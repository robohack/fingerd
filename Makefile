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

# N.B.:  The main rules for this project are in Makefile.internal
#
# You can easily use this and the related Makefile sections to wrap any BSD
# Makefile and use the result to build a simple project outside of the main BSD
# source tree, e.g. as an add-on package, perhaps on a non-BSD machine, using
# Simon's Bmake and Mk-files.  Simply rename the original Makefile to
# Makefile.internal, then copy this file, Makefile.inc, Makefile.compiler, and
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
#
# Notes:
#
# MAKEOBJDIRPREFIX may also be anywhere outside the source tree, but with some
# mk files, e.g. on NetBSD, it must exist beforehand.
#
# Except on FreeBSD you can also just run "make", twice, and a local object
# directory will be made and used.

# Wrap the basic BSD Makefile.internal with header and footer files for
# stand-alone builds (on non-BSD systems), and for and development work.
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
.include "${.CURDIR}/Makefile.internal"	# xxx should rename to Makefile.project?

# This must be after <bsd.prog.mk> normally included in Makefile.internal
#
.include "${.CURDIR}/Makefile.end"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -p); mkdir -p ${BUILD_DIR}; MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static obj dependall")
# End:
#
