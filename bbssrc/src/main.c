/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

/*
$Id: main.c,v 1.7 2009-10-02 06:03:14 thom Exp $
*/
#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

#define BADLOGINFILE    "logins.bad"
#define VISITLOG	BBSHOME"/.visitlog"

int     ERROR_READ_SYSTEM_FILE = NA;
int	fd; //add by yiyo
int     RMSG = YEA;
int     msg_num = 0;
int     count_friends = 0, count_users = 0;
int     iscolor = 1;
int     mailXX = 0;
extern int	forceresolve;
char   *getenv();
int     friend_login_wall();
char   *sysconf_str();
struct user_info *t_search();
void    r_msg();
void    count_msg();
void    c_recover();
void    tlog_recover();
void 	refreshdate();
int     listmode;
int     numofsig = 0;
jmp_buf byebye;
extern struct BCACHE *brdshm;
FILE   *ufp;
int     talkrequest = NA;
/* int ntalkrequest = NA ; */
int     enter_uflags;
time_t  lastnote;

struct user_info uinfo;

#ifndef BBSD
char    tty_name[20];
#endif
char	fromhost[60];

char    BoardName[STRLEN];
//Changed by YB for fix the bug (web and telent should of course use the same lock file)
char    ULIST[] = ".UTMP.lock";
int     utmpent = -1;
time_t  login_start_time;
int     showansi = 1;
int     started = 0;

char	GoodWish[60][STRLEN-1];//modified by bluetent 2003.1.1 (from 20 to 60)
int	WishNum=0;
int     orderWish=0;
int 	temphotinfonumber=0;
extern int enabledbchar;

extern int numboards;

#ifdef ALLOWSWITCHCODE
int	convcode = 0;
extern void resolve_GbBig5Files();
#endif

char   * boardmargin();

void log_usies(char* mode, char* mesg)
{
	time_t  now;
	char    buf[256], *fmt;
	now = time(0);
	fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
	getdatestring(now,NA);
	sprintf(buf, fmt, datestring, mode, currentuser.userid, mesg);
	file_append("usies", buf);
}

void u_enter()
{
	enter_uflags = currentuser.flags[0];
	memset(&uinfo, 0, sizeof(uinfo));
	uinfo.active = YEA;
	uinfo.pid = getpid();
	/* Ê¹·Ç SYSOP È¨ÏŞ ID µÇÂ½Ê±×Ô¶¯»Ö¸´µ½·ÇÒşÉí×´Ì¬ */
#ifdef AddWaterONLY

	if (!HAS_PERM(PERM_SYSOP|PERM_CHATCLOAK))
#else

	if (!HAS_PERM(PERM_CLOAK))
#endif

		currentuser.flags[0] &= ~CLOAK_FLAG;
	if (HAS_PERM(PERM_LOGINCLOAK) && (currentuser.flags[0] & CLOAK_FLAG))
		uinfo.invisible = YEA;
	uinfo.mode = LOGIN;
	uinfo.pager = 0;
#ifdef BBSD

	uinfo.idle_time = time(0);
#endif

	if (DEFINE(DEF_DELDBLCHAR))
		enabledbchar=1;
	else
		enabledbchar=0;
	if (DEFINE(DEF_FRIENDCALL))
	{
		uinfo.pager |= FRIEND_PAGER;
	}
	if (currentuser.flags[0] & PAGER_FLAG)
	{
		uinfo.pager |= ALL_PAGER;
		uinfo.pager |= FRIEND_PAGER;
	}
	if (DEFINE(DEF_FRIENDMSG))
	{
		uinfo.pager |= FRIENDMSG_PAGER;
	}
	if (DEFINE(DEF_ALLMSG))
	{
		uinfo.pager |= ALLMSG_PAGER;
		uinfo.pager |= FRIENDMSG_PAGER;
	}
	uinfo.uid = usernum;
	strncpy(uinfo.from, fromhost, 60);
	/*if (!DEFINE(DEF_NOTHIDEIP))
	{
		uinfo.from[22] = 'H';
	}*/ //loveni
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)
	strncpy(uinfo.tty, tty_name, 20);
#endif
	//   iscolor = (DEFINE(DEF_COLOR)) ? 1 : 0;
	strncpy(uinfo.userid, currentuser.userid, 20);
	strncpy(uinfo.realname, currentuser.realname, 20);
	strncpy(uinfo.username, currentuser.username, NAMELEN);
	getfriendstr();
	getrejectstr();
	if (HAS_PERM(PERM_EXT_IDLE))
		uinfo.ext_idle = YEA;

	listmode = 0;	/* ½èÓÃÒ»ÏÂ, ÓÃÀ´¼ÍÂ¼µ½µ× utmpent ¿¨Î»Ê§°Ü¼¸´Î */
	while ( 1 )
	{
		utmpent = getnewutmpent(&uinfo);
		if ( utmpent >= 0 || utmpent == -1 )
			break;
		if ( utmpent == -2 && listmode <= 100 )
		{
			listmode++;
			usleep(250);		/* ĞİÏ¢ËÄ·ÖÖ®Ò»ÃëÔÙ½ÓÔÚÀø */
			continue;
		}
		if ( listmode > 100 )
		{	/* ·ÅÆú°É */
			sprintf(genbuf, "getnewutmpent(): too much times, give up.");
			report(genbuf);
			prints("getnewutmpent(): Ê§°ÜÌ«¶à´Î, ·ÅÆú. Çë»Ø±¨Õ¾³¤.\n");
			sleep(3);
			bbsd_exit(0);
		}
	}
	if (utmpent < 0)
	{
		sprintf(genbuf, "Fault: No utmpent slot for %s\n", uinfo.userid);
		report(genbuf);
	}
	listmode = 0;
	digestmode = NA;
}

void setflags(int mask, int value)
{
	if (((currentuser.flags[0] & mask) && 1) != value)
	{
		if (value)
			currentuser.flags[0] |= mask;
		else
			currentuser.flags[0] &= ~mask;
	}
}

void u_exit()
{ /*ÕâĞ©ĞÅºÅµÄ´¦ÀíÒª¹Øµô, ·ñÔòÔÚÀëÏßÊ±µÈºò»Ø³µÊ±³öÏÖ  (ylsdd)
	          ĞÅºÅ»áµ¼ÖÂÖØĞ´Ãûµ¥, Õâ¸öµ¼ÖÂµÄÃûµ¥»ìÂÒ±Èkick user¸ü¶à*/
	signal(SIGHUP, SIG_DFL);
	signal(SIGALRM, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);

	setflags(PAGER_FLAG, (uinfo.pager & ALL_PAGER));
	if (HAS_PERM(PERM_LOGINCLOAK))
		setflags(CLOAK_FLAG, uinfo.invisible);

	if (!ERROR_READ_SYSTEM_FILE)
	{
		time_t  stay;
		set_safe_record();
		stay = time(0) - login_start_time;
		currentuser.stay += stay;
        if(!uinfo.invisible)
			currentuser.lastlogout = time(0);

		//±¾Õ¾½¨Õ¾Ê±¼äÎª2008Äê11ÔÂ21ÈÕ21:50:02,ÆäÊ±¼ä´ÁÎª1227275402
		if(currentuser.stay <= MULTI_LOGINS * 2 * (time(0) - 1227275402) && currentuser.userid[0] != '\0')
			substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	}

	uinfo.invisible = YEA;
	uinfo.sockactive = NA;
	uinfo.sockaddr = 0;
	uinfo.destuid = 0;
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

	strcpy(uinfo.tty, "NoTTY");
#endif

	uinfo.pid = 0;
	uinfo.active = NA;
	//   update_utmp();
	if( (fd = open(ULIST, O_RDWR | O_CREAT, 0600)) > 0 )
	{
		flock(fd, LOCK_EX);
	}
	update_utmp();
	if( fd > 0 )
	{
		flock(fd, LOCK_UN);
		close(fd);
	}
	utmpent = -1; // add by yiyo to prevent supercloak
}

int cmpuids(char* uid, struct userec* up)
{
	return !ci_strncmp(uid, up->userid, sizeof(up->userid));
}

int dosearchuser(char* userid)
{
	int     id;
	if ((id = getuser(userid)) != 0)
	{
		if (cmpuids(userid, &lookupuser))
		{
			memcpy(&currentuser, &lookupuser, sizeof(currentuser));
			return usernum = id;
		}
	}
	memset(&currentuser, 0, sizeof(currentuser));
	return usernum = 0;
}


void talk_request()
{
	signal(SIGUSR1, talk_request);
	talkrequest = YEA;
	bell();
	bell();
	bell();
	sleep(1);
	bell();
	bell();
	bell();
	bell();
	bell();
	return;
}

void abort_bbs()
{
	extern int	child_pid;

	if(child_pid)
		kill(child_pid, 9);
	if (  uinfo.mode==POSTING||uinfo.mode==SMAIL||uinfo.mode==EDIT
	        ||uinfo.mode==EDITUFILE||uinfo.mode==EDITSFILE||uinfo.mode==EDITANN)
		keep_fail_post();
	if (started)
	{
		time_t  stay;
		stay = time(0) - login_start_time;
		sprintf(genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);
		log_usies("AXXED", genbuf);
		u_exit();
	}
	bbsd_exit(0);
}

int cmpuids2(int unum, struct user_info* urec)
{
	return (unum == urec->uid);
}

int count_multi(struct user_info* uentp)
{
	static int count;
	if (uentp == NULL)
	{
		int     num = count;
		count = 0;
		return num;
	}
	if (!uentp->active || !uentp->pid)
		return 0;
	if (uentp->uid == usernum)
		count++;
	return 1;
}

int count_user()
{
	count_multi(NULL);
	apply_ulist(count_multi);
	return count_multi(NULL);
}



/* add by bluetent Í¬Ò» IP ÔÊĞíÉÏÕ¾µÄ id ÊıÏŞ¶¨ 2002.01.11 */

void multi_ip_check(char* fromhost,int curr_login_num, char* uid)
{
	struct  user_info *uentp;
	extern  struct UTMPFILE *utmpshm;
	int i;
	//   int count=0;
	int countguest=0;
	for(i=0;i<curr_login_num;i++)
	{
		uentp=&(utmpshm->uinfo[i]);
		if(!uentp->active||!uentp->pid)
			continue;
		if(!strcmp(uentp->from,fromhost)&&!strcmp(uentp->userid,"guest"))
			countguest++;
	}
	if(countguest>=MAXGUESTPERIP&&!strcasecmp(uid,"guest"))
	{
		prints("[1;36m* Í¬ipÏÂguestµÇÂ¼ÈËÊıÒÑ¾­³¬¹ı%d¸ö,´ËÁ¬Ïß½«±»È¡Ïû![m\n", MAXGUESTPERIP);
		oflush();
		sleep(1);
		exit(1);
	}
	/*
	   for(i=0;i<curr_login_num;i++){
	     uentp=&(utmpshm->uinfo[i]);
	     if(!uentp->active||!uentp->pid)continue; 
	     if(!strcmp(uentp->from,fromhost))count++; 
	   } 
	     if(count>=MAXMULTIPLEIP&&(strstr(fromhost,"10.10.1")||strstr(fromhost,"202.113.239")))
	    {
	    prints("[1;36m* Í¬ipµÇÂ¼ÈËÊıÒÑ¾­³¬¹ı%d¸ö,´ËÁ¬Ïß½«±»È¡Ïû![m\n",
	             MAXMULTIPLEIP);
	     oflush();
	     sleep(1);
	     exit(1);
	   } 
	*/
}

/* add end */
void multi_user_check()
{
	struct user_info uin;
	int     logins, mustkick=0;
	int		randnum1,randnum2;
	int ch;
	if (HAS_PERM(PERM_MULTILOG))
		return;		/* don't check sysops */
	if (!strcasecmp("BMcoach", currentuser.userid))
		return;

	
	/* allow multiple guest user */
	logins = count_user();
	if (heavyload() && logins)
	{
		prints("[1;33m±§Ç¸, Ä¿Ç°ÏµÍ³¸ººÉ¹ıÖØ, ÇëÎğÖØ¸´ Login¡£[m\n");
		oflush();
		sleep(3);
		exit(1);
	}
	
	if(time(0) - currentuser.lastlogout < 60){
		srand(time(0));
		randnum1=random() % 5;
		srand(time(0) - 5);
		randnum2=random() % 5;
		prints("\033[1;33mÄúÔÚ 60 ÃëÄÚÖØ¸´ÉÏÕ¾.àÅ£¬ÄúÖªµÀ %d + %d = ? ¶àÉÙÃ´? Çë°´´Ë¼ü\033[0m\n",randnum1,randnum2);
		ch = egetch();
		if((ch - '0') != (randnum1 + randnum2))
			exit(1);
	}

	if (!strcasecmp("guest", currentuser.userid))
	{
		if (logins > MAXGUEST)
		{
			prints("[1;33m±§Ç¸, Ä¿Ç°ÒÑÓĞÌ«¶à [1;36mguest[33m, ÇëÉÔºóÔÙÊÔ¡£[m\n");
			oflush();
			sleep(3);
			exit(1);
		}
		return;
	}
	else if (logins >= MULTI_LOGINS)
	{
		prints("[1;32mÎªÈ·±£ËûÈËÉÏÕ¾È¨Òæ, ±¾Õ¾½öÔÊĞíÄúÓÃ¸ÃÕÊºÅµÇÂ½ %d ¸ö¡£\n[0m",MULTI_LOGINS);
		prints("[1;36mÄúÄ¿Ç°ÒÑ¾­Ê¹ÓÃ¸ÃÕÊºÅµÇÂ½ÁË %d ¸ö£¬Äú±ØĞë¶Ï¿ªÆäËûµÄÁ¬½Ó·½ÄÜ½øÈë±¾Õ¾£¡\n[0m",logins);
		mustkick = 1;
	}
	if (!search_ulist(&uin, cmpuids2, usernum))
		return;		/* user isn't logged in */

	if (!uin.active || (uin.pid && kill(uin.pid, 0)&&uin.mode<20000 == -1))
		return;		/* stale entry in utmp file */

	getdata(0, 0, "[1;37mÄúÏëÉ¾³ıÖØ¸´µÄ login Âğ (Y/N)? [N][m",
	        genbuf, 4, DOECHO, YEA);
	if (genbuf[0] == 'N' || genbuf[0] == 'n'||genbuf[0] == '\0')
	{
		if ( !mustkick )
			return;
		prints("[33mºÜ±§Ç¸£¬ÄúÒÑ¾­ÓÃ¸ÃÕÊºÅµÇÂ½ %d ¸ö£¬ËùÒÔ£¬´ËÁ¬Ïß½«±»È¡Ïû¡£[m\n",logins);
		oflush();
		sleep(3);
		exit(1);
	}
	else
	{
		if(!uin.pid)
			return ;
                if(uin.mode<20000)
    	        	kill(uin.pid, SIGHUP);
                else
                   uin.active=0;

		//ÒÔÇ°²»ÊÇSIGHUP£¬»áµ¼ÖÂ±à¼­×÷Òµ¶ªÊ§ by sunner
		report("kicked (multi-login)");
		log_usies("KICK ", currentuser.username);
	}
}
#ifndef BBSD
void system_init(int argc, char** argv)
#else
void system_init()
#endif
{
#ifndef BBSD
	char   *rhost;
#endif

	struct sigaction act;

	//gethostname(genbuf, 256);
	//sprintf(ULIST, "%s.%s", ULIST_BASE, genbuf);

#ifndef BBSD

	if (argc >= 3)
	{
		strncpy(fromhost, argv[2], 60);
	}
	else
	{
		fromhost[0] = '\0';
	}
	if ((rhost = getenv("REMOTEHOST")) != NULL)
		strncpy(fromhost, rhost, 60);
	fromhost[59] = '\0';
#if defined(SHOW_IDLE_TIME)

	if (argc >= 4)
	{
		strncpy(tty_name, argv[3], 20);
	}
	else
	{
		tty_name[0] = '\0';
	}
#endif
#endif

#ifndef lint
	signal(SIGHUP, abort_bbs);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, abort_bbs);
#ifdef DOTIMEOUT

	init_alarm();
	uinfo.mode = LOGIN;
	alarm(LOGIN_TIMEOUT);
#else

	signal(SIGALRM, SIG_SIG);
#endif

	signal(SIGTERM, SIG_IGN);
	signal(SIGURG, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
#endif

	signal(SIGUSR1, talk_request);

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NODEFER;
	act.sa_handler = r_msg;
	sigaction(SIGUSR2, &act, NULL);
}

void system_abort()
{
	if (started)
	{
		log_usies("ABORT", currentuser.username);
		u_exit();
	}
	clear();
	refresh();
	prints("Ğ»Ğ»¹âÁÙ, ¼ÇµÃ³£À´à¸ !\n");
	bbsd_exit(0);
}
char genbuff[80];
void logattempt(char* uid, char* frm, char* wrongpasswd)
{
	char    fname[STRLEN];
	getdatestring(time(0),NA);
	sprintf(genbuf, "%-12.12s  %-30s %s %s\n",
	        uid, datestring, frm, wrongpasswd);
	sprintf(genbuff, "%-12.12s  %-30s %s\n",
	        uid, datestring, frm);

	file_append(BADLOGINFILE, genbuf);
	sethomefile(fname, uid, BADLOGINFILE);
	file_append(fname, genbuff);

}

int check_tty_lines()
{  /* dii.nju.edu.cn  zhch  2000.4.11 */
	static unsigned char buf1[]= {255, 253, 31};
	unsigned char buf2[100];
	int n;
	if(ttyname(0))
		return;
	write(0, buf1, 3);
	n= read(0, buf2, 80);
	if(n== 12)
	{
		if(buf2[0]!= 255|| buf2[1]!= 251|| buf2[2]!= 31)
			return;
		if(buf2[3]!= 255|| buf2[4]!= 250|| buf2[5]!= 31
		        || buf2[10]!= 255|| buf2[11]!= 240)
			return;
		t_lines= buf2[9];
	}
	if(n== 9)
	{
		if(buf2[0]!= 255|| buf2[1]!= 250|| buf2[2]!= 31
		        || buf2[7]!= 255|| buf2[8]!= 240)
			return;
		t_lines= buf2[6];
	}
	if(t_lines< 24 ||t_lines> 100)
		t_lines=24;
}
struct max_log_record
{
	int     year;
	int     month;
	int     day;
	int     logins;
	unsigned long   visit;
}
max_log, wwwmax_log;
void visitlog(void)
{
	int vfp, online;
	char buf[80];
	time_t now;
	struct tm *tm;
	extern struct UTMPFILE *utmpshm;
	FILE *fp;
	int maxtoday = 0;//½ñÈÕ×î¸ßÈËÊı
	vfp = open(VISITLOG,O_RDWR|O_CREAT,0644);
	if(vfp == -1)
	{
		report("Can NOT write visit Log to .visitlog");
		return ;
	}
	flock(vfp, LOCK_EX);
	lseek(vfp,(off_t)0,SEEK_SET);
	read(vfp, &max_log,(size_t)sizeof(max_log));
	if(max_log.year < 1990 || max_log.year > 2020)
	{
		now = time(0);
		tm = localtime(&now);
		max_log.year = tm->tm_year+1900;
		max_log.month = tm->tm_mon+1;
		max_log.day = tm->tm_mday;
		max_log.visit = 0;
		max_log.logins = 0;
	}
	max_log.visit ++ ;
	if( max_log.logins > utmpshm->max_login_num )
		utmpshm->max_login_num = max_log.logins;
	else
		max_log.logins =  utmpshm->max_login_num;
	lseek(vfp,(off_t)0,SEEK_SET);
	write(vfp,&max_log,(size_t)sizeof(max_log));
	lseek(vfp,(off_t)sizeof(max_log),SEEK_SET);//bluetent
	read(vfp,&wwwmax_log,(size_t)sizeof(max_log));//bluetent
	flock(vfp, LOCK_UN);
	close(vfp);

	sprintf(buf, "%s/maxtoday", BBSHOME);
	fp = fopen(buf, "r+");
	if(fp == NULL) {
		/* Maybe this is the first time you run bbsd,
		 * so I should create ~/maxtoday for you */
		fp = fopen(buf, "w");

		if(fp != NULL) {
			/* I think there is no need to use flock...
			 * Because there should be just one bbsd now */
			fprintf(fp, "%d\n", 0);
			fclose(fp);
		}
		
	} else {
		
		online=num_active_users();
		flock(fileno(fp), LOCK_EX);
		fscanf(fp, "%d", &maxtoday);
		
		if(online>maxtoday) {
			maxtoday=online;
		}
		
		fseek(fp, 0, SEEK_SET);
		fprintf(fp, "%d\n", maxtoday);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
	}


    #ifdef NKTEST
		sprintf(genbuf," ×î¸ßÔÚÏß±¾ÈÕ/ÀúÊ·:[32m%d[36m/[32m%d[36m   ×Ü·ÃÎÊÁ¿: [32m%u[36m[m\n", maxtoday,max_log.logins,max_log.visit+wwwmax_log.visit);
    #else	  
         sprintf(genbuf,"[1;32m×î¸ßÔÚÏß[±¾ÈÕ/Æù½ñ]:[[36m%d[32m/[36m%d[32m]·ÃÎÊÁ¿:[[36m%u[32m][m\n", maxtoday,max_log.logins,max_log.visit+wwwmax_log.visit);
	#endif

	prints(genbuf);
#ifdef AddWaterONLY

	{ FILE *fp;
		fp = fopen("/home/httpd/html/counter/BBS","w+");
		if(fp)
		{
			fprintf(fp,"%d",max_log.visit);
			fclose(fp);
		}
	}
#endif
}

struct stat *f_stat(char *file) {
	static struct stat buf;
	bzero(&buf, sizeof(buf));
	if(stat(file, &buf)==-1) bzero(&buf, sizeof(buf));
	return &buf;
}

#define file_size(x) f_stat(x)->st_size
#define file_time(x) f_stat(x)->st_mtime
#define file_rtime(x) f_stat(x)->st_atime
#define file_exist(x) (file_time(x)!=0)
#define file_isdir(x) ((f_stat(x)->st_mode & S_IFDIR)!=0)
#define file_isfile(x) ((f_stat(x)->st_mode & S_IFREG)!=0)
extern int cmpfilename();
extern char    currfile[STRLEN];
int WebAuthMailExist(char *userid)
{
        int i,total;
        struct fileheader  fileh;
        char    filename[STRLEN];
        char    authfile[STRLEN]={0};
        FILE *fp;
        sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
 
        if(!file_exist(filename)) {
                return 0;
        }
        fp=fopen(filename, "r");
        if(fp==0)
        {
              prints("\n,ÎÄ¼ş´ò¿ªÊ§°Ü:0");
              return 0;
        }
        fread(authfile,STRLEN,1,fp);
        fclose(fp);
        if(strlen(authfile)==0)
        {
              prints("\n,ÎÄ¼ş´ò¿ªÊ§°Ü:1");
           	return 0;
        }
        sprintf(filename, "mail/T/tany/.DIR");
        total=file_size(filename)/sizeof(struct fileheader);
        if(!file_exist(filename)) {
                char buf[80];
                sprintf(buf, "mail/%c/%s", toupper(currentuser.userid[0]), currentuser.userid);
                if(!file_exist(buf))
                                mkdir(buf, 0770);
                fp=fopen(filename, "a+");
                fclose(fp);
                return 0;
        }
        fp=fopen(filename, "r");
        if(fp==0)
        {
                prints("\n,ÎÄ¼ş´ò¿ªÊ§°Ü:2");
                return 0;
        }
 
        for(i=0;i<total;i++)
        {
                fread((void*)&fileh, sizeof(struct fileheader), 1, fp);
	//	prints("test:%s,%s",fileh.title,authfile);
	//	pressanykey();
                if(strstr(fileh.title,authfile)-fileh.title<6)
                {//find it...
                        fclose(fp);
			strcpy(currfile,fileh.filename);
	//		prints("fount!!");
	//		pressanykey();
			if (delete_file(filename, sizeof(struct fileheader), i, cmpfilename)==0)
			{
	//			prints("test2");
	  //                      pressanykey();

           //             del_record(f	ilename, sizeof(struct fileheader), i);
                        sprintf(filename, "mail/T/tany/.DELW");
                       append_record(filename,&fileh,sizeof(struct fileheader));
	//	                prints("test3");
            //                    pressanykey();
 
                        sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
                        unlink(filename);
sprintf(filename, "home/%c/%s/.webauth.done", toupper(userid[0]), userid);
			fp=fopen(filename,"a");
			if(fp)
			{
				fprintf(fp, "Time:%d", time(0));
				fclose(fp);
			}

                        return 1;
			}
			else
				return 0;
                }
        }
        fclose(fp);
        //http_fatal("user Not %s:",authfile,filename);
        return 0;
}
 


void login_query()
{
	FILE *fp;//bluetent
	char    uid[IDLEN + 2];
	char 	passbuf[PASSLEN];
	int     curr_login_num,num_wwwguests;
	int     attempts;
	int		uptime;//bluetent
	char    genbuf[STRLEN];
	char   *ptr;
	extern struct UTMPFILE *utmpshm;

	curr_login_num = num_active_users();
	num_wwwguests  = num_wwwguest();
	if (curr_login_num >= MAXACTIVE)
	{
		ansimore("etc/loginfull", NA);
		oflush();
		sleep(1);
		exit(1);
	}
#ifdef BBSNAME
	strcpy(BoardName, BBSNAME);
#else

	ptr = sysconf_str("BBSNAME");
	if (ptr == NULL)
		ptr = "ÉĞÎ´ÃüÃû²âÊÔÕ¾";
	strcpy(BoardName, ptr);
#endif

	fill_shmfile(5, "etc/sysmsg", "ENDLINE1_SHMKEY");//bluetent

//	if (fill_shmfile(1, "etc/issue", "ISSUE_SHMKEY"))
//	{
//		show_issue();	/* is anibanner ready, remark this and put * \n\n */
//	}
	if((fp=fopen("etc/issue", "r"))!=NULL)
	{
		while(fgets(genbuf, sizeof(genbuf), fp)!=NULL)
		{
			prints(genbuf);
		}
		fclose(fp);
	}
	

	#ifdef NKTEST
			prints("[36m»¶Ó­¹âÁÙ[37m¡ô%s¡ô[36mBBS.", BoardName);
	#else
			prints("[1;35m»¶Ó­¹âÁÙ[1;40;33m¡¾ %s ¡¿", BoardName);
	#endif

	if((fp=fopen("/proc/uptime", "r"))!=NULL)
	{
		fgets(genbuf, STRLEN, fp);
		uptime=0;
		uptime=atoi(genbuf);
        
		#ifdef NKTEST
			prints("ÏµÍ³Á¬ĞøÔËĞĞ: [32m%d[36mÌì[32m%2d[36mĞ¡Ê±[32m%2d[36m·ÖÖÓ,", uptime/86400, (uptime%86400)/3600, (uptime%86400%3600)/60);
		#else
			prints("[1;32mÏµÍ³Á¬ĞøÔËĞĞ:[[36m%d[32mÌì[36m%2d[32mĞ¡Ê±[36m%2d[32m·ÖÖÓ][m", uptime/86400, (uptime%86400)/3600, (uptime%86400%3600)/60);
        #endif
		fclose(fp);
	}
	if (utmpshm->usersum == 0)
		utmpshm->usersum = allusers();
	if (utmpshm->max_login_num < curr_login_num)
		utmpshm->max_login_num = curr_login_num;

	#ifdef NKTEST
		prints("ÕÊºÅ×ÜÊı: [1;32m%d[36m/[1;32m%d[36m\n",utmpshm->usersum, MAXUSERS);
    	prints("µ±Ç°ÔÚÏß: [1;32m%d[36m/[1;32m%d[36m,web:\033[32m%d\033[36m", curr_login_num, MAXACTIVE,num_wwwguests);
	#else
	prints(" [1;32mÕÊºÅÊı:[[1;36m%d[32m/[1;36m%d[32m]\n",utmpshm->usersum, MAXUSERS);

	prints("[1;32mÄ¿Ç°ÔÚÏß:[[1;36m%d[1;32m/[1;36m%d[1;32m](WWW:\033[36m%d\033[32m)", 
    curr_login_num, MAXACTIVE,num_wwwguests);
	#endif

	visitlog();
#ifdef MUDCHECK_BEFORELOGIN

	prints("[1;33mÎª·ÀÖ¹Ê¹ÓÃ³ÌÊ½ÉÏÕ¾£¬Çë°´ [1;36mCTRL + C[m : ");
	genbuf[0] = igetkey();
	if ( genbuf[0] != Ctrl('C') )
	{
		prints("\n¶Ô²»Æğ£¬Äã²¢Ã»ÓĞ°´ÏÂ CTRL+C ¼ü£¡\n");
		oflush();
		exit(1);
	}
	else
	{
		prints("[CTRL] + [C]\n");
	}
#endif
     
    #ifdef NKTEST
    prints("ÊÔÓÃÇëÊäÈë guest, ×¢²áĞÂÕÊºÅÇëÊäÈë new.\n");
	#endif

	attempts = 0;
	while (1)
	{
		if (attempts++ >= LOGINATTEMPTS)
		{
			ansimore("etc/goodbye", NA);
			oflush();
			pressanykey();//bluetent
			exit(1);
		}
		#ifdef NKTEST
			getdata(0, 0, "´óÃû: ", uid, IDLEN + 1, DOECHO, YEA);
		#else
     		getdata(0, 0, "[1;33mÇëÊäÈëÕÊºÅ[m(ÊÔÓÃÇëÊäÈë `[1;36mguest[m', ×¢²áÇëÊäÈë`[1;31mnew[m'): ", uid, IDLEN + 1, DOECHO, YEA);
		#endif

		forceresolve=1;
		resolve_ucache();
		forceresolve=0;
		multi_ip_check(fromhost,curr_login_num,uid); //bluetent
#ifdef ALLOWSWITCHCODE

		/*ptr = strchr(uid, '.');
		if(ptr)
		{
			convcode = 1;
			*ptr = '\0';
		}*/
#endif
		if ((strcasecmp(uid,"guest")==0) && (MAXACTIVE - curr_login_num<10))
		{
			ansimore("etc/loginfull", NA);
			oflush();
			sleep(1);
			exit(1);
		}
		if (strcasecmp(uid, "new") == 0)
		{
#ifdef LOGINASNEW
			memset(&currentuser, 0, sizeof(currentuser));
			new_register();
			ansimore3("etc/firstlogin", YEA);
			break;
#else

			prints("[1;37m±¾ÏµÍ³Ä¿Ç°ÎŞ·¨ÒÔ [36mnew[37m ×¢²á, ÇëÓÃ[36m guest[37m ½øÈë...[m\n");
#endif

		}
		else if (*uid == '\0')
			;
		else if( !dosearchuser(uid))
		{
			prints("[1;31m¾­²éÖ¤£¬ÎŞ´Ë ID (User ID Error)...[m\n");
		}
		else if (strcasecmp(uid, "guest") == 0)
		{
			currentuser.userlevel = 0;
			break;
#ifdef SYSOPLOGINPROTECT

		}
		else if (!strcasecmp(uid,"SYSOP")&&strcmp(fromhost,"localhost")
		         &&strcmp(fromhost,"127.0.0.1"))
		{
			prints("[1;32m ¾¯¸æ: ´Ó %s µÇÂ¼ÊÇ·Ç·¨µÄ! ÇëÎğÔÙÊÔ![m\n", fromhost);
			prints("[×¢Òâ] Îª°²È«Æğ¼û£¬±¾Õ¾ÒÑ¾­Éè¶¨ SYSOP Ö»ÄÜ´ÓÖ÷»úµÇÂ½¡£\n       Èç¹ûÄúÈ·ÊµÊÇ±¾Õ¾µÄ SYSOP £¬ÇëµÇÂ½µ½±¾ BBS ·şÎñÆ÷£¬È»ºó: \n              telnet localhost port.\n");
			oflush();
			sleep( 1 );
			exit ( 1 );
#endif

		}
		else
		{
#ifdef ALLOWSWITCHCODE
		//	if (!convcode)
		//		convcode=!(currentuser.userdefine&DEF_USEGB);
#endif

			sprintf(genbuf, "home/%c/%s/badhost", toupper(currentuser.userid[0]), currentuser.userid);
			if(bad_host(fromhost,genbuf))
			{
				printf("¶Ô²»Æğ£¬´ËÕÊºÅÒÑ±»Éè¶¨Îª²»¿É´Ó´ËIPµÇÂ¼±¾Õ¾");
				oflush();
				sleep( 1 );
				exit ( 1 );
			}
			/*
					sprintf (genbuf, "home/%c/%s/onlyhost", toupper(currentuser.userid[0]), currentuser.userid);
					if (bad_host (fromhost, genbuf)) {
						printf ("¶Ô²»Æğ£¬´ËÕÊºÅÒÑ±»Éè¶¨Îª²»¿É´Ó´ËIPµÇÂ¼±¾Õ¾");
						oflush ();
						sleep (1);
						exit (1);
					}
			*/

sprintf(genbuf, "home/%c/%s/.webauth.U", toupper(currentuser.userid[0]), currentuser.userid);
if(file_exist(genbuf))
{
        //check whether exist user auth.file
       /* if(WebAuthMailExist(currentuser.userid))
        {
              prints("¹§Ï²Äú£¬ÄãµÄÕÊºÅÒÑ±»¼¤»î:)");
              pressanykey();
	      prints("\n");
        }
        else*/
        {                                
        prints("\n¶Ô²»Æğ£¬ÄúµÄ×¢²áĞÅÏ¢Ã»ÓĞµÃµ½emailÑéÖ¤¡£Çë»Ø¸´ºóÓÃWEBµÇÂ½×Ô¶¯¼¤»î");
              pressanykey();
              exit(-1);
         }
}

            #ifdef NKTEST
				getdata(0,0,"[1;37mÃÜÂë: [m",passbuf,PASSLEN,NOECHO,YEA);
			#else
				getdata(0,0,"[1;37mÇëÊäÈëÃÜÂë: [m",passbuf,PASSLEN,NOECHO,YEA);
			#endif

			passbuf[8] = '\0';
			if (!checkpasswd(currentuser.passwd, passbuf))
			{
				if (strlen(passbuf))
				{
					logattempt(currentuser.userid, fromhost, passbuf);
					prints("[1;31mÃÜÂëÊäÈë´íÎó (Password Error)...[m\n");
				}
			}
			else
			{
				if( HAS_PERM(PERM_RECLUSION)&&!HAS_PERM(PERM_SYSOP))
				{
					//modified by bluetent 2002.12.05
					FILE *fp;
					char reclusionbuf[256];
					char reclusiondaychar[8];
					int reclusionday=30;
					sprintf(reclusionbuf, "home/%c/%s/Reclusion", toupper(currentuser.userid[0]), currentuser.userid);
					fp=fopen(reclusionbuf,"r");
					if(fp)
					{
						fscanf(fp,"%s",reclusiondaychar);
						fclose(fp);
					}
					else
						strcpy(reclusiondaychar,"30");
					reclusionday=atoi(reclusiondaychar);
					//modified end
					if((time(0)-currentuser.lastlogin)/(24*60*60)<reclusionday)
					{
						prints("\033[1;32m±¾ÕÊºÅÒÑ¾­¹éÒş,µ«ÊÇÃ»µ½×Ê¸ñÖØ³ö½­ºş,ÇëÓÚ¹éÒş%dÌìºó½â³ı¹éÒş×´Ì¬\033[m\n", reclusionday);
						oflush();
						sleep(1);
						exit(1);
					}
					if(askyn("È·¶¨Òª½â³ı¹éÒş×´Ì¬,ÖØ³ö½­ºşÂğ",NA,NA)==1)
					{
						currentuser.userlevel &= ~PERM_RECLUSION;
						substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
						Postfile("etc/ccjh", "notepad", "ÖØ³ö½­ºş--ÎÒÓÖ»ØÀ´ÁË!!", 2);
					}
					else
					{
						oflush();
						sleep(1);
						exit(1);
					}
				}
				if(HAS_PERM(PERM_SUICIDE)&&!HAS_PERM(PERM_SYSOP))
				{
					 if(askyn("±¾ÕÊºÅÒÑ¾­×ÔÉ±,Òª»Ö¸´Âğ?",YEA,YEA)==YEA)
					 {
						 currentuser.userlevel &= ~PERM_SUICIDE;
					 }
					 else
					 {
						 oflush();
						 sleep(1);
						 exit(1);
					 }
						 
				}
	char msgbuf[STRLEN];
	char repbuf[STRLEN];
	char secu[STRLEN];
	char fname[STRLEN];
	char days[10];
	int day;
	if( !HAS_PERM( PERM_BASIC ) && !HAS_PERM(PERM_SYSOP))
	{
		sprintf(fname, "home/%c/%s/permbasic", toupper(currentuser.userid[0]), currentuser.userid);
		
		FILE *fp;
		if ((fp = fopen(fname, "r")) != NULL)
		{
			fgets(days,9,fp);
			day=atoi(days);
			if(day>0 && (time(NULL)-file_time(fname) >day * 24 *60 * 60) ) 
				{
	 	 			prints( "[1;32m·â½ûÊ±¼äµ½£¬»Ö¸´ÄúµÄ»ù±¾ÉÏÕ¾È¨Á¦¡£Ğ»Ğ»Ö§³ÖÎÒÃÇµÄ¹¤×÷[m\n" );
					pressanykey();
					currentuser.userlevel |= PERM_BASIC;
					substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
					
					sprintf(repbuf,"%s ±»»Ö¸´»ù±¾ÉÏÕ¾È¨Á¦",currentuser.userid);
					sprintf(msgbuf,
		        			"\n  %s ÍøÓÑ: \n\n"
		        			"\tÄúÒÑ¾­±»»Ö¸´»ù±¾ÉÏÕ¾È¨Á¦, \n\n"
		        			"\t ¸½ÑÔ:·â½ûÆÚÂú \n\n"
		        			"\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
		        			"\t\t\t\t\t\t\t     \n\n",
		       				 currentuser.userid);
					autoreport(repbuf,msgbuf,0,currentuser.userid);
				        sprintf(secu, "»Ö¸´%sµÄ»ù±¾ÉÏÕ¾È¨Á¦", currentuser.userid);
                      			security_report(secu,1);
					fclose(fp);
					unlink(fname);
				} 
				else
				{
					char res[STRLEN];
					memset(res,0,sizeof(res));				
					fgets(res,STRLEN,fp);
					if(res[0]==0) prints( "[1;32m±¾ÕÊºÅÒÑÍ£ÓÃ¡£ÇëÏò [36mSYSOP[32m ²éÑ¯Ô­Òò[m\n" );
					else
					{
						sprintf(genbuf,"[1;32m±¾ÕÊºÅÒÑÍ£ÓÃ¡£Ô­Òò [36m%s[32m[m\n",res);
						prints(genbuf);
					}
					oflush();
					sleep( 1 );
					exit( 1 );
				}
						
		}//loveni
		else
			{
				prints( "[1;32m±¾ÕÊºÅÒÑÍ£ÓÃ¡£ÇëÏò [36mSYSOP[32m ²éÑ¯Ô­Òò[m\n" );
				oflush();
				sleep( 1 );
				exit( 1 );
			}
	}//loveni

	if( !HAS_PERM( PERM_POST ) && !HAS_PERM(PERM_SYSOP))
	{
		sprintf(fname, "home/%c/%s/permpost", toupper(currentuser.userid[0]), currentuser.userid);
		FILE *fp;
		if ((fp = fopen(fname, "r")) != NULL)
		{
			fgets(days,9,fp);
			day=atoi(days);
			if(day>0)
			if(time(NULL)-file_time(fname) >day * 24 *60 * 60 ) 
			{
				prints( "[1;32m·â½ûÊ±¼äµ½£¬»Ö¸´ÄúµÄ·¢±íÎÄÕÂÈ¨Á¦¡£Ğ»Ğ»Ö§³ÖÎÒÃÇµÄ¹¤×÷[m\n" );
				pressanykey();
				currentuser.userlevel |= PERM_POST;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				
				sprintf(repbuf,"%s ±»»Ö¸´·¢±íÎÄÕÂÈ¨Á¦",currentuser.userid);
				sprintf(msgbuf,
		        		"\n  %s ÍøÓÑ: \n\n"
		        		"\tÄúÒÑ¾­±»»Ö¸´·¢±íÎÄÕÂÈ¨Á¦, \n\n"
		        		"\t ¸½ÑÔ:·â½ûÆÚÂú \n\n"
		        		"\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
		        		"\t\t\t\t\t\t\t     \n\n",
		       			 currentuser.userid);
				autoreport(repbuf,msgbuf,0,currentuser.userid);
				sprintf(secu, "»Ö¸´%sµÄ·¢±íÎÄÕÂÈ¨Á¦", currentuser.userid);
                      		security_report(secu,1);
				fclose(fp);
				unlink(fname);
				
			} else fclose(fp);
		}
	}//loveni
	if( !HAS_PERM( PERM_CHAT ) && !HAS_PERM(PERM_SYSOP))
	{
		sprintf(fname, "home/%c/%s/permchat", toupper(currentuser.userid[0]), currentuser.userid);
		FILE *fp;
		if ((fp = fopen(fname, "r")) != NULL)
		{
			fgets(days,9,fp);
			day=atoi(days);
			if(day>0)
			if(time(NULL)-file_time(fname) >day * 24 *60 * 60 ) 
			{
				prints( "[1;32m·â½ûÊ±¼äµ½£¬»Ö¸´ÄúµÄ½øÈëÁÄÌìÊÒÈ¨Á¦¡£Ğ»Ğ»Ö§³ÖÎÒÃÇµÄ¹¤×÷[m\n" );
				pressanykey();
				currentuser.userlevel |= PERM_CHAT;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				
				sprintf(repbuf,"%s ±»»Ö¸´½øÈëÁÄÌìÊÒÈ¨Á¦",currentuser.userid);
				sprintf(msgbuf,
			        "\n  %s ÍøÓÑ: \n\n"
			        "\tÄúÒÑ¾­±»»Ö¸´½øÈëÁÄÌìÊÒÈ¨Á¦, \n\n"
			        "\t ¸½ÑÔ:·â½ûÆÚÂú \n\n"
			        "\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
			        "\t\t\t\t\t\t\t     \n\n",currentuser.userid);
				autoreport(repbuf,msgbuf,0,currentuser.userid);
                      		sprintf(secu, "»Ö¸´%sµÄ½øÈëÁÄÌìÊÒÈ¨Á¦", currentuser.userid);
                      		security_report(secu,1);
				fclose(fp);
				unlink(fname);
				
			} else fclose(fp);
		}//loveni
	}
	if( !HAS_PERM( PERM_PAGE ) && !HAS_PERM(PERM_SYSOP))
	{

		sprintf(fname, "home/%c/%s/permpage", toupper(currentuser.userid[0]), currentuser.userid);
		FILE *fp;
		if ((fp = fopen(fname, "r")) != NULL)
		{
			fgets(days,9,fp);
			day=atoi(days);
			if(day>0)
			if(time(NULL)-file_time(fname) >day * 24 *60 * 60 ) 
			{
				prints( "[1;32m·â½ûÊ±¼äµ½£¬»Ö¸´ÄúµÄºô½ĞËûÈËÁÄÌìÈ¨Á¦¡£Ğ»Ğ»Ö§³ÖÎÒÃÇµÄ¹¤×÷[m\n" );
				pressanykey();
				currentuser.userlevel |= PERM_PAGE;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				
				sprintf(repbuf,"%s ±»»Ö¸´ºô½ĞËûÈËÁÄÌìÈ¨Á¦",currentuser.userid);
				sprintf(msgbuf,
			        "\n  %s ÍøÓÑ: \n\n"
			        "\tÄúÒÑ¾­±»»Ö¸´ºô½ĞËûÈËÁÄÌìÈ¨Á¦, \n\n"
			        "\t ¸½ÑÔ:·â½ûÆÚÂú \n\n"
			        "\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
			        "\t\t\t\t\t\t\t     \n\n",currentuser.userid);
				autoreport(repbuf,msgbuf,0,currentuser.userid);
				sprintf(secu, "»Ö¸´%sµÄºô½ĞËûÈËÁÄÌìÈ¨Á¦", currentuser.userid);
                      		security_report(secu,1);
				fclose(fp);
				unlink(fname);
				
			} else fclose(fp);
		}//loveni
	}
	if( !HAS_PERM( PERM_MESSAGE ) && !HAS_PERM(PERM_SYSOP))
	{

		sprintf(fname, "home/%c/%s/permmessage", toupper(currentuser.userid[0]), currentuser.userid);
		FILE *fp;
		if ((fp = fopen(fname, "r")) != NULL)
		{
			fgets(days,9,fp);
			day=atoi(days);
			if(day>0)
			if(time(NULL)-file_time(fname) >day * 24 *60 * 60 ) 
			{
				prints( "[1;32m·â½ûÊ±¼äµ½£¬»Ö¸´ÄúµÄ¸øËûÈË·¢ÏûÏ¢È¨Á¦¡£Ğ»Ğ»Ö§³ÖÎÒÃÇµÄ¹¤×÷[m\n" );
				pressanykey();
				currentuser.userlevel |= PERM_MESSAGE;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				sprintf(repbuf,"%s ±»»Ö¸´¸øËûÈË·¢ÏûÏ¢È¨Á¦",currentuser.userid);
				sprintf(msgbuf,
			        "\n  %s ÍøÓÑ: \n\n"
			        "\tÄúÒÑ¾­±»»Ö¸´¸øËûÈË·¢ÏûÏ¢È¨Á¦, \n\n"
			        "\t ¸½ÑÔ:·â½ûÆÚÂú \n\n"
			        "\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
			        "\t\t\t\t\t\t\t     \n\n",currentuser.userid);
				autoreport(repbuf,msgbuf,0,currentuser.userid);
				sprintf(secu, "»Ö¸´%sµÄ¸øËûÈË·¢ÏûÏ¢È¨Á¦", currentuser.userid);
                      		security_report(secu,1);
				fclose(fp);
				unlink(fname);
				
			} else fclose(fp);
		}//loveni
	}


	checkpostperm(fromhost);//by loveni ÅĞ¶ÏÊÇ·ñÓĞÍâÍø·¢ÎÄÈ¨ÏŞ
	CheckBmCloak();








				if(check_limit("logincheck", currentuser.numlogins, 100))  //add by yiyo ÏŞÖÆÃ¿ÌìµÇÂ½´ÎÊı
                                {
                                      security_report("Ê¹ÓÃÉÏÕ¾»ú?",4);
				      prints("´ËÕÊºÅ24Ğ¡Ê±ÄÚµÇÂ½´ÎÊı¹ı¶à, Çë¹ıÒ»¶ÎÊ±¼äÔÙÀ´, ÇëÎğÊ¹ÓÃÉÏÕ¾»ú.\n");
 				      oflush(1);
  				      sleep(3);
  				      exit(0);
                                }
#ifdef CHECK_LESS60SEC

				if (abs(time(0)-currentuser.lastlogout)<60&&!HAS_PERM(PERM_SYSOP) &&
				        strcasecmp(currentuser.userid, "guest") != 0)
				{
					prints("ÄúÔÚ 60 ÃëÄÚÖØ¸´ÉÏÕ¾.Çë°´ [1;36mCtrl+C[m ¼üÈ·ÈÏÄúµÄÉí·İ: ");
					genbuf[0] = igetkey();
					if ( genbuf[0] != Ctrl('C') )
					{
						prints("\n¶Ô²»Æğ£¬Äú²¢Ã»ÓĞ°´ÏÂ CTRL+C ¼ü£¡Äú²»ÄÜ½øÈë±¾Õ¾\n");
						prints("ÈôÓĞÒÉÎÊÇëÍ¨ÖªÕ¾ÎñÈËÔ±, Ğ»Ğ».\n");
						oflush();
						sleep(3);
						exit(1);
					}
					prints("\n");
				}
#endif

#ifdef CHECK_SYSTEM_PASS
				if (HAS_PERM(PERM_SYSOP))
				{
					if(!check_systempasswd())
					{
						prints("\n¿ÚÁî´íÎó, ²»µÃÇ©Èë ! !\n");
						oflush();
						sleep(2);
						exit(1);
					}
				}
#endif
				bzero(passbuf, PASSLEN - 1);
				break;
			}
		}
	} 
  	multi_user_check();
	if (!term_init(currentuser.termtype))
	{
		prints("Bad terminal type.  Defaulting to 'vt100'\n");
		strcpy(currentuser.termtype, "vt100");
		term_init(currentuser.termtype);
	}

	check_tty_lines(); /* 2000.03.14 */
	sethomepath(genbuf, currentuser.userid);
	mkdir(genbuf, 0755);
	login_start_time = time(0);
}

int valid_ident(char* ident)
{
	static char *invalid[] = {"unknown@", "root@", "gopher@", "bbs@",
	                          "guest@", "nobody@", "www@", NULL};
	int     i;
	if (ident[0] == '@')
		return 0;
	for (i = 0; invalid[i] != NULL; i++)
		if (strstr(ident, invalid[i]) != NULL)
			return 0;
	return 1;
}

void write_defnotepad()
{
	currentuser.notedate = time(0);
	set_safe_record();
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	return;
}

void notepad_init()
{
	FILE   *check;
	char    notetitle[STRLEN];
	char    tmp[STRLEN * 2];
	char   *fname, *bname, *ntitle;
	long int maxsec;
	time_t  now;
	maxsec = 86400;
	lastnote = 0;
	if ((check = fopen("etc/checknotepad", "r")) != NULL)
	{
		fgets(tmp, sizeof(tmp), check);
		lastnote = atol(tmp);
		fclose(check);
	}
	now = time(0);
	if ((now - lastnote) >= maxsec)
	{
		move(t_lines - 1, 0);
		prints("¶Ô²»Æğ£¬ÏµÍ³×Ô¶¯·¢ĞÅ£¬ÇëÉÔºò.....");
		refresh();
		check = fopen("etc/checknotepad", "w");
		lastnote = now - (now % maxsec);
		fprintf(check, "%d", lastnote);
		fclose(check);
		if ((check = fopen("etc/autopost", "r")) != NULL)
		{
			while (fgets(tmp, STRLEN, check) != NULL)
			{
				fname = strtok(tmp, " \n\t:@");
				bname = strtok(NULL, " \n\t:@");
				ntitle = strtok(NULL, " \n\t:@");
				if (fname == NULL || bname == NULL || ntitle == NULL)
					continue;
				else
				{
					getdatestring(now,NA);
					sprintf(notetitle, "[%8.8s %6.6s] %s",datestring+6, datestring + 23,ntitle);
					if (dashf(fname))
					{
						Postfile(fname, bname, notetitle, 1);
						sprintf(tmp, "%s ×Ô¶¯ÕÅÌù", ntitle);
						report(tmp);
					}
				}
			}
			fclose(check);
		}
		getdatestring(now,NA);
		sprintf(notetitle, "[%s] ÁôÑÔ°å¼ÇÂ¼", datestring);
		if (dashf("etc/notepad", "r"))
		{
			Postfile("etc/notepad", "notepad", notetitle, 1);
			unlink("etc/notepad");
		}
		report("×Ô¶¯·¢ĞÅÊ±¼ä¸ü¸Ä");
	}
	return;
}


void user_login()
{
	char    fname[STRLEN];
	char   *ruser;
	if (strcmp(currentuser.userid, "SYSOP") == 0)
	{
		currentuser.userlevel = ~0;	/* SYSOP gets all permission bits */
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	}
	ruser = getenv("REMOTEUSER");
	sprintf(genbuf, "%s@%s", ruser ? ruser : "?", fromhost);
	log_usies("ENTER", genbuf);
	u_enter();
	if (ruser != NULL)
	{
		sprintf(genbuf, "%s@%s", ruser, fromhost);
		if (valid_ident(genbuf))
		{
			strncpy(currentuser.ident, genbuf, NAMELEN);
			currentuser.ident[NAMELEN - 1] = '\0';
		}
	}
	report("Enter");
	started = 1;
	if((!HAS_PERM(PERM_MULTILOG|PERM_SYSOP)) 
		&& count_user() > MULTI_LOGINS
	        && strcmp(currentuser.userid,"needhelp")
		&& strcmp(currentuser.userid,"guest")
		&& strcasecmp(currentuser.userid, "BMcoach"))
	{
		report("kicked (multi-login)[Â©ÍøÖ®Óã]");
		abort_bbs();
	}
	initscr();
#ifdef USE_NOTEPAD

	notepad_init();
	if (strcmp(currentuser.userid, "guest") != 0 )
	{
		if (DEFINE(DEF_NOTEPAD))
		{
			int     noteln;
			if (lastnote > currentuser.notedate)
				currentuser.noteline = 0;
			noteln = countln("etc/notepad");
			if (currentuser.noteline == 0)
			{
				shownotepad();
			}
			else if ((noteln - currentuser.noteline) > 0)
			{
				move(0, 0);
				ansimore2("etc/notepad", NA, 0, noteln - currentuser.noteline + 1);
				igetkey();
				clear();
			}
			currentuser.noteline = noteln;
			write_defnotepad();
		}
	}
#endif
	if(show_statshm("0Announce/bbslist/countusr", 0) && DEFINE(DEF_GRAPH))
	{
		refresh();
		pressanykey();
	}
	if ((vote_flag(NULL, '\0', 2 /* ¼ì²é¶Á¹ıĞÂµÄWelcome Ã» */ ) == 0))
	{
		if (dashf("Welcome"))
		{
			ansimore("Welcome", YEA);
			vote_flag(NULL, 'R', 2 /* Ğ´Èë¶Á¹ıĞÂµÄWelcome */ );
		}
		else {
			if (fill_shmfile(3, "Welcome2", "WELCOME_SHMKEY"))
				show_welcomeshm();
		}
	}
	else
	{
		if (fill_shmfile(3, "Welcome2", "WELCOME_SHMKEY"))
			show_welcomeshm();
	}
	show_statshm("etc/posts/day", 1);
	refresh();
	move(t_lines - 2, 0);
	clrtoeol();
	if(currentuser.numlogins < 1)
	{
		currentuser.numlogins = 0;
		getdatestring(time(0),NA);
		prints("[1;36m¡î ÕâÊÇÄúµÚ [33m1[3m ´Î¹âÁÙ±¾Õ¾,Çë¼Ç×¡½ñÌì°É.\n");

		prints("¡î ÄúµÚÒ»´ÎÁ¬Èë±¾Õ¾µÄÊ±¼äÎª [33m%s[m ", datestring);
	}
	else
	{
		getdatestring(currentuser.lastlogin,NA);
		prints("[1;36m¡î ÕâÊÇÄúµÚ [33m%d[3m ´Î¹âÁÙ±¾Õ¾£¬ÉÏ´ÎÄúÊÇ´Ó [33m%s[36m Á¬Íù±¾Õ¾¡£\n", currentuser.numlogins + 1, currentuser.lasthost);
		prints("¡î ÉÏ´ÎÁ¬ÏßÊ±¼äÎª [33m%s[m ", datestring);
	}
	igetkey();
	/* Stop this service now */
	//show_help("0Announce/bbslist/newsday");//add by yiyo È«¹úÊ®´óÈÈÃÅ
	if (dashf("etc/announce"))
	{
		ansimore("etc/announce",NA);
		pressanykey();
		//pressreturn();
	}//add by yiyo ÖØÒª¹«¸æ
	if (DEFINE(DEF_TOP10))
	{
		ansimore("etc/bless/day",NA);
		pressanykey();
	}
	if (DEFINE(DEF_CAMPUS))
	{
		ansimore("etc/bbsannounce",NA);
		pressanykey();
		ansimore("etc/campus",NA);
		pressanykey();
	}//loveni:±¾Õ¾¹«¸æ

	


	WishNum = 9999;



	setuserfile(fname, BADLOGINFILE);
	if (ansimore(fname, NA) != -1)
	{
		if (askyn("ÄúÒªÉ¾³ıÒÔÉÏÃÜÂëÊäÈë´íÎóµÄ¼ÇÂ¼Âğ", NA, NA) == YEA)
			unlink(fname);
	}

        
	  setuserfile(fname, "msgfile.me");
if(get_num_records(fname,129)>1000)
{
  clear();
  prints("ÁÄÌì¼ÇÂ¼¹ıÁ¿£¬(C)É¾³ı, (M)¼Ä»ØĞÅÏä, (G)¼ÌĞø [G]: ");
 
  time_t now;
  switch (egetch())
  {
  case 'C':
  case 'c':
          unlink(fname);
          break;

  case 'M':
  case 'm':
             now = time(0);
             getdatestring(now,NA);
             char title[STRLEN];
             sprintf(title, "[%s] ËùÓĞÑ¶Ï¢±¸·İ", datestring);
             mail_file(fname, currentuser.userid, title);
             unlink(fname);
           break;
  }

}

	set_safe_record();
	check_uinfo(&currentuser,0);
	strncpy(currentuser.lasthost, fromhost, 16);
	currentuser.lasthost[15] = '\0';	/* dumb mistake on my part */


        if(!uinfo.invisible)
	currentuser.lastlogin = time(0);

	if (HAS_PERM(PERM_SYSOP) || !strcmp(currentuser.userid, "guest"))
		currentuser.lastjustify = time(0);

	//	if(time(0)>currentuser.lastjustify)
	//		currentuser.userlevel |= (PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);//:)

	/*
	   if (    HAS_PERM(PERM_LOGINOK) 
	       && (abs(time(0)-currentuser.lastjustify)>=REG_EXPIRED * 86400)) {
	#ifdef MAILCHECK       
	          currentuser.email[0]= '\0';
	#endif	  
		  currentuser.address[0]='\0';
		  currentuser.userlevel &= ~(PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);
		  mail_file("etc/expired", currentuser.userid, "¸üĞÂ¸öÈË×ÊÁÏËµÃ÷¡£");
	   }*/
	report_user_enter();     // add by nicky 2002.4.3
	currentuser.numlogins++;
	currentuser.userlevel&=~PERM_SUICIDE;
	//add by yiyo »Ö¸´×ÔÉ±
#ifdef ALLOWGAME

	if( currentuser.money > 1000000 )
	{
		currentuser.nummedals += currentuser.money / 10000;
		currentuser.money %= 1000000;
	}
	if((signed int)(currentuser.money - currentuser.bet) < -4990
	        && currentuser.numlogins < 10  && currentuser.numposts < 10
	        && currentuser.nummedals == 0)
		currentuser.money += 1000;
#endif

	if (currentuser.firstlogin == 0)
	{
		currentuser.firstlogin = time(0)-7*86400;
	}

	//add by madoldman
  	//ÖØĞÂÅĞ¶ÏÉÏÕ¾»ú£¬ĞŞÕıË¢ÉÏÕ¾bug
  	if (check_limit ("logincheck", currentuser.numlogins, 100))
    	{
      		security_report ("Ê¹ÓÃÉÏÕ¾»ú?", 4);
	  	clear();
      		prints("´ËÕÊºÅ24Ğ¡Ê±ÄÚµÇÂ½´ÎÊı¹ı¶à, Çë¹ıÒ»¶ÎÊ±¼äÔÙÀ´, ÇëÎğÊ¹ÓÃÉÏÕ¾»ú.\n");
      		oflush (1);
      		sleep (3);
      		exit (0);
    	}	

	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	check_register_info();
}


void set_numofsig()
{
	int     sigln;
	char    signame[STRLEN];
	setuserfile(signame, "signatures");
	sigln = countln(signame);
	numofsig = sigln / MAXSIGLINES;
	if ((sigln % MAXSIGLINES) != 0)
		numofsig += 1;
}

#ifdef CHK_FRIEND_BOOK
int chk_friend_book()
{
	FILE   *fp;
	int     idnum, n = 0;
	char    buf[STRLEN], *ptr;
	if ((fp = fopen("friendbook", "r")) == NULL)
		return 0;

	move(10, 0);
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		char    uid[14];
		char    msg[STRLEN];
		struct user_info *uin;
		ptr = strstr(buf, "@");
		if (ptr == NULL)
			continue;
		ptr++;
		strcpy(uid, ptr);
		ptr = strstr(uid, "\n");
		*ptr = '\0';
		idnum = atoi(buf);
		if (idnum != usernum || idnum <= 0)
			continue;
		uin = t_search(uid, NA);
		sprintf(msg, "%s ÒÑ¾­ÉÏÕ¾¡£", currentuser.userid);
		if (!uinfo.invisible && uin != NULL && !DEFINE(DEF_NOLOGINSEND)
		        && do_sendmsg(uin, msg, 2, uin->pid) == 1)
		{
			prints("[1m%s[m ÕÒÄã£¬ÏµÍ³ÒÑ¾­¸æËßËûÄãÉÏÕ¾µÄÏûÏ¢¡£\n", uid);
		}
		else
			prints("[1m%s[m ÕÒÄã£¬ÏµÍ³ÎŞ·¨ÁªÂçµ½Ëû£¬ÇëÄã¸úËûÁªÂç¡£\n", uid);
		n++;
		del_from_file("friendbook", buf);
		if (n > 15)
		{
			pressanykey();
			move(10, 0);
			clrtobot();
		}
	}
	fclose(fp);
	if(n)
	{
		move(8,0);
		prints("[1mÏµÍ³Ñ°ÈËÃû²áÁĞ±í:[m");
	}
	return n;
}
#endif

int check_maxmail()
{
	extern char currmaildir[STRLEN];
	int      maxsize, mailsize,oldnum;
	/*maxmail = (HAS_PERM(PERM_SYSOP)) ?
	MAX_SYSOPMAIL_HOLD : (HAS_PERM(PERM_BOARDS)) ?
	MAX_BMMAIL_HOLD : MAX_MAIL_HOLD;
	*/
	set_safe_record();
	oldnum = currentuser.nummails;
	currentuser.nummails=get_num_records(currmaildir, sizeof(struct fileheader));
	if(oldnum != currentuser.nummails)
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	if(HAS_PERM(PERM_MAILINFINITY))
		return 0;
	maxsize = getmailboxsize(currentuser);
	mailsize = getmailsize(currentuser.userid);
	// if (currentuser.nummails > maxmail || mailsize > maxsize) {
	if ( mailsize > maxsize)
	{
		mailXX = 1;
		clear();
		move(4,0);
		/*
		if( currentuser.nummails > maxmail ) 
		   prints("ÄúµÄË½ÈËĞÅ¼ş¸ß´ï %d ·â, ÄúµÄÓÊ¼şÉÏÏŞ: %d ·â\n", 
		currentuser.nummails, maxmail); 
		*/
		if( mailsize > maxsize )
			prints("ÄúµÄĞÅ¼şÈİÁ¿¸ß´ï %d K£¬ÄúµÄÈİÁ¿ÉÏÏŞ: %d K\n",
			       mailsize, maxsize);
		prints("ÄúµÄË½ÈËĞÅ¼şÒÑ¾­³¬ÏŞ, ÇëÕûÀíĞÅÏä£¬·ñÔòÎŞ·¨Ê¹ÓÃ±¾Õ¾µÄËÍĞÅ¹¦ÄÜ¡£\n");
		/*
		if (currentuser.nummails > maxmail + 100 ) {
		   sprintf(genbuf, "Ë½ÈËĞÅ¼ş¹ıÁ¿: %d ·â", currentuser.nummails);
		security_report(genbuf,4);
		}
		*/
		if (mailsize  > maxsize + 1000 )
		{
			sprintf(genbuf, "Ë½ÈËĞÅ¼ş¹ıÁ¿: %d K", mailsize);
			security_report(genbuf,4);
		}
	}
	else
		mailXX = 0;  //add by yiyo½â¾öĞÅ¼ş¹ıÁ¿ÌáÊ¾ÎÊÌâ
	return mailXX;
}
#ifndef BBSD
int main(int argc, char** argv)
#else
void start_client()
#endif
{
	load_sysconf();

#ifdef ALLOWSWITCHCODE

	resolve_GbBig5Files();
#endif

#ifndef BBSD

	if (argc < 2 || *argv[1] != 'h')
	{
		printf("You cannot execute this program directly.\n");
		exit(-1);
	}
	system_init(argc, argv);
#else

	system_init();
#endif

	if (setjmp(byebye))
	{
		system_abort();
	}
#ifndef BBSD
	get_tty();
	init_tty();
#endif

	login_query();
	user_login();
	m_init();
	RMSG = NA;
	clear();
	c_recover();
#ifdef TALK_LOG

	tlog_recover();		/* 990713.edwardc for talk_log recover */
#endif

	if (strcmp(currentuser.userid, "guest"))
	{
		if (HAS_PERM(PERM_ACCOUNTS) && dashf("new_register"))
		{
			/* modified by yiyo ½øÕ¾Ê±´ı×¢²á¾ßÌåÈËÊıµÄÌáÊ¾ */
			FILE *fp;
			int m;
			system("wc new_register > tmp/tmp.tmp");
			fp=fopen("tmp/tmp.tmp", "r");
			fscanf(fp, "%d", &m);
			fclose(fp);
			prints("[0;1m\n\n");
			prints("[1;33mÄ¿Ç°ÓĞĞÂÊ¹ÓÃÕß [35m%d[33m ÈËÕıÔÚµÈºòÄúµÄÉóÅú¡£[m\n\n",m/8);
			//	 prints("[33mÄ¿Ç°ÓĞĞÂÊ¹ÓÃÕßµÈºòÄúµÄÉóÅú¡£[37m\n\n");
#ifdef SHOW_THANKYOU

			prints("¸ĞĞ»ÄúÊ¹ÓÃ FB2000£¡·Ç³£Ï£ÍûÄúÄÜ¾­³£¹â¹Ë [32m¿ìÒâ¹àË®Õ¾ [telnet fb2000.dhs.org][37m £¡\n");
			prints("Èç¹û¿ÉÄÜµÄ»°£¬[45;33mÇë»Ø±¨ÄúµÄÊ¹ÓÃÇé¿ö![0;1;37m£¬°üÀ¨£º BUG¡¢ÒÉÎÊ¡¢½¨Òé£¡·Ç³£¸ĞĞ»£¡\n\n");
			prints("Èç¹ûÄúÏ£Íû¼°Ê±ÁË½â FB2000 µÄ×îĞÂÇé¿ö£¬Çë¶©ÔÄ¡¾FB2000 ¿ª·¢Í¨Ñ¶¡¿ÓÊ¼şÁĞ±í£¡\n");
			prints("¶©ÔÄ·½·¨£ºĞ´ĞÅ¸ø fb2000-request@list.cn99.com£¬ÕıÎÄÌîĞ´£ºsubscribe");
#endif

			pressanykey();
			//	 pressreturn();
		}

		/* add by bluetent 2003.4.1, ½øÕ¾Ë¢ĞÂ¸÷¸ö°åÃæÔÚÏßÊı¾İ */
		RefreshBoardOnline();
		/* add end */
		if(check_maxmail())
			pressanykey();
#ifdef CHK_FRIEND_BOOK

		if(chk_friend_book())
			pressanykey();
#endif

		move(9, 0);
		clrtobot();
		if (!DEFINE(DEF_NOLOGINSEND))
			if (!uinfo.invisible)
				apply_ulist(friend_login_wall);
		clear();
		set_numofsig();
	}

	ActiveBoard_Init();

	fill_date();		/* ¼ì²é¼ÍÄîÈÕ */
	b_closepolls();	/* ¹Ø±ÕÍ¶Æ± */

	

	num_alcounter();

	boardmargin();/*³õÊ¼»¯°åÃæ*/
	if (count_friends > 0 && DEFINE(DEF_LOGFRIEND)) t_friends();
	check_x_vote();//added by loveni
	while (1)
	{
		if (DEFINE(DEF_NORMALSCR))
			domenu("TOPMENU");
		else
			domenu("TOPMENU2");
		Goodbye();
	}
}

int     refscreen = NA;

int egetch()
{
	int     rval;
	check_calltime();
	if (talkrequest)
	{
		talkreply();
		refscreen = YEA;
		return -1;
	}
	while (1)
	{
		rval = igetkey();
		if (talkrequest)
		{
			talkreply();
			refscreen = YEA;
			return -1;
		}
		if (rval != Ctrl('L'))
			break;
		redoscr();
	}
	refscreen = NA;
	return rval;
}

char   * boardmargin()
{
	static char buf[STRLEN];
	if (selboard)
		sprintf(buf, "ÌÖÂÛÇø [%s]", currboard);
	else
	{
		brc_initial(DEFAULTBOARD);
		if (!getbnum(currboard))
			setoboard(currboard);
		selboard = 1;
		sprintf(buf, "ÌÖÂÛÇø [%s]", currboard);
	}
	return buf;
}

void update_endline()
{
	char    buf[255],fname[STRLEN], *ptr;
	time_t  now;
	FILE *fp;
	int     i, allstay, foo, foo2;
	if (!DEFINE(DEF_ENDLINE))
		return;

	now = time(0);
	allstay = getdatestring (now,YEA); // allstay Îªµ±Ç°ÃëÊı


	//bluetent
	if(allstay%2==0&&DEFINE(DEF_SYSMSG)&&uinfo.mode!=FRIEND&&uinfo.mode!=LUSERS)
	{
		move(t_lines - 1, 0);
		clrtoeol();
		if(show_endline())
			return;
	}
	//end



	move(t_lines - 1, 0);
	clrtoeol();



	if(allstay == 0)
	{
nowishfile:
		resolve_boards();
		strcpy(datestring, brdshm->date);
		allstay = 1;
	}
	if(allstay < 5 )
	{
		allstay = (now - login_start_time) / 60;
		sprintf(buf, "[[36m%.12s[33m]", currentuser.userid);
		num_alcounter();
		prints( "[1;44;33m[[36m%29s[33m][[36m%4d[33mÈË/[1;36m%3d[33mÓÑ][[36m%1s%1s%1s%1s%1s%1s[33m]ÕÊºÅ%-24s[[36m%3d[33m:[36m%2d[33m][m",
		        datestring,count_users,count_friends,(uinfo.pager&ALL_PAGER)?"P":"p",
		        (uinfo.pager&FRIEND_PAGER)?"O":"o",(uinfo.pager&ALLMSG_PAGER)?"M":"m",
		        (uinfo.pager&FRIENDMSG_PAGER)?"F":"f",(DEFINE(DEF_MSGGETKEY))?"X":"x",
		        (uinfo.invisible==1)?"C":"c",buf,(allstay /60)%1000,allstay%60);
		return ;
	}

	//   if(uinfo.mode==FRIEND||uinfo.mode==LUSERS)//bluetent
	//	   return;
	setuserfile(fname, "HaveNewWish");
	if(WishNum == 9999 || dashf(fname))
	{
		if(WishNum != 9999)
			unlink(fname);
		WishNum = 0;
		orderWish = 0;

		if(is_birth(currentuser))
		{
			strcpy(GoodWish[WishNum],
			       "                     À²À²¡«¡«£¬ÉúÈÕ¿ìÀÖ!   ¼ÇµÃÒªÇë¿ÍÓ´ :P                   ");
			WishNum ++;
		}
		setuserfile(fname, "GoodWish");
		if( (fp = fopen(fname,"r")) != NULL)
		{
			for(; WishNum < 20 ;)
			{
				if(fgets(buf,255,fp)==NULL)
					break;
				buf[STRLEN-4] = '\0';
				ptr = strtok(buf, "\n\r");
				if(ptr == NULL || ptr[0] == '#')
					continue;
				strcpy(buf,ptr);
				for(ptr=buf;*ptr==' '&& *ptr!=0; ptr ++)
					;
				if(*ptr == 0 || ptr[0] == '#')
					continue;
				for(i= strlen(ptr)-1; i<0; i--)
					if(ptr[i] != ' ')
						break;
				if(i < 0)
					continue;
				foo = strlen(ptr);
				foo2 = (STRLEN - 3 - foo) / 2;
				strcpy(GoodWish[WishNum], "");
				for(i = 0; i < foo2; i++)
					strcat(GoodWish[WishNum], " ");
				strcat(GoodWish[WishNum], ptr);
				for(i=0;i<STRLEN-3-(foo+foo2);i++)
					strcat(GoodWish[WishNum]," ");
				GoodWish[WishNum][STRLEN-4] = '\0';
				WishNum ++;

			}
			fclose(fp);
		}
	}
	if(WishNum == 0)
		goto nowishfile;
	if(uinfo.mode==FRIEND||uinfo.mode==LUSERS)
		goto nowishfile;
	if(orderWish>=WishNum*2)
		orderWish=0;
	prints("[0;1;44;33m[[36m%s[33m][m",GoodWish[orderWish/2]);
	//   prints("[0;1;44;36m%s[m",GoodWish[orderWish/2]);
	orderWish ++;


}

/*ReWrite by SmallPig*/
void showtitle(char* title, char* mid)
{
	char    buf[STRLEN], *note;
	int     spc1, spc2;
	note = boardmargin();
	spc1 = 39 + num_ans_chr(title) - strlen(title) - strlen(mid) / 2;
	if(spc1 < 2)
		spc1 = 2;
	spc2 = 79 - (strlen(title)-num_ans_chr(title)+spc1 + strlen(note) + strlen(mid));
	//if (spc1 < 1) spc1 = 1;
	if (spc2 < 1)
		spc2 = 1;
	move(0, 0);
	clrtoeol();
	sprintf(buf, "%*s", spc1, "");
	if (!strcmp(mid, BoardName))
		prints("[1;44;33m%s%s[37m%s[1;44m", title, buf, mid);
	else if (mid[0] == '[')
		prints("[1;44;33m%s%s[5;36m%s[m[1;44m", title, buf, mid);
	else
		prints("[1;44;33m%s%s[36m%s", title, buf, mid);
	sprintf(buf, "%*s", spc2, "");
	prints("%s[33m%s[m\n", buf, note);
	update_endline();
	move(1, 0);
}
void firsttitle(char* title)
{
	char    middoc[30];
	char	buf[80];
	if (chkmail()&&mail_unread()>0)
	{
		if(strstr(title,"ÌÖÂÛÇøÁĞ±í"))
		{
			sprintf(buf, "[%d·âĞÂĞÅ, °´ M ²é¿´]", mail_unread());
		}
		else
		{
			sprintf(buf, "[%d·âĞÂĞÅ, °´ V ²é¿´]", mail_unread());
		}
		strcpy(middoc, buf);
	}
	else if (mailXX == 1)
		strcpy(middoc, "[ĞÅ¼ş¹ıÁ¿£¬ÇëÕûÀíĞÅ¼ş!]");
	else
		strcpy(middoc, BoardName);

	showtitle(title, middoc);
}
void docmdtitle(char* title, char* prompt)
{
	firsttitle(title);
	move(1, 0);
	clrtoeol();
	prints("%s", prompt);
	clrtoeol();
}

void c_recover()
{
	char    fname[STRLEN], buf[STRLEN];
	int     a;
	sprintf(fname, "home/%c/%s/%s.deadve", toupper(currentuser.userid[0]), currentuser.userid, currentuser.userid);
	if (!dashf(fname) || strcmp(currentuser.userid, "guest") == 0)
		return;
	clear();
	strcpy(genbuf, "");
	getdata(0,0, "[1;32mÄúÓĞÒ»¸ö±à¼­×÷Òµ²»Õı³£ÖĞ¶Ï£¬(S) Ğ´ÈëÔİ´æµµ (M) ¼Ä»ØĞÅÏä (Q) ËãÁË£¿[M]£º[m", genbuf, 2, DOECHO, YEA);
	switch (genbuf[0])
	{
	case 'Q':
	case 'q':
		unlink(fname);
		break;
	case 'S':
	case 's':
		while (1)
		{
			strcpy(genbuf, "");
			getdata(2, 0, "[1;33mÇëÑ¡ÔñÔİ´æµµ [0-7] [0]£º[m", genbuf, 2, DOECHO, YEA);
			if (genbuf[0] == '\0')
				a = 0;
			else
				a = atoi(genbuf);
			if (a >= 0 && a <= 7)
			{
				sprintf(buf, "home/%c/%s/clip_%d", toupper(currentuser.userid[0]), currentuser.userid, a);
				if (dashf(buf))
				{
					getdata(3,0, "[1;31mÔİ´æµµÒÑ´æÔÚ£¬¸²¸Ç»ò¸½¼Ó? (O)¸²¸Ç (A)¸½¼Ó [O]£º[m", genbuf, 2, DOECHO, YEA);
					switch (genbuf[0])
					{
					case 'A':
					case 'a':
						f_cp(fname, buf, O_APPEND);
						unlink(fname);
						break;
					default:
						unlink(buf);
						rename(fname, buf);
						break;
					}
				}
				else
					rename(fname, buf);
				break;
			}
		}
		break;
	default:
		mail_file(fname, currentuser.userid, "²»Õı³£¶ÏÏßËù±£ÁôµÄ²¿·İ...");
		unlink(fname);
		break;
	}
}

#ifdef TALK_LOG
void tlog_recover()
{
	char    buf[256];
	sprintf(buf, "home/%c/%s/talk_log", toupper(currentuser.userid[0]), currentuser.userid);

	if (strcasecmp(currentuser.userid, "guest") == 0 || !dashf(buf))
		return;

	clear();
	strcpy(genbuf, "");
	getdata(0, 0, "[1;32mÄúÓĞÒ»¸ö²»Õı³£¶ÏÏßËùÁôÏÂÀ´µÄÁÄÌì¼ÇÂ¼, ÄúÒª .. (M) ¼Ä»ØĞÅÏä (Q) ËãÁË£¿[Q]£º[m", genbuf, 2, DOECHO, YEA);

	if (genbuf[0] == 'M' || genbuf[0] == 'm')
	{
		mail_file(buf, currentuser.userid, "ÁÄÌì¼ÇÂ¼");
	}
	unlink(buf);
	return;
}
#endif

/* ²éÑ¯ÉÏÏßµØÖ·ÊÇ·ñ±»ÓÃ»§ÉèÖÃ·â½û bluetent 2003.01.15 */
int bad_host(char *name, char *buf2)
{
	FILE   *list;
	char    buf[40], *ptr;
	int allow=0;
	int stat=0;
	if (list = fopen(buf2, "r"))
	{
		while (fgets(buf, 40, list))
		{
			ptr = strtok(buf, " \n\t\r");
			if (ptr != NULL && *ptr != '#')
			{
				if(!strcmp(ptr, "allow"))
				{
					allow=1;
					stat=1;
				}
				if(!strcmp(ptr, name))
					stat=(allow?0:1);
				if(ptr[0] == '#')
					continue;
				if(ptr[0] == '-' && !strcmp(name,&ptr[1]))
					stat=(allow?1:0);
				if(ptr[strlen(ptr)-1]=='.'&&!strncmp(ptr,name,strlen(ptr)-1))
					stat=(allow?0:1);
				if(ptr[0]=='.'&&strlen(ptr)<strlen(name)&&
				        !strcmp(ptr,name+strlen(name)-strlen(ptr)))
					stat=(allow?0:1);
			}
		}
		fclose(list);
	}
	return stat;
}

int RefreshBoardOnline()
{
	struct shortfile *bptr;
	struct user_info *x;
	extern struct UTMPFILE *utmpshm;
	extern struct shortfile *bcache;
	int n, i, j;
	char title[MAXBOARD][80];
	int inboard[MAXBOARD];
	resolve_boards();
	for (n = 0; n < numboards; n++)
	{
		bptr = &bcache[n];
		strcpy(title[n], bptr->filename);
	}
	for(j=0; j< numboards; j++)
		inboard[j]=0;
	for(i=0; i<MAXACTIVE; i++)
	{
		x=&(utmpshm->uinfo[i]);
		if(x->board[0]=='-')
			continue;
		for(j=0; j< numboards; j++)
			if(!strcmp(title[j], x->board))
			{
				inboard[j]++;
				break;
			}
	}
	for (j=0; j<numboards ;j++)
	{
		bptr = &bcache[j];
		//		(int*) (bptr->filename+72)=inboard[j];
		//		substitute_record(BOARDS, bptr, sizeof(*bptr), j);
		*(int*) (bcache[j].filename+72)=inboard[j];
	}
	return 1;
}

//huangxu@071203:ÎªÊ²Ã´½ø³ÌËÀ²»µô£¿
int bbsd_exit(int exitcode)
{
	char buf[64];
	exit(exitcode);
	sprintf(buf, "kill -9 %d", getpid());
	system(buf);//×ÔÉ±£¬hoho¡£
	return exitcode;
}

