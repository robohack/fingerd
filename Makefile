CC = /usr/5bin/cc
COPTS = -g 
# uncomment if you need sys/cdefs.h
#CINCS = -I./compat
CFLAGS = $(CINCS) $(COPTS) $(DEFINES)
STRIP=#-s
LOCAL = /local
SHARE = /share
BINDIR = $(LOCAL)/sbin
LIBDIR = $(LOCAL)/lib/finger
MANDIR = $(LOCAL)$(SHARE)/man/man8
MANEXT = 8
EXECDIR = /usr/etc
# SunOS needs this....
INPREFIX = in.
# To replace the vendor installed fingerd with a symlink...
INSTALL-LINK = install-link
#  SunOS needs strerror
STRERROR = strerror.o
# Most other operating systems do not ...
#STRERROR = 
# This is needed on SVR4 machines
#LIBS=-lnsl -lsocket


OBJS = fingerd.o access.o wildcard.o rfc931.o misc.o $(STRERROR)

fingerd: $(OBJS)
	$(CC) $(COPTS) -o fingerd $(OBJS)

fingerd.man: fingerd.8
	sed -e 's,xBINDIRx,$(BINDIR),g' -e 's,xLIBDIRx,$(LIBDIR),g' fingerd.8 > $@-t
	rm -f $@
	mv $@-t $@
	chmod 644 $@

clean:
	rm -f fingerd *.o core a.out *~ 

install: install-prog $(INSTALL-LINK)

install-prog: fingerd fingerd.man $(LIBDIR)
	install -c -m 755 $(STRIP) fingerd $(BINDIR)/$(INPREFIX)fingerd
	install -c -m 644 fingerd.man $(MANDIR)/fingerd.$(MANEXT)
	if [ -n "$(INPREFIX)" ] ; then \
		rm -f $(MANDIR)/$(INPREFIX)fingerd.$(MANEXT) ; \
		echo ".so `basename $(MANDIR)`/fingerd.$(MANEXT)" > $(MANDIR)/$(INPREFIX)fingerd.$(MANEXT) ; \
	fi

$(LIBDIR): Makefile
	-mkdir $(LIBDIR)

install-link: 
	if [ ! -h $(EXECDIR)/$(INPREFIX)fingerd -a ! -f $(EXECDIR)/$(INPREFIX)fingerd-ORIG ] ; then \
		mv $(EXECDIR)/$(INPREFIX)fingerd $(EXECDIR)/$(INPREFIX)fingerd-ORIG ; \
	fi
	rm -f $(EXECDIR)/$(INPREFIX)fingerd
	-ln -s $(BINDIR)/$(INPREFIX)fingerd $(EXECDIR)/$(INPREFIX)fingerd
