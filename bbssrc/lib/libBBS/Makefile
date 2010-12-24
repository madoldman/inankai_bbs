#
#	libBBS (libbbs.a) version 1.0 for FirebirdBBS Series
#	a part of SEEDNetBBS generation 1
#
#	$Id: Makefile,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
#
#############################################################
###        You needn't modify the following stuff      ######
#############################################################


LIBNAME	 = libBBS.a

GARBAGE  = a.out core out.* *.BAK DEAD*

CFILE	 = string.c html.c mmdecode.c fileio.c

OBJS	 = string.o html.o mmdecode.o fileio.o

.SUFFIXES: .o .c

.c.o:   ;   $(CC) -s $(CFLAGS) -Wunused -c $*.c

#--------------------- Dependency starts here -------------------

all: $(OBJS)
	ar rv ../$(LIBNAME) $(OBJS)
	ranlib ../$(LIBNAME)

tag:
	ctags $(CFILE)

clean: /tmp
	@echo --- Cleaning Garbage ... ---
	-rm -fr $(GARBAGE) *.o *~

cleanall: clean
	@echo --- Cleanning Grabage with $(LIBNAME) ---
	-rm -fr $(LIBNAME) ../$(LIBNAME)

install:
	@echo --- lib: skip ---
