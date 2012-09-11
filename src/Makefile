#
# $Id: Makefile,v 1.5 2008-09-21 14:17:34 welle Exp $
#

OBJECTS = flood.o hash.o patcher.o datafile.o jobdata.o
CFLAGS  = -O2 -Wall 
#CFLAGS += -g -DDEBUG

# Uncomment if building on Solaris
#LDFLAGS += -lsocket -lnsl
#CFLAGS += -D_SOLARIS_


DEBBASE=pkg/deb
RPMBASE=pkg/rpm
INSTALLDIR=/usr/local/

PKGVERSION=0.2.2

all: flood

flood: $(OBJECTS)

install: flood strip
	mkdir -p ${INSTALLDIR}/sbin 
	cp flood ${INSTALLDIR}/sbin
	mkdir -p ${INSTALLDIR}/share/doc/flood/examples
	cp Changelog ${INSTALLDIR}/share/doc/flood
	cp doc/README ${INSTALLDIR}/share/doc/flood
	cp doc/DOCUMENTATION ${INSTALLDIR}/share/doc/flood
	cp examples/*.flood ${INSTALLDIR}/share/doc/flood/examples
	mkdir -p ${INSTALLDIR}/share/flood/tpl
	cp tpl/*.tpl ${INSTALLDIR}/share/flood/tpl

tar:
	tar czf flood-${PKGVERSION}.tar.gz\
           flood.[ch] hash.[ch] macros.h patcher.[ch]\
	   datafile.[ch] jobdata.[ch]\
           Makefile tpl/*tpl examples/*flood doc Changelog

strip:
	strip flood

.PHONY: clean

clean:
	rm -f ${OBJECTS} flood
