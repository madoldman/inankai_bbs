# Generated automatically from Makefile.in by configure.
#  Makefile for local utility tool kit
# 
#  $Id: Makefile,v 1.7 2010-07-05 04:58:45 madoldman Exp $
#

CC       = gcc-3.4
CFLAGS   = -g -export -Wunused -I../include -I../../include
BBSHOME	 = /home/bbs

UTILITY  = account averun bbstop bbspop3d birthday horoscope \
	   newacct poststat usage showuser bbsuptime AnnIndex \
	   badbms showusertime scanbaduser Xhtml birthday2 deadaccount \
	   DigestPackage dumpboardrc info topbless \
	   badbms showusertime scanbaduser perms listbms

.c.o:   ;   $(CC) $(CFLAGS) -c $*.c

all: $(UTILITY) usage boardchat bbspop3d

install: $(UTILITY)
	cp $(UTILITY) $(BBSHOME)/bin
	chown bbs.bbs $(BBSHOME)/bin/*

clean:
	rm -fr *.o *.BAK *.log $(UTILITY) *core* DEAD* *~
boardchat:boardchat.c ../../src/record.c
	$(CC) $(CFLAGS) -o boardchat boardchat.c ../../src/record.c

bbspop3d:bbspop3d.c
	$(CC) $(CFLAGS) -o bbspop3d bbspop3d.c -lcrypt

usage: usage.c ../../src/record.c usage1.c
	$(CC) $(CFLAGS) -o usage usage.c ../../src/record.c 
	$(CC) $(CFLAGS) -L../../lib -o usage1 usage1.c ../../src/record.c -lBBS 
checkpwd:checkpwd.c ../../src/pass.c ../../src/crypt.c
	$(CC) $(CFLAGS) -o checkpwd checkpwd.c ../../src/pass.c ../../src/crypt.c
