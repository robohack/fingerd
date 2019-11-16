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
# INSTALL
#
#	MAKEOBJDIRPREFIX=$(pwd)/build bsdmake DESTDIR=/usr/local install
#

# Wrap the basic BSD Makefile with header and footer files for stand-alone
# builds (on non-BSD systems), and for and development work.
#
.include "${.CURDIR}/Makefile.inc"
.include "${.CURDIR}/Makefile.internal"
.include "${.CURDIR}/Makefile.end"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build " (default-value 'compile-command))
# End:
#
