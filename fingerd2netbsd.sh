#! /bin/sh
:
#
#	fingerd2netbsd - upgrade from latest fingerd release
#
# NOTE: needs a POSIX /bin/sh to run....
#
#ident	"@(#)$Name:  $:$Id: fingerd2netbsd.sh,v 1.2 1999/01/15 20:34:06 woods Exp $"

PWD=$(/bin/pwd)
version=$(basename $PWD | sed -e 's/fingerd-//')
releasetag=$(basename $PWD | sed -e 's/\./_/g')

cpsed ()
{
	if [ ! -r $1 ] ; then
		echo "$1: no such file!" 1>&2
		exit 1
	if
	# assume that there will only be one #ident line....
	sed -e '/^#ident/i\
#ident	"@(#)$\Name$:$\NetBSD$"\
' \
	    -e '/^\.\\"ident/i\
.\\"ident	"@(#)$\Name$:$\NetBSD$"\
' \
	    -e '/^\.\\"#ident/i\
.\\"#ident	"@(#)$\Name$:$\NetBSD$"\
' $1 > $2
}

IMPORTDIR="import.d"

mkdir ${IMPORTDIR}

if [ ! -r Makefile.BSD -o ! -r fingerd.8 ] ; then
	./configure
fi

# note the rename....
cpsed Makefile.BSD ${IMPORTDIR}/Makefile

cpsed fingerd.8 ${IMPORTDIR}/fingerd.8
cpsed fingerd.c ${IMPORTDIR}/fingerd.c
cpsed fingerd.h ${IMPORTDIR}/fingerd.h
cpsed access.c ${IMPORTDIR}/access.c
cpsed misc.c ${IMPORTDIR}/misc.c
cpsed version.c ${IMPORTDIR}/version.c
cpsed wildcard.c ${IMPORTDIR}/wildcard.c
cpsed fingerd2netbsd.sh ${IMPORTDIR}/fingerd2netbsd.sh

mkdir ${IMPORTDIR}/samples

cpsed samples/access ${IMPORTDIR}/samples/fingerd.acl
cpsed samples/motd ${IMPORTDIR}/samples/fingerd.motd
cpsed samples/users ${IMPORTDIR}/samples/fingerd.users

cd ${IMPORTDIR}

# tell them what to do....
echo "cd ${IMPORTDIR} ; cvs import -m 'Import of Planix fingerd verion $version' src/libexec/fingerd PLANIX $releasetag"

exit 0
