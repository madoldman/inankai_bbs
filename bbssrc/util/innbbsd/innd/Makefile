# Makefile of bbsinnd for Firebird BBS 3.0
#
# $Id: Makefile,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $

DATE = parsedate.o timefmt.o

OBJ  = bbspost.o ${BBS_SRC}/src/record.c

all:	parsedate bbspost fillperl

.c.o:   ;   $(CC) ${CFLAGS} -I${BBS_SRC}/include -c $*.c

parsedate: ${DATE}
	${CC} -o parsedate ${DATE}

bbspost: ${OBJ}
	${CC} ${CFLAGS} -I${BBS_SRC}/include -o bbspost ${OBJ} -lcrypt

clean:
	rm -rf *.o *~ bbspost parsedate
	rm -fr bbslib.pl innbbslib.pl rfc931.pl tcp.pl mailpost bntplink
	rm -fr expirepost in.bntpd inntobbs

fillperl:
	@echo " *** note: expirepost has been replaced by other program in ~bbs/bin  ***"
	@echo " *** called \"expire\", and it's 100% settings and usage compatiables ***"
	rm -fr bbslib.pl innbbslib.pl rfc931.pl tcp.pl mailpost bntplink
	rm -fr in.bntpd inntobbs
	sed -e "s:@PERL@:${PERL}:" bbslib.pl.in > bbslib.pl
	sed -e "s:@PERL@:${PERL}:" innbbslib.pl.in > innbbslib.pl
	sed -e "s:@PERL@:${PERL}:" rfc931.pl.in > rfc931.pl
	sed -e "s:@PERL@:${PERL}:" tcp.pl.in > tcp.pl
	sed -e "s:@PERL@:${PERL}:" mailpost.in > mailpost
	sed -e "s:@PERL@:${PERL}:" bntplink.in > bntplink
#	sed -e "s:@PERL@:${PERL}:" expirepost.in > expirepost
	sed -e "s:@PERL@:${PERL}:" in.bntpd.in > in.bntpd
	sed -e "s:@PERL@:${PERL}:" inntobbs.in > inntobbs
