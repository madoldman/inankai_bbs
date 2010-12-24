# Generated automatically from Makefile.in by configure.
# Makefile of mail2bbs for Firebird BBS 3.0
#
# $Id: Makefile,v 1.3 2009-09-23 07:04:31 madoldman Exp $

CC = gcc-3.4
BBS_SRC = ../../src
RECORD_OBJ = $(BBS_SRC)/record.c
CFLAGS = -I../../include
INSTALLDIR = /home/bbs/bin
PERL = /usr/bin/perl
DOMAIN = inankai.com

all: mail2bbs fillperl

mail2bbs: mail2bbs.c $(RECORD_OBJ)
	$(CC) $(CFLAGS) -o mail2bbs mail2bbs.c qp_conv.c $(RECORD_OBJ)

install: mail2bbs
	install -g bbs -m 4750 -o bbs mail2bbs $(INSTALLDIR)
	install -g bbs -m  750 -o bbs bbsmail     $(INSTALLDIR)
	install -g bbs -m  750 -o bbs mailgate.pl $(INSTALLDIR)

fillperl:
	rm -fr bbsmail mailgate.pl
	sed -e "s:@PERL@:${PERL}:" bbsmail.in | \
	sed -e "s:@BBSUID@:9999:" | \
	sed -e "s:@BBSGID@:999:" | \
	sed -e "s:@BBSHOME@:/home/bbs:" > bbsmail
	sed -e "s:@PERL@:${PERL}:" mailgate.pl.in | \
	sed -e "s:@BBSDOMAIN@:$(DOMAIN):" > mailgate.pl

clean:
	rm -fr *.o *~ mail2bbs
