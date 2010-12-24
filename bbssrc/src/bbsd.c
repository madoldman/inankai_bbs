/*
 * bbsd.c		-- bbs daemon for Firebird BBS 3.0
 *
 * A part of Firebird BBS 3.0 Project
 *
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@firebird.dhs.org>
 * All rights reserved.
 *
 * ** use metrials from mbbsd of Maple BBS 3.00 **
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
 * CVS: $Id: bbsd.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
 */
/*-------------------------------------------------------*/
/* mbbsd.c      ( NTHU CS MapleBBS Ver 3.00 )            */
/*-------------------------------------------------------*/
/* target : BBS daemon/main/login/top-menu routines      */
/* create : 95/03/29                                     */
/* update : 96/10/10                                     */
/*-------------------------------------------------------*/

#include "bbs.h"

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/telnet.h>

#ifdef SYSV
#include <sys/termios.h>
#else
#include <termios.h>
#endif

#ifdef LINUX
#include <sys/ioctl.h>
#endif

#ifdef LINUX
#define HAVE_CHKLOAD
#endif

#define SOCKET_QLEN     4

#define TH_LOW          100
#define TH_HIGH         150

#define PID_FILE		BBSHOME"/reclog/bbsd.pid"
#define LOG_FILE		BBSHOME"/reclog/bbsd.log"
#define BAD_HOST		BBSHOME"/.bad_host"
#define NOLOGIN			BBSHOME"/NOLOGIN"

#ifdef  HAVE_CHKLOAD
#define BANNER  "[1;35m"BBSNAME" [1;36m×î½ü [33m(1,5,15)[36m ·ÖÖÓÆ½¾ù¸ººÉÎª[33m %s [36m(ÉÏÏÞ = %d) [%s][0m\r\n"
#else
#define BANNER  "\r\n»¶Ó­¹âÁÙ[1;33m"BBSNAME"[m[ [1;32m"BBSHOST"[m] [1;33m"BBSVERSION"[m [bbsd ready]\r\n"
#endif

jmp_buf byebye;

//char    remoteusername[40] = "?";
extern  char    fromhost[60];
char    genbuf[1024];
#ifdef HAVE_CHKLOAD
char	loadstr[1024];
#endif
char    status[64];

char h_str[2000][32];
int ip[2000], h_total;

void loadhosts()
{
	FILE *fp;
	char tmphost[80], tmp[80], tmpip[80];
	h_total=0;
	fp=fopen("etc/hosts", "r");
        if(fp==0) return ;
	while(1)
	{
		bzero(tmphost, 80);
		if(fscanf(fp, "%s %s %s %s", tmphost, tmp, tmp, tmpip)<0)
			break;
		strncpy(h_str[h_total], tmphost, 28);
		ip[h_total]=inet_addr(tmpip);
		if(h_total++>=1999)
			break;
	}
	fclose(fp);
}

char* gethost(int x)
{
	int i;
	for(i=0; i<2000; i++)
	{
		if(ip[i]==0)
			break;
		if(ip[i]==x)
			return h_str[i];
	}
	return (char*) inet_ntoa(x);
}


/* ----------------------------------------------------- */
/* FSA (finite state automata) for telnet protocol       */
/* ----------------------------------------------------- */


static void telnet_init()
{
	static char svr[] = {
	                        IAC, WILL, TELOPT_ECHO,
	                        IAC, WILL, TELOPT_SGA
	                    };
	struct timeval to;
	int     rset = 1;
	char    buf[256];

	send(0, svr, 6, 0);
	to.tv_sec = 6;
	to.tv_usec = 1;
	if(select(1, (fd_set *) & rset, NULL, NULL, &to)> 0)
		recv(0, buf, sizeof(buf), 0);
}


/* ----------------------------------------------- */
/* È¡µÃ remote user name ÒÔÅÐ¶¨Éí·Ý                */
/* ----------------------------------------------- */

#include <setjmp.h>

static void getremotename(struct sockaddr_in * from, char *rhost)
{
#ifdef DOMAIN_NAME
	setjmp(byebye);
	(void) strcpy(rhost, gethost(inet_addr(inet_ntoa(from->sin_addr))));
#else

	setjmp(byebye);
	(void)strcpy(rhost,(char *)inet_ntoa(from->sin_addr));
#endif
}


/* ----------------------------------- */
/* check system / memory / CPU loading */
/* ----------------------------------- */

#ifdef  HAVE_CHKLOAD
int     fkmem;

int chkload(int limit)
{
	double  cpu_load[3];
	register int i;
#ifdef BSD44

	getloadavg(cpu_load, 3);
#elif defined(LINUX)

	FILE   *fp;
	fp = fopen("/proc/loadavg", "r");

	if (!fp)
		cpu_load[0] = cpu_load[1] = cpu_load[2] = 0;
	else
	{
		float   av[3];
		fscanf(fp, "%g %g %g", av, av + 1, av + 2);
		fclose(fp);
		cpu_load[0] = av[0];
		cpu_load[1] = av[1];
		cpu_load[2] = av[2];
	}
#else

#include <nlist.h>

#ifdef SOLARIS

#define VMUNIX  "/dev/ksyms"
#define KMEM    "/dev/kmem"

	static struct nlist nlst[] =
	    {
		    {"avenrun"
		    },
		    {0}
	    };
#else

#define VMUNIX  "/vmunix"
#define KMEM    "/dev/kmem"

	static struct nlist nlst[] =
	    {
		    {"_avenrun"
		    },
		    {0}
	    };
#endif

	long    avenrun[3];
	static long offset = -1;
	int     kmem;
	if ((kmem = open(KMEM, O_RDONLY)) == -1)
		return (1);

	if (offset < 0)
	{
		(void) nlist(VMUNIX, nlst);
		if (nlst[0].n_type == 0)
			return (1);
		offset = (long) nlst[0].n_value;
	}
	if (lseek(kmem, offset, L_SET) == -1)
	{
		close(kmem);
		return (1);
	}
	if (read(kmem, (char *) avenrun, sizeof(avenrun)) == -1)
	{
		close(kmem);
		return (1);
	}
	close(kmem);
#define loaddouble(la) ((double)(la) / (1 << 8))

	for (i = 0; i < 3; i++)
		cpu_load[i] = loaddouble(avenrun[i]);
#endif

	i = cpu_load[0];
	if (i < limit)
		i = 0;
	if(i)
	{
		strcpy(status,"³¬¸ººÉ£¬ÇëÉÔºóÔÙÀ´");
	}
	else if (cpu_load[0] >= (float) 0 && cpu_load[0] < (float) 10)
	{
		strcpy(status, "¸ººÉÕý³£");
	}
	else if (cpu_load[0] >= 10 && cpu_load[0] < (float) 100)
	{
		strcpy(status, "¸ººÉÆ«¸ß");
	}
	else
		strcpy(status, "¸ººÉ¹ýÖØ");
	sprintf(loadstr,"%.2f %.2f %.2f",cpu_load[0],cpu_load[1],cpu_load[2]);

	return i;
}
#endif


/* ----------------------------------------------------- */
/* stand-alone daemon                                    */
/* ----------------------------------------------------- */


static int mainset;		/* read file descriptor set */
static struct sockaddr_in xsin;

static void reapchild()
{
	int     state, pid;
	while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0)
		;
}

static void start_daemon()
{
	int     n;
	char    buf[80];
	/*
	 * More idiot speed-hacking --- the first time conversion makes the C
	 * library open the files containing the locale definition and time
	 * zone. If this hasn't happened in the parent process, it happens in
	 * the children, once per connection --- and it does add up.
	 */

	time_t  dummy = time(NULL);
	struct tm *dummy_time = localtime(&dummy);
	(void) strftime(buf, 80, "%d/%b/%Y:%H:%M:%S", dummy_time);

	n = getdtablesize();
	if (fork())
		exit(0);
	/* we use setsid instead of open("/dev/tty") for compatibility.  by cucme*/
	if (setsid()==-1)
		exit(0);
	/**/
	if (fork())	/* one more time */
		exit(0);

	sprintf(genbuf, "%d\t%s", getpid(), buf);

	while (n)
		(void) close(--n);
	/*
		n = open("/dev/tty", O_RDWR);
		if (n > 0) {
			(void) ioctl(n, TIOCNOTTY, (char *) 0);
			(void) close(n);
		}
	*/
	for (n = 1; n <= NSIG; n++)
		(void) signal(n, SIG_IGN);
}


static void close_daemon()
{
	exit(0);
}

static void bbsd_log(char *str)
{
	char buf[256];
	time_t mytime;
	struct tm *tm;

	mytime = time(0);
	tm = localtime(&mytime);
	sprintf(buf, "%.2d/%.2d/%.2d %.2d:%.2d:%.2d bbsd[%d]: %s", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday,
	        tm->tm_hour, tm->tm_min, tm->tm_sec, getpid(), str);
	file_append(LOG_FILE, buf);
}

static int bind_port(int port)
{
	int     sock, on;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	on = 1;
	(void) setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

	xsin.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) & xsin, sizeof xsin) < 0)
	{
		sprintf(genbuf, "bbsd bind_port can't bind to %d\n", port);
		bbsd_log(genbuf);
		exit(1);
	}
	if (listen(sock, SOCKET_QLEN) < 0)
	{
		sprintf(genbuf,"bbsd bind_port can't listen to %d\n", port);
		bbsd_log(genbuf);
		exit(1);
	}
	FD_SET(sock, (fd_set *) & mainset);

	sprintf(genbuf,"started on port %d\n", port);
	bbsd_log(genbuf);

	return sock;
}

static int bad_host(char *name)
{
	FILE   *list;
	char    buf[40], *ptr;

	if (list = fopen(BAD_HOST, "r"))
	{
		while (fgets(buf, 40, list))
		{
			ptr = strtok(buf, " \n\t\r");
			if (ptr != NULL && *ptr != '#')
			{
				if(!strcmp(ptr, name))
				{
					fclose(list);
					return 1;
				}
				if(ptr[0] == '#')
					continue;
				if(ptr[0] == '-' && !strcmp(name,&ptr[1]))
				{
					fclose(list);
					return 0;
				}
				if(ptr[strlen(ptr)-1]=='.'&&!strncmp(ptr,name,strlen(ptr)-1))
				{
					fclose(list);
					return 1;
				}
				if(ptr[0]=='.'&&strlen(ptr)<strlen(name)&&
				       !strcmp(ptr,name+strlen(name)-strlen(ptr)))
				{
					fclose(list);
					return 1;
				}
			}
		}
		fclose(list);
	}
	return 0;
}

/* modified by yiyo ²ÉÓÃ¶Ô¿¹ÉÏÕ¾»úµÄ³ÌÐò */
#ifdef AddWaterONLY
//checkaddr(...), add by ylsdd, ¶Ô¿¹ÉÏÕ¾»ú
#define NADDRCHECK 300
struct
{
	struct in_addr addr;
	time_t t;
	float x;
	int n;
}
addrcheck[NADDRCHECK];

int checkaddr(struct in_addr addr, int csock)
{
	int i, j;
	static int fd=-1;
	char str[150];
	time_t timenow, ttemp;

	if(fd<0)
	{
		fd=open("/tmp/attacklog", O_CREAT|O_WRONLY|O_APPEND, 0664);
		if(fd>=0)
			fcntl(fd, F_SETFD, 1);
	}

	time(&timenow);
	for(i=0;i<NADDRCHECK;i++)
	{
		if(addrcheck[i].t==0)
			continue;
		if(timenow-addrcheck[i].t>60*3||timenow<addrcheck[i].t)
		{
			if(addrcheck[i].x>100&&addrcheck[i].n>7)
			{
				sprintf(str, "remove\t%s\t%d\t%s", inet_ntoa(addrcheck[i].addr),
				        addrcheck[i].n, ctime(&timenow));
				write(fd, str, strlen(str));
			}
			addrcheck[i].t=0;
			continue;
		}
		if(memcmp(&addrcheck[i].addr,&addr,sizeof(addr))==0)
		{
			if(addrcheck[i].x<=100||addrcheck[i].n<=7)
			{
				j=0;
				addrcheck[i].x
				=addrcheck[i].x/(((unsigned)(timenow-addrcheck[i].t)+15)/20.)+30;
			}
			else
				j=1;
			addrcheck[i].n++;

			if(addrcheck[i].x>100&&addrcheck[i].n>7)
			{
				if(j==0&&fd>=0)
					if(fork()==0)
					{
						sprintf(str, "add\t%s\t%d\t%s", inet_ntoa(addr), addrcheck[i].n,
						        ctime(&timenow));
						write(fd, str, strlen(str));
#define WARNING "¶Ô²»Æð, Á¬½Ó½«·â±Õ3·ÖÖÓ¡£Çë²»Òª²»¶ÏÁ¬½Ó³å»÷±¾Õ¾\n"

						write(csock, WARNING, strlen(WARNING));
						sleep(5);
						exit(0);
					}
				return -1;
			}
			addrcheck[i].t=timenow;
			return 0;
		}
	}
	//Èç¹ûÔÚaddrcheckÖÐÃ»ÓÐ¸ÃµØÖ·, Ôò¼ÓÈë.
	for(i=0, j=-1, ttemp=timenow+1; i<NADDRCHECK; i++)
	{
		if(addrcheck[i].t<ttemp&&(addrcheck[i].x<=100||addrcheck[i].n<=7))
		{
			ttemp=addrcheck[i].t;
			j=i;
		}
	}
	if(j==-1)
		for(i=0, j=0, ttemp=timenow+1; i<NADDRCHECK; i++)
		{
			if(addrcheck[i].t<ttemp)
			{
				ttemp=addrcheck[i].t;
				j=i;
			}
		}
	if(addrcheck[j].x>100&&addrcheck[j].n>7)
	{
		sprintf(str, "remove\t%s\t%d\t%s", inet_ntoa(addrcheck[i].addr),
		        addrcheck[j].n, ctime(&timenow));
		write(fd, str, strlen(str));
	}
	addrcheck[j].addr=addr;
	addrcheck[j].t=timenow;
	addrcheck[j].x=0;
	addrcheck[j].n=1;
	return 0;
}
#endif

int main(int argc, char* argv[])
{
	extern int errno;

	register int msock, csock;	/* socket for Master and Child */
	register int nfds;	/* number of sockets */
	register int overload;
	register pid_t pid;
	register time_t uptime;
	int     readset;
	int     value;
	struct timeval tv;

	/* --------------------------------------------------- */
	/* setup standalone                                    */
	/* --------------------------------------------------- */

	start_daemon();

	(void) signal(SIGCHLD, reapchild);
	(void) signal(SIGTERM, close_daemon);


	/* --------------------------------------------------- */
	/* port binding                                        */
	/* --------------------------------------------------- */

	xsin.sin_family = AF_INET;
	if (argc > 1)
		csock = atoi(argv[1]) ;
	if (csock <= 0)
		csock = 12345;
	msock = bind_port(csock);
	if (msock < 0)
		exit(1);
	nfds = msock + 1;

	/* --------------------------------------------------- */
	/* Give up root privileges: no way back from here      */
	/* --------------------------------------------------- */

	(void) setgid((gid_t)BBSGID);
	(void) setuid((uid_t)BBSUID);
	(void) chdir(BBSHOME);
	umask((mode_t)022);

	unlink(PID_FILE);
	sprintf(genbuf, "%d", getpid());
	file_append(PID_FILE, genbuf);

	loadhosts();

	/* --------------------------------------------------- */
	/* main loop                                           */
	/* --------------------------------------------------- */

#if 0

	resolve_utmp();
#endif

	tv.tv_sec = 60 * 30;
	tv.tv_usec = 0;

	overload = uptime = 0;

	for (;;)
	{

#ifdef  HAVE_CHKLOAD
		pid = time(0);
		if (pid > uptime)
		{
			overload = chkload(overload ? TH_LOW : TH_HIGH);
			uptime = pid + overload + 45;
			/* ¶ÌÊ±¼äÄÚ²»ÔÙ¼ì²é system load */
		}
#endif

again:

		readset = mainset;

		value = sizeof xsin;
		do
		{
			csock = accept(msock, (struct sockaddr *) & xsin, &value);
		}
		while (csock < 0 && errno == EINTR);

		if (csock < 0)
		{
			goto again;
		}
#ifdef AddWaterONLY
		if(checkaddr(xsin.sin_addr, csock)<0)
		{
			close(csock);
			continue;
		}
#endif

#ifndef NKTEST
#ifdef HAVE_CHKLOAD
		sprintf(genbuf, BANNER, loadstr, TH_LOW, status);
#else

		sprintf(genbuf, BANNER);
#endif

		(void) write(csock, genbuf, strlen(genbuf));
#endif

#ifdef  HAVE_CHKLOAD

		if (overload)
		{
			sleep(3);
			(void) close(csock);
			continue;
		}
#endif

#ifdef NOLOGIN
		{
			FILE   *fp;
			char    buf[256];
#define MYBANNER "\r\nÏµÍ³´¦ÓÚ[1;33mÔÝÍ£µÇÂ½[m×´Ì¬\r\n[1;32m[±¾Õ¾³ÌÐòÎ¬»¤¿ÉÒÔÉ¾³ý \'[36m~bbs/NOLOGIN[32m\' ºó½â³ý¸Ã×´Ì¬][m\r\n\r\n£½£½£½£½£½£½¹ØÓÚÏµÍ³½øÈëÔÝÍ£µÇÂ½×´Ì¬µÄ¡¾¹«¸æ¡¿£½£½£½£½£½£½\r\n"

			if ((fp = fopen(NOLOGIN, "r")) != NULL)
			{
				(void) write(csock, MYBANNER, strlen(MYBANNER));
				while (fgets(buf, 255, fp) != 0)
				{
					strcat(buf, "\r");
					(void) write(csock, buf, strlen(buf));
				}
				fclose(fp);
				sleep(5);
				close(csock);
				continue;
			}
		}
#endif

		pid = fork();

		if (!pid)
		{

#ifdef  HAVE_CHKLOAD
			(void) close(fkmem);
#endif

			while (--nfds >= 0)
				(void) close(nfds);
			(void) dup2(csock, 0);
			(void) close(csock);
			(void) dup2(0, 1);

			getremotename(&xsin, fromhost);

			/* ban µô bad host / bad user */
			if (bad_host(fromhost))
				exit(1);

			bbssetenv("REMOTEHOST", fromhost, 1);
			bbssetenv("REMOTEUSERNAME", fromhost, 1);

			telnet_init();
			nice(3);	/* lower priority .. */
			start_client();
		}
		(void) close(csock);
	}
}
