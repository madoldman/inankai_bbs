/*
 * daemon.c			-- a standalone server of this project
 *	
 * A part of BBS Express Project
 *
 * Copyright (c) 1998, 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
 * of BBS Express Project, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: daemon.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
 */
        
#include <sys/ioctl.h>
#include "bbs.h"

#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/termios.h>

static const char daemon_c[] =
   "$Id: daemon.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $";
/* Edit these lines to fit the configuration of your server */

#define DEF_PORT  2000

#define BBSUID      9999
#define BBSGID      99
#define BUFSIZE     1024

#ifndef BBSHOME
  #define BBSHOME     "/service/bbs"
#endif
#define BBSPASSWDS  BBSHOME"/.PASSWDS"

/* You needn't modify any lines below unless you know what you're doing */

struct fileheader currentmail;
struct userec   currentuser;

char            LowUserid[20];
char            hostname[80];

#define QLEN            5

#define S_CONNECT       1
#define S_LOGIN         2


#define STRN_CPY(d,s,l) { strncpy((d),(s),(l)); (d)[(l)-1] = 0; }
#define ANY_PORT        0
#define RFC931_PORT     113
#define RFC931_TIMEOUT  20
#define POP3_TIMEOUT    1200

static jmp_buf  timebuf;

int             State;
int             msock, sock;	/* master server socket */
static int      reaper();
char            fromhost[STRLEN];
char            inbuf[BUFSIZE];
char            remote_userid[STRLEN];
FILE           *cfp;
char           *msg, *cmd;
int             fd;
struct fileheader *fcache;
int             totalnum, totalbyte, markdel, idletime;
int            *postlen;
int             loginok = 0;

void            log_usies();
int             Quit(), User(), Pass(), Conf(), Stat(), List(), Retr(), Rset();
int             Last(), Dele(), Dump(), Upld(), Mtag(), Grup(), Version(), Mget();

struct commandlist {
	char           *name;
	int             (*fptr) ();
}               cmdlists[] = {
	"retr", Retr,
	"dele", Dele,
	"user", User,
	"pass", Pass,
	"stat", Stat,
	"list", List,
	"dump", Dump,
	"quit", Quit,
	"rset", Rset,
	"last", Last,
	"conf", Conf,
	"upld", Upld,
	"mtag", Mtag,
	"group", Grup,
	"version", Version,
	"mget", Mget,
	NULL, NULL
};

char           *crypt();

int
checkpasswd(passwd, test)
	char           *passwd, *test;
{
	static char     pwbuf[14];
	char           *pw;

	strncpy(pwbuf, test, 14);
	pw = crypt(pwbuf, passwd);
	return (!strcmp(pw, passwd));
}

static int
abort_server()
{
	log_usies("ABORT SERVER");
	close(msock);
	close(sock);
	exit(1);
}

int
dokill()
{
	return kill(0, SIGKILL);
}

static FILE    *
fsocket(domain, type, protocol)
	int             domain;
	int             type;
	int             protocol;
{
	int             s;
	FILE           *fp;

	if ((s = socket(domain, type, protocol)) < 0) {
		return (0);
	} else {
		if ((fp = fdopen(s, "r+")) == 0) {
			close(s);
		}
		return (fp);
	}
}

void
outs(str)
	char           *str;
{
	char            sendbuf[BUFSIZE];

	(void) bzero(sendbuf, sizeof(sendbuf));
	(void) sprintf(sendbuf, "%s\r\n", str);
	(void) write(sock, sendbuf, strlen(sendbuf));
}

/* timeout - handle timeouts */
static void
timeout(sig)
	int             sig;
{
	longjmp(timebuf, sig);
}


void
rfc931(rmt_sin, our_sin, dest)
	struct sockaddr_in *rmt_sin;
	struct sockaddr_in *our_sin;
	char           *dest;
{
	unsigned        rmt_port;
	unsigned        our_port;
	struct sockaddr_in rmt_query_sin;
	struct sockaddr_in our_query_sin;
	char            user[256];
	char            buffer[512];
	char           *cp;
	FILE           *fp;
	char           *result = "unknown";
	struct hostent *hp;

	/*
	 * Use one unbuffered stdio stream for writing to and for reading
	 * from the RFC931 etc. server. This is done because of a bug in the
	 * SunOS 4.1.x stdio library. The bug may live in other stdio
	 * implementations, too. When we use a single, buffered,
	 * bidirectional stdio stream ("r+" or "w+" mode) we read our own
	 * output. Such behaviour would make sense with resources that
	 * support random-access operations, but not with   sockets.
	 */
	if ((fp = fsocket(AF_INET, SOCK_STREAM, 0)) != 0) {
		setbuf(fp, (char *) 0);

		/*
		 * Set up a timer so we won't get stuck while waiting for the
		 * server.
		 */

		if (setjmp(timebuf) == 0) {
			signal(SIGALRM, (void *) timeout);
			alarm(RFC931_TIMEOUT);

			/*
			 * Bind the local and remote ends of the query socket
			 * to the same IP addresses as the connection under
			 * investigation. We go through all this trouble
			 * because the local or remote system might have more
			 * than one network address. The RFC931 etc.  client
			 * sends only port numbers; the server takes the IP
			 * addresses from the query socket.
			 */

			our_query_sin = *our_sin;
			our_query_sin.sin_port = htons(ANY_PORT);
			rmt_query_sin = *rmt_sin;
			rmt_query_sin.sin_port = htons(RFC931_PORT);

			if (bind(fileno(fp), (struct sockaddr *) & our_query_sin,
				 sizeof(our_query_sin)) >= 0 &&
			    connect(fileno(fp), (struct sockaddr *) & rmt_query_sin,
				    sizeof(rmt_query_sin)) >= 0) {

				/*
				 * Send query to server. Neglect the risk
				 * that a 13-byte write would have to be
				 * fragmented by the local system and cause
				 * trouble with buggy System V stdio
				 * libraries.
				 */

				fprintf(fp, "%u,%u\r\n",
					ntohs(rmt_sin->sin_port),
					ntohs(our_sin->sin_port));
				fflush(fp);

				/*
				 * Read response from server. Use
				 * fgets()/sscanf() so we can work around
				 * System V stdio libraries that incorrectly
				 * assume EOF when a read from a socket
				 * returns less than requested.
				 */

				if (fgets(buffer, sizeof(buffer), fp) != 0
				    && ferror(fp) == 0 && feof(fp) == 0
				    && sscanf(buffer, "%u , %u : USERID :%*[^:]:%255s",
					    &rmt_port, &our_port, user) == 3
				    && ntohs(rmt_sin->sin_port) == rmt_port
				  && ntohs(our_sin->sin_port) == our_port) {

					/*
					 * Strip trailing carriage return. It
					 * is part of the protocol, not part
					 * of the data.
					 */

					if (cp = strchr(user, '\r'))
						*cp = 0;
					result = user;
				}
			}
			alarm(0);
		}
		fclose(fp);
	}
	STRN_CPY(dest, result, 60);

	if (strcmp(dest, "unknown") == 0)
		strcpy(dest, "");
	else
		strcat(dest, "@");

	hp = gethostbyaddr((char *) &rmt_sin->sin_addr, sizeof(struct in_addr),
			   rmt_sin->sin_family);
	if (hp)
		strcat(dest, hp->h_name);
	else
		strcat(dest, (char *) inet_ntoa(rmt_sin->sin_addr));

}

int
Isspace(ch)
	char            ch;
{
	return (ch == ' ' || ch == '\t' || ch == 10 || ch == 13);
}

char           *
nextwordlower(str)
	char          **str;
{
	char           *p;

	while (Isspace(**str))
		(*str)++;
	p = (*str);

	while (**str && !Isspace(**str)) {
		**str = tolower(**str);
		(*str)++;
	}

	if (**str) {
		**str = '\0';
		(*str)++;
	}
	return p;
}

char           *
nextword(str)
	char          **str;
{
	char           *p;

	while (Isspace(**str))
		(*str)++;
	p = (*str);

	while (**str && !Isspace(**str))
		(*str)++;

	if (**str) {
		**str = '\0';
		(*str)++;
	}
	return p;
}

void
Init()
{
	State = S_CONNECT;
	LowUserid[0] = '\0';
	markdel = 0;
	idletime = 0;
	(void) gethostname(hostname, 80);
}

void
Login_init()
{
	int             fd, i;
	char           *ptr;
	struct stat     st;

	totalnum = totalbyte = 0;
	sprintf(genbuf, "mail/%c/%s/.DIR", toupper(*LowUserid), LowUserid);
	if (stat(genbuf, &st) == -1 || st.st_size == 0) {
		return;
	}
	totalnum = st.st_size / sizeof(struct fileheader);
	fcache = (struct fileheader *) malloc(st.st_size);
	postlen = (int *) malloc(sizeof(int) * totalnum);
	fd = open(genbuf, O_RDONLY);
	read(fd, fcache, st.st_size);
	close(fd);

	for (i = 0; i < totalnum; i++) {
		if (index(fcache[i].owner, '@') == NULL) {
			if (ptr = strchr(fcache[i].owner, ' '))
				*ptr = '\0';
			sprintf(genbuf, ".bbs@%s", hostname);
			strcat(fcache[i].owner, genbuf);
		}
		sprintf(genbuf, "mail/%c/%s/%s", toupper(*LowUserid), LowUserid, fcache[i].filename);
		if (stat(genbuf, &st) == -1)
			st.st_size = 0;
		postlen[i] = st.st_size + strlen(fcache[i].owner) + 10 + strlen(fcache[i].title)
			+ 10 + 40;
		totalbyte += postlen[i];
		fcache[i].accessed[0] = ' ';
	}
}

void
pop3_timeout()
{
	idletime++;
	if (idletime > 5) {
		log_usies("ABORT - TIMEOUT");
		fclose(cfp);
		close(sock);
		exit(1);
	}
	alarm(POP3_TIMEOUT);
}


main(argc, argv)
	int             argc;
	char          **argv;
{

	struct sockaddr_in fsin, our;
	int             on, alen, len, i, n;
	char           *str;
	int             portnum;
	int             childpid;

	if ( argc == 1 )
		portnum = (int) DEF_PORT;
	else
		portnum = atoi(argv[1]);
	
	if (fork())
		exit(0);

	fprintf(stderr,"\nexpress[%d] at port %d started\n",getpid(),portnum);

	for (n = 0; n < 10; n++)
		close(n);
	open("/dev/null", O_RDONLY);
	dup2(0, 1);
	dup2(0, 2);
	if ((n = open("/dev/tty", O_RDWR)) > 0) {
		ioctl(n, TIOCNOTTY, 0);
		close(n);
	}
	if ((msock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		exit(1);
	}
	setsockopt(msock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	bzero((char *) &fsin, sizeof(fsin));
	fsin.sin_family = AF_INET;
	fsin.sin_addr.s_addr = htonl(INADDR_ANY);
	fsin.sin_port = htons(portnum);

	if (bind(msock, (struct sockaddr *) & fsin, sizeof(fsin)) < 0) {
		exit(1);
	}
	signal(SIGHUP, (void *) abort_server);
	signal(SIGCHLD, (void *) reaper);

	listen(msock, QLEN);

	while (1) {

		alen = sizeof(fsin);
		sock = accept(msock, (struct sockaddr *) & fsin, &alen);
		if ((sock < 0) && (errno == EINTR))
			continue;

		if ((childpid = fork()) < 0) {
			exit(1);
		}
		switch (childpid) {
		case 0:	/* child process */
			close(msock);

			setgid(BBSGID);
			setuid(BBSUID);

			strcpy(fromhost, (char *) inet_ntoa(fsin.sin_addr));
			len = sizeof our;
			getsockname(sock, (struct sockaddr *) & our, &len);

			Init();

			rfc931(&fsin, &our, remote_userid);

			cfp = fdopen(sock, "r+");
			setbuf(cfp, (char *) 0);

			sprintf(genbuf, "200 BlueWave Express server at %s starting, nice to see you.", hostname);
			outs(genbuf);

			chdir(BBSHOME);

			log_usies("CONNECT");
			alarm(0);
			signal(SIGALRM, (void *) pop3_timeout);
			alarm(POP3_TIMEOUT);

			while (fgets(inbuf, sizeof(inbuf), cfp) != 0) {

				idletime = 0;

				msg = inbuf;

				inbuf[strlen(inbuf) - 1] = '\0';
				if (inbuf[strlen(inbuf) - 1] == '\r')
					inbuf[strlen(inbuf) - 1] = '\0';
				cmd = nextwordlower(&msg);

				if (*cmd == 0)
					continue;

				i = 0;
				while ((str = cmdlists[i].name) != NULL) {
					if (strcmp(cmd, str) == 0)
						break;
					i++;
				}

				if (str == NULL) {
					sprintf(genbuf, "500 Unknown command: \"%s\".", cmd);
					outs(genbuf);
				} else
					(*cmdlists[i].fptr) ();

			}

			if (State == S_LOGIN) {
				free(fcache);
				free(postlen);
			}
			log_usies("ABORT");
			fclose(cfp);
			close(sock);
			exit(0);
			break;
		default:	/* parent process */
			close(sock);
			break;
		}
	}
}

static int
reaper()
{
	int             state, pid;
	while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0);
	return 0;
}

int
Conf()
{
	if (State != S_LOGIN) {
		outs("999 Unknown command: \"conf\".");
		return -1;
	}

	b_create();
	return 0;
}

int
Grup()
{
	if (State != S_LOGIN) {
		outs("999 Unknown command: \"group\".");
		return -1;
	}

	return(group());
}

int
Version()
{
	extern char c_express_c[], mmencode_c[], stuff_c[], bigfile_c[];
	
	sprintf(genbuf, "200 %s",daemon_c);
	outs(genbuf);
	sprintf(genbuf, "200 %s",c_express_c);
	outs(genbuf);
	sprintf(genbuf, "200 %s",mmencode_c);
	outs(genbuf);
	sprintf(genbuf, "200 %s",stuff_c);
	outs(genbuf);
	sprintf(genbuf, "200 %s",bigfile_c);
	outs(genbuf);
}

int
Mtag()
{
	if (!loginok) {
		outs("402 USER assign first.");
		return -1;
	}
	
	tagmail();
	return 0;

}

int
get_userdata(user)
	char           *user;
{
	FILE           *rec;
	int             found = 0;

	if ((rec = fopen(BBSPASSWDS, "rb")) == NULL)
		return -1;
	while (1) {
		if (fread(&currentuser, sizeof(currentuser), 1, rec) <= 0)
			break;
		if (currentuser.numlogins <= 0)
			continue;
		/* edwardc.990111 it may case insensetive */
		if (strcasecmp(user, currentuser.userid) == 0 ) {
			found = 1;
			break;
		}
	}
	fclose(rec);
	return found;
}

int
User()
{

	if (State == S_LOGIN) {
		outs("999 Unknown command: \"user\".");
		return -1;
	}
	cmd = nextwordlower(&msg);

	if ( strcasecmp(cmd, "guest") == 0 ) {
		sprintf(genbuf, "501 Unknown user \"%s\".", cmd);
		outs(genbuf);
		return -1;
	}
	
	if (get_userdata(cmd) == 1) {
		strcpy(LowUserid, cmd);
		sprintf(genbuf, "300 Password required for [%s]", currentuser.userid);
		outs(genbuf);
		return 0;
	} else {
		sprintf(genbuf, "501 Unknown user: \"%s\".", cmd);
		outs(genbuf);
		return -1;
	}
}

void
log_usies(buf)
	char           *buf;
{
	FILE           *fp;

	if ((fp = fopen("reclog/express.log", "a")) != NULL) {
		time_t          now;
		struct tm      *p;

		time(&now);
		p = localtime(&now);
		fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d [%s](%s) %s\n",
		p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min,
		    p->tm_sec, currentuser.userid ? currentuser.userid : "",
			remote_userid ? remote_userid : "", buf);
		fflush(fp);
		fclose(fp);
	}
}

int
Retr()
{

	if (State != S_LOGIN) {
		outs("999 Unknown command: \"retr\".");
		return -1;
	}
	compress();
}


int
Stat()
{
	if (State != S_LOGIN) {
		outs("999 Unknown command: \"stat\".");
		return -1;
	}
	cmd = nextword(&msg);
	
	if ( *cmd == 0 ) {
 	   sprintf(genbuf, "200 %d %d", totalnum, totalbyte);
	   outs(genbuf);
	} else if ( strcasecmp(cmd, "blist") == 0 ) {
	   statblist();
	} else {
	   outs("511 unknow parameter for \"stat\"");
	}
	
	return 0;
}


int
Rset()
{
	int             i;

	if (State != S_LOGIN) {
		outs("999 Unknown command: \"rset\".");
		return -1;
	}
	for (i = 0; i < totalnum; i++) {
		fcache[i].accessed[0] = ' ';
	}
	markdel = 0;
	sprintf(genbuf, "204 Maildrop has %d messages (%d octets)", totalnum, totalbyte);
	outs(genbuf);
	return 0;
}

int
List()
{
	int             i;

	if (State != S_LOGIN) {
		outs("999 Unknown command: \"list\".");
		return -1;
	}
	cmd = nextword(&msg);

	if (*cmd == 0) {
		sprintf(genbuf, "205 %d messages (%d octets)", totalnum, totalbyte);
		outs(genbuf);
		for (i = 0; i < totalnum; i++) {
			if (fcache[i].accessed[0] == ' ') {
				sprintf(genbuf, "%d %d", i + 1, postlen[i]);
				outs(genbuf);
			}
		}
		outs(".");
	} else {
		i = atoi(cmd);
		if (i <= 0 || totalnum < i) {
			sprintf(genbuf, "503 Message %d does not exist.", i);
			outs(genbuf);
			return -1;
		} else if (fcache[i - 1].accessed[0] == 'X') {
			sprintf(genbuf, "504 Message %d has been deleted.", i);
			outs(genbuf);
			return -1;
		}
		sprintf(genbuf, "205 %d %d", i, postlen[i - 1]);
		outs(genbuf);
	}
	return 0;
}

int
Dump()
{

	if (State != S_LOGIN) {
		outs("999 Unknown command: \"uidl\".");
		return -1;
	}
	cmd = nextword(&msg);

	outs("250 Data follow up");
	if ( strcasecmp(cmd, "boards") == 0 )
		boardlist();
	else {
		outs("250 Data follow up");
		dumplist();
	}
	
	return 0;

}


int
Pass()
{
	if (State == S_LOGIN) {
		outs("-ERR Unknown command: \"pass\".");
		return -1;
	}
	cmd = nextword(&msg);

	if (*cmd == 0) {
		outs("-ERR Too few arguments for the pass command.");
		return -1;
	}
	if (LowUserid[0] == '\0') {
		outs("-ERR need a USER");
		return -1;
	}
	if (!checkpasswd(currentuser.passwd, cmd)) {
		sprintf(genbuf, "-ERR Password supplied for \"%s.bbs\" is incorrect.", LowUserid);
		outs(genbuf);
		LowUserid[0] = '\0';
		log_usies("ERROR PASSWD");
		return -1;
	}
	if (State == S_CONNECT) {
		log_usies("ENTER");
		State = S_LOGIN;
	}
	Login_init();
	loginok = 1;
	express();
	return 0;
}

int
Last()
{
	if (State != S_LOGIN) {
		outs("-ERR Unknown command: \"last\".");
		return -1;
	}
	 
	cmd = nextword(&msg);
	
	tagnew();
}

int
Upld()
{
	if (State != S_LOGIN) {
		outs("-ERR Unknown command: \"Upld\".");
		return -1;
	}
}

int
Dele()
{
	int             num;

	if (State != S_LOGIN) {
		outs("-ERR Unknown command: \"dele\".");
		return -1;
	}
	cmd = nextword(&msg);

	if (*cmd == 0) {
		outs("-ERR Too few arguments for the dele command.");
		return -1;
	}
	num = atoi(cmd);
	if (num <= 0 || totalnum < num) {
		sprintf(genbuf, "-ERR Message %d does not exist.", num);
		outs(genbuf);
		return -1;
	} else if (fcache[num - 1].accessed[0] == 'X') {
		sprintf(genbuf, "-ERR Message %d has already been deleted.", num);
		outs(genbuf);
		return -1;
	}
	fcache[num-1].accessed[0] = 'X';
	markdel++;
	sprintf(genbuf, "+OK Message %d has been deleted.", num);
	outs(genbuf);
	return 0;
}

int
do_delete()
{
	int             i, fdr, fdw, count;
	char            fpath[80], fnew[80];

	sprintf(fpath, "mail/%c/%s/.DIR", toupper(*LowUserid), LowUserid);
	sprintf(fnew, "mail/%c/%s/pop3.DIR", toupper(*LowUserid), LowUserid);
	if ((fdr = open(fpath, O_RDONLY)) == -1)
		return -1;
	if ((fdw = open(fnew, O_RDWR | O_CREAT, 0644)) == -1)
		return -1;
	i = count = 0;
	while (read(fdr, &currentmail, sizeof(currentmail))) {
		if (i >= totalnum || fcache[i].accessed[0] == ' ') {
			write(fdw, &currentmail, sizeof(currentmail));
			count++;
		} else {
			sprintf(genbuf, "mail/%c/%s/%s", toupper(*LowUserid), LowUserid,
				currentmail.filename);
			unlink(genbuf);
		}
		i++;
	}
	close(fdr);
	close(fdw);
	unlink(fpath);
	if (count) {
		rename(fnew, fpath);
	} else
		unlink(fnew);
	return 0;
}

int
Fatal()
{
	Quit();
}

int
Mget()
{
	maxget();
}

int
Quit()
{
	if (State == S_LOGIN) {
		free(fcache);
		free(postlen);
		if (markdel)
			do_delete();
	}
	log_usies("EXIT");
	sprintf(genbuf, "200 BlueWave Express server at %s signing off, bye bye.", hostname);
	outs(genbuf);
	fclose(cfp);
	close(sock);
	exit(0);
}

void
oouts(int status, char *str, int havereturn)
{
	char            sendbuf[200];

	(void) bzero(sendbuf, sizeof(sendbuf));
	(void) sprintf(sendbuf, "%d %s\r%s", status, str, (havereturn == 0) ? "\n" : "");
	(void) write(sock, sendbuf, strlen(sendbuf));
}
