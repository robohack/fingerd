CC = gcc
COPTS = -g 
CFLAGS = $(COPTS) $(DEFINES)
BINDIR = /usr/local/etc
MANDIR = /usr/local/man/man8
#  SunOS needs strerror
STRERROR = strerror.o
# Most other operating systems do not ...
#STRERROR = 
# This is needed on SVR4 machines
#LIBS=-lnsl -lsocket


OBJS = fingerd.o access.o wildcard.o rfc931.o misc.o $(STRERROR)

fingerd: $(OBJS)
	$(CC) $(COPTS) -o fingerd $(OBJS)
clean:
	rm -f fingerd *.o core a.out *~ 

install: fingerd
	install -c -s fingerd $(BINDIR)/fingerd
	install -c fingerd.8 $(MANDIR)
