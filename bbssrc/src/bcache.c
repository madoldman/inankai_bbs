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
$Id: bcache.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>
#define chartoupper(c)  ((c >= 'a' && c <= 'z') ? c+'A'-'a' : c)

struct BCACHE *brdshm=NULL;
struct UCACHE *uidshm=NULL;
struct UTMPFILE *utmpshm=NULL;
struct userec lookupuser;
struct shortfile *bcache=NULL;
int     usernumber=0;
int     numboards = -1;
int		tmpmode;//add by bluetent 2002-10-30
int		iswwwmode;//add by bluetent 2002-10-30
extern int	forceresolve;

void attach_err(int shmkey, char* name, int err)
{
	sprintf(genbuf, "Error! %s error #%d! key = %x.\n", name, err, shmkey);
	write(1, genbuf, strlen(genbuf));
	exit(1);
}

int search_shmkey(char *keyname)
{
	int     i = 0, found = 0;
	while (shmkeys[i].key != NULL)
	{
		if (strcmp(shmkeys[i].key, keyname) == 0)
		{
			found = shmkeys[i].value;
			break;
		}
		i++;
	}

	if (found == 0)
	{
		sprintf(genbuf, "search_shmkey(): cannot found %s SHMKEY entry!", keyname);
		report(genbuf);
	}
	return found;
}

void* attach_shm(char* shmstr, int defaultkey, int shmsize)
{
	void   *shmptr;
	int     shmkey, shmid;
	shmkey = search_shmkey(shmstr);
	if (shmkey < 1024)
		shmkey = defaultkey;
	shmid = shmget(shmkey, shmsize, 0);
	if (shmid < 0)
	{
		shmid = shmget(shmkey, shmsize, IPC_CREAT | 0640);
		if (shmid < 0)
			attach_err(shmkey, "shmget", errno);
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
			attach_err(shmkey, "shmat", errno);
		memset(shmptr, 0, shmsize);
	}
	else
	{
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
			attach_err(shmkey, "shmat", errno);
	}
	return shmptr;
}

int fillbcache(struct boardheader* fptr)
{
	struct shortfile *bptr;
	if (numboards >= MAXBOARD)
		return 0;
	bptr = &bcache[numboards++];
	memcpy(bptr, fptr, sizeof(struct shortfile));
	return 0;
}

void resolve_boards()
{
	struct stat st;
	time_t  now;
	if (brdshm == NULL)
	{
		brdshm = attach_shm("BCACHE_SHMKEY", 3693, sizeof(*brdshm));
	}
	numboards = brdshm->number;
	bcache = brdshm->bcache;
	now = time(0);
	if (stat(BOARDS, &st) < 0)
	{
		st.st_mtime = now - 3600;
	}
	if (brdshm->uptime < st.st_mtime || brdshm->uptime < now - 3600)
	{
		log_usies("CACHE", "reload bcache");
		numboards = 0;
		apply_record(BOARDS, fillbcache, sizeof(struct boardheader));
		brdshm->number = numboards;
		brdshm->uptime = now;
	}
}

int apply_boards(int (*func)())
{
	register int i;
	resolve_boards();
	for (i = 0; i < numboards; i++)
		if (bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level) || (bcache[i].level & PERM_NOZAP))
			if ((*func) (&bcache[i]) == QUIT)
				return QUIT;
	return 0;
}

struct shortfile * getbcache(char* bname)
{
	register int i;
	resolve_boards();
	for (i = 0; i < numboards; i++)
		if (!ci_strncmp(bname, bcache[i].filename, STRLEN))
			return &bcache[i];
	return NULL;
}

int getbnum(char *bname)
{
	register int i;
	resolve_boards();

	for (i = 0; i < numboards; i++)
	{
		if(bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level)
		        ||(bcache[i].level & PERM_NOZAP))
		{
			if (!ci_strncmp(bname, bcache[i].filename, STRLEN))
				return i + 1;
		}
	}
	return 0;
}

void setoboard(char *bname)
{
	register int i;

	resolve_boards();
	for (i = 0; i < numboards; i++)
	{
		if (    bcache[i].level & PERM_POSTMASK || HAS_PERM(bcache[i].level)
		        || (bcache[i].level & PERM_NOZAP))
		{
			if(bcache[i].filename[0] != 0 && bcache[i].filename[0] != ' ')
			{
				strcpy(bname, bcache[i].filename);
				return ;
			}
		}
	}
}

int haspostperm(char* bname)
{
	register int i;
	if (digestmode)
		return 0;
	if ( deny_me(bname) && !HAS_PERM(PERM_SYSOP))
		return 0;
	if (strcmp(bname, DEFAULTBOARD) == 0)
		return 1;
	if ( (!strcmp(bname, "Appeal")||(!strcmp(bname,"Complain"))) && strcmp(lookupuser.userid,"guest") && HAS_PERM(PERM_LOGINOK))
		return 1;
	if ((i = getbnum(bname)) == 0)
		return 0;
	set_safe_record();
	if (!HAS_PERM(PERM_POST))
		return 0;
	return (HAS_PERM((bcache[i - 1].level & ~PERM_NOZAP) & ~PERM_POSTMASK));
}

int normal_board(char* bname)
{
	register int i;
	if (strcmp (bname, "Nankai_News") ==0)
		return 1;
	if (strcmp(bname, DEFAULTBOARD) == 0)
		return 1;
	if ((i = getbnum(bname)) == 0)
		return 0;
	if (bcache[i - 1].level & PERM_NOZAP)
		return 1;
	return (bcache[i - 1].level == 0);
}

int fillucache(struct userec* uentp)
{
	if (usernumber < MAXUSERS)
	{
		strncpy(uidshm->userid[usernumber], uentp->userid, IDLEN + 1);
		uidshm->userid[usernumber++][IDLEN] = '\0';
	}
	return 0;
}

void resolve_ucache()
{
	struct stat st;
	int     ftime;
	if (uidshm == NULL)
	{
		uidshm = attach_shm("UCACHE_SHMKEY", 3696, sizeof(*uidshm));
	}
	if (stat(FLUSH, &st) < 0)
	{
		ftime = time(0) - 86400;
	}
	else
		ftime = st.st_mtime;
	if (uidshm->uptime < ftime||forceresolve)
	{
		log_usies("CACHE", "reload ucache");
		/* uidshm->uptime = ftime; */
		usernumber = 0;
		apply_record(PASSFILE, fillucache, sizeof(struct userec));
		uidshm->number = usernumber;
		uidshm->uptime = ftime;
	}
}

void setuserid(int num, char* userid)
{
	if (num > 0 && num <= MAXUSERS)
	{
		if (num > uidshm->number)
			uidshm->number = num;
		strncpy(uidshm->userid[num - 1], userid, IDLEN + 1);
	}
}

int searchnewuser()
{
	register int num, i;
	resolve_ucache();
	num = uidshm->number;
	for (i = 0; i < num; i++)
		if (uidshm->userid[i][0] == '\0')
			return i + 1;
	if (num < MAXUSERS)
		return (num + 1);
	return 0;
}

void getuserid(char* userid, unsigned short int uid)
{
	resolve_ucache();
	strcpy(userid, uidshm->userid[uid - 1]);
}

int searchuser(char* userid)
{
	register int i;
	resolve_ucache();
	//	for (i = 0; i < uidshm->number; i++)
	for (i = 0; i < MAXUSERS; i++)
		if (!ci_strncmp(userid, uidshm->userid[i], IDLEN + 1))
			return i + 1;
	return 0;
}
/*
int
apply_users(func)
void (*func)() ;
{
    register int i ;
    resolve_ucache() ;
    for(i=0; i < uidshm->number; i++)
        (*func)(uidshm->userid[i],i+1) ;
    return 0 ;
}
*/
int getuser(char* userid)
{
	int     uid = searchuser(userid);
	if (uid == 0)
		return 0;
	get_record(PASSFILE, &lookupuser, sizeof(lookupuser), uid);
	return uid;
}

char* u_namearray(char buf[][IDLEN + 1], int* pnum, char* tag)
{
	register struct UCACHE *reg_ushm = uidshm;
	register char *ptr, tmp;
	register int n, total;
	char    tagbuf[STRLEN];
	int     ch, num = 0;
	resolve_ucache();
	if (*tag == '\0')
	{
		*pnum = reg_ushm->number;
		return reg_ushm->userid[0];
	}
	for (n = 0; tag[n] != '\0'; n++)
	{
		tagbuf[n] = chartoupper(tag[n]);
	}
	tagbuf[n] = '\0';
	ch = tagbuf[0];
	total = reg_ushm->number;
	for (n = 0; n < total; n++)
	{
		ptr = reg_ushm->userid[n];
		tmp = *ptr;
		if (tmp == ch || tmp == ch - 'A' + 'a')
			if (chkstr(tag, tagbuf, ptr))
				strcpy(buf[num++], ptr);
	}
	*pnum = num;
	return buf[0];
}

void resolve_utmp()
{
	if (utmpshm == NULL)
	{
		utmpshm = attach_shm("UTMP_SHMKEY", 3699, sizeof(*utmpshm));
	}
}

int getnewutmpent( struct user_info *up)
{
	int utmpfd;
	struct user_info *uentp;
	time_t  now;
	int     i, n, num[2];
	FILE * fp;

        fp=fopen(ULIST, "rw+");
	utmpfd = fileno(fp);
	if (utmpfd < 0)
		return -1;
        
	flock(utmpfd, LOCK_EX);

	resolve_utmp();
	flock(utmpfd, LOCK_EX);
	if (utmpshm->max_login_num < count_users)
		utmpshm->max_login_num = count_users;
	for (i = 0; i < USHM_SIZE; i++)
	{
		uentp = &(utmpshm->uinfo[i]);
		if (!uentp->active || !uentp->pid)
			break;
	}
	if (i >= USHM_SIZE)
	{
		flock(utmpfd, LOCK_UN);
		close(utmpfd);/* add by yiyo */
		return -2;
	}
	utmpshm->uinfo[i] = *up;

	now = time(0);
	if (now > utmpshm->uptime + 60)
	{
		num[0] = num[1] = 0;
		utmpshm->uptime = now;
		for (n = 0; n < USHM_SIZE; n++)
		{
			uentp = &(utmpshm->uinfo[n]);
			if (uentp->active && uentp->pid)
			{
                                //webµÇÂ½Ã»ÓÐÊØ»¤½ø³Ì£¬pidÃ»ÓÐÒâÒå£¬ËùÒÔ£¬²»¼ì²âwebµÄpid
				if (uentp->mode < 20000 && kill(uentp->pid, 0) == -1)
				{
					//huangxu@071220:Ã²ËÆ²»ÄÜÕâÑù
					//kill(uentp->pid, 9);
					//memset(uentp, 0, sizeof(struct user_info));
					continue;
				}
				else
				{
					num[(uentp->invisible == YEA) ? 1 : 0]++;
					//huangxu@071203:Õâ¸öÊý×éÓÐÂïÓÃ£¿»¹ÊÇ³¢ÊÔÇå0°É¡£
					//memset(uentp, 0, sizeof(struct user_info));
					//todo:ÐèÒªÒ»¸ö×¨ÃÅµÄº¯Êý´¦ÀíÊ±¼ä¼ÆËã¡£
				}
			}
		}
		utmpshm->usersum = allusers();
		n = USHM_SIZE - 1;
		while (n > 0 && utmpshm->uinfo[n].active == 0)
			n--;
		ftruncate(utmpfd, 0);
		write(utmpfd, utmpshm->uinfo, (n + 1) * sizeof(struct user_info));
	}
	flock(utmpfd, LOCK_UN);
	close(utmpfd);
	return i + 1;
}

int apply_ulist(int (*fptr)())
{
	struct user_info *uentp, utmp;
	int     i, max;

	resolve_utmp();
	max = USHM_SIZE - 1;
	while (max > 0 && utmpshm->uinfo[max].active == 0)
		max--;
	for (i = 0; i <= max; i++)
	{
		uentp = &(utmpshm->uinfo[i]);
		utmp = *uentp;
		if ((*fptr) (&utmp) == QUIT)
			return QUIT;
	}
	return 0;
}

int apply_ulist_address(int (*fptr)())
{
	int     i, max;
	resolve_utmp();
	max = USHM_SIZE - 1;
	while (max > 0 && utmpshm->uinfo[max].active == 0)
		max--;
	for (i = 0; i <= max; i++)
	{
		if ((*fptr) (&(utmpshm->uinfo[i])) == QUIT)
			return QUIT;
	}
	return 0;
}

int search_ulist(struct user_info *uentp, int (*fptr)(), int farg)
{
	int     i;
	resolve_utmp();
	for (i = 0; i < USHM_SIZE; i++)
	{
		*uentp = utmpshm->uinfo[i];
		if ((*fptr) (farg, uentp))
			return i + 1;
	}
	return 0;
}

int search_ulistn(struct user_info* uentp, int (*fptr)(), int farg, int unum)
{
	int     i, j;
	j = 1;
	resolve_utmp();
	for (i = 0; i < USHM_SIZE; i++)
	{
		*uentp = utmpshm->uinfo[i];
		if ((*fptr) (farg, uentp))
		{
			if (j == unum)
				return i + 1;
			else
				j++;
		}
	}
	return 0;
}

int t_search_ulist(struct user_info* uentp, int (*fptr)(), int farg, int show, int doTalk)
{
	int     i, num;
	char col[14];
	char col2[14];
	resolve_utmp();
	num = 0;
	for (i = 0; i < USHM_SIZE; i++)
	{
		*uentp = utmpshm->uinfo[i];
		if ((*fptr) (farg, uentp))
		{
			if (!uentp->active || !uentp->pid || isreject(uentp))
				continue;
			if (  (uentp->invisible==0)||(uentp->uid == usernum)
			        ||(uentp->invisible==1)&&(HAS_PERM(PERM_SYSOP|PERM_SEECLOAK)||canseeme(uentp)))
			{
				num++;
			}
			else
			{
				continue;
			}
			if (!show)
				continue;
			/* Efan: Ôö¼Ó½±ÕÂÊý	*/

#ifdef ALLOW_MEDALS

			if (num == 1)
				prints ("Ä¿Ç° %s ×´Ì¬ÈçÏÂ:   ", uentp->userid);
#else

			if (num == 1)
				prints("Ä¿Ç° %s ×´Ì¬ÈçÏÂ: \n", uentp->userid);
#endif
			//add by bluetent 2002-10-30 begin
			tmpmode=uentp->mode;
			if (tmpmode>20000)
			{
				tmpmode-=20000;
				iswwwmode=1;
			}
			else
			{
				iswwwmode=0;
			};
			//add by bluetent 2002-10-30 end


			if ( uentp->mode>20000)
				strcpy(col2, "[1;44m");
			else
				strcpy(col2, "[1;40m");
			if ( uentp->invisible )
				strcpy(col, "[1;36m");
			else if ( tmpmode == POSTING || tmpmode == MARKET )
				strcpy(col,"[1;32m");
			else if ( tmpmode == FIVE || tmpmode == BBSNET)
				strcpy(col,"[1;33m");
			else
				strcpy(col,"[1m");
			if(doTalk)
			{
				prints("(%d) ×´Ì¬: %s%s%-10s[m, À´×Ô: %.20s[1m\n",
				       num, col, col2, ModeType(tmpmode),
				       (uentp->from[22]!='H'||hisfriend(uentp))?uentp->from:BBSIP_DOT);
			}
			else
			{
				prints("%s%s%-10s[1;37;40m ",col, col2,ModeType(tmpmode));
				if ((num) % 5 == 0)
					outc('\n');
			}
		}
	}
	if (show)
		outc('\n');
	return num;
}

void update_ulist(struct user_info* uentp, int uent)
{
	resolve_utmp();
	if (uent > 0 && uent <= USHM_SIZE)
	{
		utmpshm->uinfo[uent - 1] = *uentp;
	}
}

void update_utmp()
{
	update_ulist(&uinfo, utmpent);
}

/* added by djq 99.7.19*/
/* function added by douglas 990305
 set uentp to the user who is calling me
 solve the "one of 2 line call sb. to five" problem
*/

int who_callme( struct user_info* uentp, int(*fptr)(), int farg, int me )
{
	int         i;

	resolve_utmp();
	for( i = 0; i < USHM_SIZE; i++ )
	{
		*uentp = utmpshm->uinfo[ i ];
		if( (*fptr)( farg, uentp ) && uentp->destuid==me )
			return i+1;
	}
	return 0;
}

void *attach_shm2(char* shmstr, int defaultkey, int shmsize, int* iscreate)
{
	void *shmptr;
	int shmkey, shmid;
	shmkey = search_shmkey(shmstr);
	if (shmkey < 1024)
		shmkey = defaultkey;
	shmid = shmget(shmkey, shmsize, 0);
	if (shmid < 0)
	{
		shmid = shmget(shmkey, shmsize, IPC_CREAT | 0644);
		*iscreate = 1;
		if (shmid < 0)
			attach_err(shmkey, "shmget", errno);
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
			attach_err(shmkey, "shmat", errno);
		memset(shmptr, 0, shmsize);
	}
	else
	{
		*iscreate = 0;
		shmptr = (void *) shmat(shmid, NULL, 0);
		if (shmptr == (void *) -1)
			attach_err(shmkey, "shmat", errno);
	}
	return shmptr;
}

