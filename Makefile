# -*- makefile-bsdmake -*-
#
#	Makefile.BSD - input for pmake
#
# Copyright (C) 2019 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake, and
# Simon Gerraty's Bmake & Mk-files from http://www.crufty.net/FreeWare/.
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm

# BUILD:
#
#	mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build bsdmake
#
# (You will probably have to run this twice.)
#
# INSTALL
#
#	MAKEOBJDIRPREFIX=$(pwd)/build bsdmake DESTDIR=/usr/local install
#
# MAKEOBJDIRPREFIX may also be anywhere outside the source tree, but with some
# mk files, e.g. on NetBSD, it must exist beforehand.
#
# Except on FreeBSD you can also just run "make", twice, and local a object
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

.include "${.CURDIR}/Makefile.internal"
.include "${.CURDIR}/Makefile.end"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build " (default-value 'compile-command))
# End:
#
