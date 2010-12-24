#
#	BBS Express Project for SEEDNet generation 1
#
# CVS: $Id: Makefile,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
#
#################################################################

#  BBSSRC  请设定为 bbs 程式码所在目录, 通常是 /home/bbsroot/bbs_src

BBSSRC	 = ../../src			# for SEEDNet bbs


#############################################################
###        You needn't modify the following stuff      ######
#############################################################

COBJS = c_express.o daemon.o stuff.o mmencode.o bigfile.o

CFILE = c_express.c daemon.c stuff.c mmencode.c bigfile.c

.SUFFIXES: .o .c .ln

.c.o:   ;   $(CC) $(CFLAGS) $(DEFINES) -I../../include -c $*.c

#--------------------- Dependency starts here -------------------

all: express

libz:
	@echo "making libz .."
	(cd zlib ; make all ; cd ..)

express: libz $(COBJS)
	$(CC) $(CFLAGS) -o expressd $(COBJS) -Lzlib/ -lcrypt -lgzip

clean: /tmp
	-rm -fr *~ *.o *.BAK *.a expressd
	(cd zlib; make clean; cd ..)

install:
	-cp expressd $(BBSHOME)/bin

cleanall: clean
	-rm -fr expressd

# DO NOT DELETE
