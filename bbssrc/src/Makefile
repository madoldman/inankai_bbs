#############################################################
####   Setup the following site-specific information     ####
#############################################################

# The home directory for user 'bbs'
BBSHOME=$(BBS_HOME)

# The uid/gid info for user 'bbs' and 'bbsadm'
BBSUID=$(BBS_UID)
BBSGRP=$(BBSGID)

OS_DEF   = $(OS_DEF)
CC       = $(CC)
CFLAGS   = $(CFLAGS)
LIBS     = $(LIBS)

INSTALL  = $(INSTALL)

CSIE_DEF = $(CSIE_DEF)

#############################################################
###        You needn't modify the following stuff      ######
#############################################################
PROGNAME = bbsd
CFILE	 = admintool.c announce.c bbs.c bbsd.c bbsgopher.c bcache.c bm.c \
	   boards.c chat.c comm_lists.c delete.c edit.c fileshm.c goodbye.c \
	   help.c io.c list.c mail.c main.c maintain.c modetype.c more.c \
	   namecomplete.c pass.c postheader.c read.c record.c crypt.c\
	   register.c screen.c sendmsg.c stuff.c talk.c term.c sec_hand.c\
	   userinfo.c vote.c xyz.c readabc.c cal.c friendtest.c five.c\
	 addressbook.c killer.c select.c emailauth.c


COBJS	 = admintool.o announce.o bbs.o bbsd.o bbsgopher.o bcache.o bm.o \
	   boards.o chat.o comm_lists.o delete.o edit.o fileshm.o goodbye.o \
	   help.o io.o list.o mail.o main.o maintain.o modetype.o more.o \
	   namecomplete.o pass.o postheader.o read.o record.o crypt.o\
	   register.o screen.o sendmsg.o stuff.o talk.o term.o sec_hand.o\
	   userinfo.o vote.o xyz.o readabc.o cal.o friendtest.o five.o \
	    addressbook.o killer.o select.o emailauth.o

GARBAGE  = a.out core installchatd chatd bbsrf thread *~ *.BAK

SO	 = admintool.so

# The -DINVISIBLE makes bbs users invisible to 'finger' and 'who'.
# Simply remove it if you don't want this feature.

DEFINES	=  $(CSIE_DEF) $(OS_DEF)
EXTRADEF = -DBACKUP_RANGE_DELETE

.SUFFIXES: .o .c

.c.o:	;$(CC) $(CFLAGS) $(DEFINES) $(EXTRADEF) -c $*.c
#.o.so:	;ld -s -G $*.o -o $*.so -L../lib -lBBS

#--------------------- Dependency starts here -------------------
all:  so $(PROGNAME) chatd thread expire bbsnet 

so: admintool.so

admintool.so: admintool.c
	$(CC) $(CFLAGS) $(OS_DEF) $(EXTRADEF) -c admintool.c
	ld -s -L../lib -lBBS -G admintool.o -o admintool.so

installso: admintool.so
	echo "install .so packages .."
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) admintool.so $(BBSHOME)/so


$(PROGNAME): $(COBJS)
	$(CC) -o $(PROGNAME) $(CFLAGS) $(CSIE_DEF) $(COBJS) $(LIBS) $(EXTRADEF) -L../lib -lBBS
	@echo "Program size: `cat $(PROGNAME)|wc -c` bytes"

chatd: station.c
	$(CC) $(CFLAGS) -o chatd $(OS_DEF) station.c $(LIBS) -L../lib -lBBS

thread: thread.c record.c
	$(CC) $(CFLAGS) -o thread $(OS_DEF) record.c thread.c $(LIBS)

install:
	sh Install.sh

installbbs: bbsd
#	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) bbsd $(BBSHOME)/bin/bbsd.new
	-cp bbsd $(BBSHOME)/bin/bbsd.new
	-rm -f $(BBSHOME)/bin/bbsd.old
	-mv $(BBSHOME)/bin/bbsd $(BBSHOME)/bin/bbsd.old
	mv $(BBSHOME)/bin/bbsd.new $(BBSHOME)/bin/bbsd
installbbsnet: bbsnet
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) bbsnet $(BBSHOME)/bin/bbsnet.new
	-rm -f $(BBSHOME)/bin/bbsnet.old
	-mv $(BBSHOME)/bin/bbsnet $(BBSHOME)/bin/bbsnet.old
	mv $(BBSHOME)/bin/bbsnet.new $(BBSHOME)/bin/bbsnet

installthread: thread
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) thread $(BBSHOME)/bin/thread.new
	-rm -f $(BBSHOME)/bin/thread.old
	-mv $(BBSHOME)/bin/thread $(BBSHOME)/bin/thread.old
	mv $(BBSHOME)/bin/thread.new $(BBSHOME)/bin/thread

installchatd: chatd
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) chatd $(BBSHOME)/bin/chatd.new
	-rm -f $(BBSHOME)/bin/chatd.old
	-mv $(BBSHOME)/bin/chatd $(BBSHOME)/bin/chatd.old
	mv $(BBSHOME)/bin/chatd.new $(BBSHOME)/bin/chatd

clean: /tmp
	-rm -fr $(GARBAGE) $(SO) $(COBJS) $(PROGNAME) $(LNFILES) expire
	-rm -fr bbsnet 
	-rm -fr admintool.o $(GAMEOBJS)

cleanall: clean
	-rm -f Install.sh
	-rm -f ../include/config.h
	-rm -f ../include/chat.h
	-rm -f bbs bbsrf chatd thread

tags: /tmp
	ctags $(CFILE)

update: installbbs installchatd installthread installexpire installbbsnet installso

expire: expire.c record.c
	$(CC) $(CFLAGS) $(OS_DEF) -o expire expire.c record.c

bbsnet: bbsnet.c
	$(CC) ${CFLAGS} ${LIBS} bbsnet.c -o bbsnet

modestat:
	$(CC) $(CFLAGS) -o modestat modestat.c

installexpire: expire
	$(INSTALL) -s -m 550 -g $(BBSGRP) -o $(BBSUID) expire $(BBSHOME)/bin/expire.new
	-rm -f $(BBSHOME)/bin/expire.old
	-mv $(BBSHOME)/bin/expire $(BBSHOME)/bin/expire.old
	mv $(BBSHOME)/bin/expire.new $(BBSHOME)/bin/expire

# DO NOT DELETE
