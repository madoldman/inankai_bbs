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
$Id: list.c,v 1.2 2008-12-04 12:29:47 madoldman Exp $
*/

#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

#define BBS_PAGESIZE    (t_lines - 4)
#define refreshtime     (30)
extern time_t login_start_time;
extern char BoardName[];
char	annfname[STRLEN];
int     (*func_list_show) ();
time_t  update_time = 0;
int     freshmode = 0;
int     toggle1 = 0, toggle2 = 0;
int     mailmode, numf;
int     friendmode = 0;
int     usercounter, real_user_names = 0;
int     range, page, readplan, num;
int		tmpmode;//add by bluetent 2002-10-30
int		iswwwmode;//add by bluetent 2002-10-30
struct user_info *user_record[MAXACTIVE];
struct userec *user_data;
// add by Flier - 2000.5.12 - Begin
enum sort_type { stUserID, stUserName, stIP, stState } st = stUserID;
// add by Flier - 2000.5.12 - End

int	isdormip();//add by yiyo
int isfriend;//add by bluetent ÓÃÓÚÌæ´úhisfriend(uentp)£¬Ìá¸ßÒ»µãĞ§ÂÊ
#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif


void get_upper_str(char* str, char* pstr)//×ª»»³É´óĞ´×Ö·û add by bluetent
{
	strcpy(str, pstr);
	while(*str)
	{
		if(*str>='a'&&*str<='z')
			*str-=32;
		str++;
	}
}

int friend_search(unsigned short uid, struct user_info* uentp, int tblsize)
{
	int     hi, low, mid;
	int     cmp;
	if (uid == 0)
	{
		return NA;
	}
	hi = tblsize - 1;
	low = 0;
	/*
		while (low <= hi)
		{
			if (uentp->friend [low] == uid)
				return YEA;
			low ++;
		}
	*/
	while (low <= hi)
	{
		mid = (low + hi) / 2;
		cmp = uentp->friend[mid] - uid;
		if (cmp == 0)
		{
			return YEA;
		}
		if (cmp > 0)
			hi = mid - 1;
		else
			low = mid + 1;
	}
	return NA;
}

int myfriend(unsigned short uid)
{
	return friend_search(uid, &uinfo, uinfo.fnum);
}

int hisfriend(struct user_info* uentp)
{
	/* Efan: ÁÙÊ±´úÂë
		FILE *fp;
		char genbuf [256];
		struct override od;
	        sethomefile(genbuf, uentp->userid, "friends");
		fp = fopen (genbuf, "rb");
		if (!fp)
			return NA;
		while (fread (& od, sizeof (struct override), 1, fp) ==1)
		{
			if (!strcasecmp (od.id, uinfo.userid) )
				return YEA;
		}
		fclose (fp);
		return NA;
	*/
	return friend_search(uinfo.uid, uentp, uentp->fnum);
}

int isreject(struct user_info *uentp)
{
	int     i;

	if (HAS_PERM(PERM_SYSOP))
		return NA;
	if (uentp->uid != uinfo.uid)
	{
		for (i = 0;i<MAXREJECTS&&(uentp->reject[i]||uinfo.reject[i]);i++)
		{
			if (uentp->reject[i]==uinfo.uid||uentp->uid==uinfo.reject[i])
				return YEA;	/* ±»ÉèÎªºÚÃûµ¥ */
		}
	}
	return NA;
}
print_title()
{

	docmdtitle((friendmode) ? "[ºÃÅóÓÑÁĞ±í]" : "[Ê¹ÓÃÕßÁĞ±í]",
	           " ÁÄÌì[[1;32mt[m] ¼ÄĞÅ[[1;32mm[m] ËÍÑ¶Ï¢[[1;32ms[m] ¼Ó,¼õÅóÓÑ[[1;32mo[m,[1;32md[m] ¿´ËµÃ÷µµ[[1;32m¡ú[m,[1;32mRtn[m] ÇĞ»»Ä£Ê½ [[1;32mf[m] Çó¾È[[1;32mh[m]");
	update_endline();
}
print_title2()
{

	docmdtitle((friendmode) ? "[ºÃÅóÓÑÁĞ±í]" : "[Ê¹ÓÃÕßÁĞ±í]",
	           "        ¼ÄĞÅ[[1;32mm[m] ¼Ó,¼õÅóÓÑ[[1;32mo[m,[1;32md[m] ¿´ËµÃ÷µµ[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ı[m] Çó¾È[[1;32mh[m]");
	update_endline();
}

void update_data()
{
	if (readplan == YEA)
		return;
	if (time(0) >= update_time + refreshtime - 1)
	{
		freshmode = 1;
	}
	signal(SIGALRM, update_data);
	alarm(refreshtime);
	return;
}


int print_user_info_title()
{
	char    title_str[512];
	char   *field_2;

	move(2, 0);
	clrtoeol();
	field_2 = "Ê¹ÓÃÕßêÇ³Æ";
	if (real_user_names)
		field_2 = "ÕæÊµĞÕÃû  ";
	sprintf(title_str,"[1;44m ±àºÅ %sÊ¹ÓÃÕß´úºÅ%s %s%s%s%8.8s %sÉÏÕ¾µÄÎ»ÖÃ%s      P M %c%sÄ¿Ç°¶¯Ì¬%s %5s[m\n",
	        (st==stUserID)?"[32m{":" ", (st==stUserID)?"}[37m":" ",
	        (st==stUserName)?"[32m{":" ",field_2,(st==stUserName)?"}[37m":" ",
	        " ", (st==stIP)?"[32m{":" ",(st==stIP)?"}[37m":" ",
	        //      ((HAS_PERM(PERM_SYSOP|PERM_SEECLOAK|PERM_CLOAK)) ? 'C' : ' '),
	        'W',
	        (st==stState)?"[32m{":" ",(st==stState)?"}[37m":" ",
#ifdef SHOW_IDLE_TIME
	        "Ê±:·Ö");
#else
	        ""
	       );
#endif
	prints("%s", title_str);
	return 0;
}

show_message(char msg[])
{

	move(BBS_PAGESIZE + 2, 0);
	clrtoeol();
	if (msg != NULL)
		prints("[1m%s[m", msg);
	refresh();
}

void swap_user_record(int a, int b)
{
	struct user_info *c;
	c = user_record[a];
	user_record[a] = user_record[b];
	user_record[b] = c;
}
// Add by Flier - 2000.5.12 - Begin
int compare_user_record(struct user_info* left, struct user_info* right)
{
	int retCode;

	switch(st)
	{
	case stUserID:
		retCode = strcasecmp(left->userid, right->userid);
		break;
	case stUserName:
		retCode = strcasecmp(left->username, right->username);
		break;
	case stIP:
		/* Efan: ½â¾ö°´IPÅÅĞòÊ±µÄbug -- 2003.4.26	*/
		retCode = strncmp(left->from, right->from,20);
		/**/
		/*{
			int a [4], b [4];
			int i;
			char *ptr;
			char * buf [21];
			for (i =0; (i < 20 && i < strlen (left->from) && i < strlen (right->from) ); ++i)
				if ( (left->from [i] >= 'a' && left->from [i] <= 'z') ||
				(left->from [i] >= 'A' && left->from [i] <= 'Z') ||
				(right->from [i] >= 'a' && right->from [i] <= 'z') ||
				(right->from [i] >= 'A' && right->from [i] <= 'Z') )
					return strncmp (left->from, right->from, 20);
		 
			strncpy (buf, left->from, 20);
			ptr = strtok (buf, ".\0");
			for (i =0; i <4; ++i)
			{
				a [i] = atoi (ptr);
				ptr = strtok (NULL, ".\0");
			}
			strncpy (buf, right->from, 20);
			ptr = strtok (buf, ". ");
			for (i =0; i <4; ++i)
			{
				b [i] = atoi (ptr);
				ptr = strtok (NULL, ". ");
			}
			for (i =0; i <4; ++i)
				if ( (retCode = a [i] - b [i]) !=0)
					break;
		}*/
		/**/
		break;
	case stState:
		retCode = left->mode - right->mode;
		break;
	}
	return retCode;
}
// Add by Filer - 2000.5.12 - End

void sort_user_record(int left, int right)
{
	int     i, last;

	if (left >= right)
		return;
	swap_user_record(left, (left + right) / 2);
	last = left;
	for (i = left + 1; i <= right; i++)
	{
		// Modified by Flier - 2000.5.12
		if(compare_user_record(user_record[i], user_record[left])<0)
		{
			swap_user_record(++last,i);
		}
	}
	swap_user_record(left, last);
	sort_user_record(left, last - 1);
	sort_user_record(last + 1, right);
}

int fill_userlist()
{
	register int i, n, totalusernum;
	int friendno[MAXFRIENDS];
	extern struct UTMPFILE *utmpshm;
	//struct user_info *friends_online[MAXFRIENDS];

	resolve_utmp();
	totalusernum = 0;
	numf = 0;
	for (i = 0; i < USHM_SIZE; i++)
	{
		if (  !utmpshm->uinfo[i].active
		        ||!utmpshm->uinfo[i].pid||isreject(&utmpshm->uinfo[i]))
			continue;
		if (  (utmpshm->uinfo[i].invisible)
		        &&(usernum != utmpshm->uinfo[i].uid)
		        &&(!(HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)||canseeme(&utmpshm->uinfo[i]))))
			continue;
		if (myfriend(utmpshm->uinfo[i].uid))
		{
			if(numf>=MAXFRIENDS)
				goto out;
			friendno[numf++] = totalusernum;

		}
		else if( friendmode )
			continue;
out:
		user_record[totalusernum++] = &utmpshm->uinfo[i];
	}


	if (!friendmode)
	{
		for (i=0,n=0; i < totalusernum; i++)
		{
			if( n >= numf )
				break;
			if (friendno[n] == i)
			{
				if( i != n )
					swap_user_record(n, i);
				n ++;
			}
		}
		if( numf > 2 )
		{
			sort_user_record(0, numf - 1);
		}
		else if( numf == 2 )
		{
			if(compare_user_record(user_record[0], user_record[1])>0)
				swap_user_record(0,1);
		}
		sort_user_record(numf, totalusernum - 1);
	}
	else
	{
		if( totalusernum > 2 )
		{
			sort_user_record(0, totalusernum - 1);
		}
		else if( totalusernum == 2 )
		{
			if(compare_user_record(user_record[0], user_record[1])>0)
				swap_user_record(0,1);
		}
	}
	range = totalusernum;
	return totalusernum == 0 ? -1 : 1;
}

int do_userlist()
{
	int 	   i, j, override;
	int nchar;
	char showglo [20];
	char    user_info_str[STRLEN * 2];
	struct  user_info *uentp;
	char    ipshow[60];
	char username[20];
	move(3, 0);
	print_user_info_title();
	for (i = 0, j = 0; j < BBS_PAGESIZE && i + page < range; i++)
	{
		uentp = user_record[i + page];
		override = (i + page < numf) || friendmode;
		if (readplan == YEA)
			return 0;
		if ( uentp == NULL || !uentp->active || !uentp->pid )
		{ //by sunner
			continue;	/* Ä³ÈËÕıÇÉÀë¿ª */
		}
		/********************* ÊÇ·ñ¹«¿ªºÃÓÑÉèÖÃÑ¡Ïî modified by silencer 02.6.8****************/

		if (uentp != NULL)
		{  // by wujian Ôö¼ÓÊÇ·ñÎª¶Ô·½ºÃÓÑµÄÏÔÊ¾
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
			/*
			ÒÔÉÏÕâ¶Î´úÂëÓÃÀ´ÅĞ¶Ï¸ÃidÊÇ·ñÔÚWWW·½Ê½ÏÂä¯ÀÀ£¬ÔÚWWW·½Ê½ÏÂmodeÖµ¶¼±»¶à¼ÓÉÏÁË
			20000£¬Èç¹û´óÓÚ20000±íÃ÷ÎªWWW·½Ê½£¬²¢½«20000¼õÈ¥£¬²¢¸³iswwwmodeÎª1¡£Òò
			´ËtmpmodeÎªÄ¿Ç°ÕæÊµµÄmode£¬ÇëĞŞ¸ÄÒÔÏÂ´úÂëÊ±×¢Òâ£¬²»ÒªÊ¹ÓÃuentp->mode¡£
			*/
			strncpy(username, uentp->username, 20);
			/*
				if (username[19]==0xA1||(username[19]>=0xB0&&username[19]<=0xF3))
				{
					username[19]=" ";
				}
			*/
			/* Efan: ÓÃºêÀ´¿ØÖÆÊÇ·ñÒş²ØIP¿ª¹Ø	*/
#ifdef DEF_HIDE_IP_ENABLE

			if ((uentp->from[22]=='H'||tmpmode==10001)&&!HAS_PERM(PERM_SEEUSERIP)&&isdormip(uentp->from))
				strcpy(ipshow,"ÄÏ¿ªËŞÉáÄÚÍø");
			else
				strcpy(ipshow,uentp->from);
#else

			strcpy (ipshow, uentp->from);
#endif

			/* Efan: ¼ÙÈçêÇ³ÆÔÚ20¸ö×Ö·ûÎ»Ö®ÄÚ£¬»á³öÏÖ°ë¸öºº×Ö£¬ÔòÖ»ÏÔÊ¾Ç°19¸ö	*/
			nchar = 20;
			if (!real_user_names)
			{
				int tmplen;
				/*
						for (tmplen =0; tmplen < 20; ++tmplen)
							if (username [tmplen] == 0) break;
						if (tmplen == 20)
						{
							int tmpi;
							tmpi =0;
							for (tmplen =0; tmplen < 20; ++tmplen)
								if (tmpi) tmpi =0;
								else if ((unsigned char)username [tmplen] >= 128) tmpi =1;
							if (tmpi) nchar = 19;
						}
				*/
				int tmpi =0;
				for (tmplen =0; tmplen < 20 && username [tmplen] != 0; ++tmplen)
					if (tmpi)
						tmpi =0;
					else if ((unsigned char) username [tmplen] >= 128)
						tmpi =1;
				if (tmpi)
					username [tmplen -1] = '?';
			}
			isfriend=hisfriend(uentp);
			/* Efan: Èç¹ûÏµÍ³Ã÷ÏÔ±äÂı£¬Çë°ÑÕâ¶Î´úÂëÈ¥µô	*/
			/*
				getuser (uentp->userid);
				if (lookupuser.nummedals > 3)
					strcpy (showglo, (override)?((isfriend)?"\33[31m¡ï\33[32m" : "\33[36m¡ï\33[32m") : ((isfriend)?"\33[33m¡ï" : "\33[31m¡ï\33[m"));
				else if (lookupuser.nummedals >0)
					strcpy (showglo, (override)?((isfriend)?"\33[31m¡î\33[32m" : "\33[36m¡î\33[32m") : ((isfriend)?"\33[33m¡î" : "\33[31m¡î\33[m"));
				else strcpy (showglo, (override)?((isfriend)?"\33[32m¡Ì":"¡Ì\33[32m") : ((isfriend)?"\33[33m  " : "  "));
			*/

                        if(override){
                             if(myfriend(uentp->uid))
			          strcpy(showglo, isfriend?"\33[32m¡Ì":"¡Ì\33[32m");
                             else continue;
                        }
		        else strcpy(showglo, ((isfriend)?"\33[33m  " : "  "));

			/*
				if (override)
				{
					if (isfriend)
						sprintf (showglo, "\33[32m%s", glo);
					else sprintf (showglo, "%s\33[32m", glo);
				}
				else {
					if (isfriend)
						sprintf (showglo, "\33[33m%s", glo);
					else sprintf (showglo, "%s", glo);
				}
			*/

			/* Efan: end	*/
			if ( HAS_DEFINE(lookupuser.userdefine, DEF_FRIEND_INFO)	)
	{
				sprintf(user_info_str," [m%4d%2s%-12.22s [37m%-20.*s [37m%-16.16s %c %c %c %s%-10.10s [37m%4.4s[m\n", i + 1 + page,
				        /*
				        	    (override) ? ((isfriend)? "[32m¡Ì":"¡Ì[32m"):((isfriend)?"[33m  ":"  "), 
				        	    */
				        showglo,
				        uentp->userid, nchar, (real_user_names)?uentp->realname:username,		ipshow,
				        (tmpmode == FIVE || tmpmode == BBSNET)?'@':pagerchar(isfriend,uentp->pager),msgchar(uentp),
				        (iswwwmode)?'@':' ',
				        (uentp->invisible == YEA)?"[36m":(tmpmode==POSTING||tmpmode==MARKET)?"[32m":((tmpmode==FIVE||tmpmode==BBSNET||tmpmode==10001)?"[33m":""),
				        ModeType(tmpmode),
#ifdef SHOW_IDLE_TIME
				        idle_str(uentp));
#else
				        ""
				       );
#endif

			}
			else
			{
				sprintf(user_info_str," [m%4d%2s%-12.12s [37m%-20.*s [37m%-16.16s %c %c %c %s%-10.10s [37m%4.4s[m\n", i + 1 + page,
				        /*
				          (override) ? "[32m¡Ì":"  ", 
				          */
				        showglo,
				        uentp->userid, nchar, (real_user_names)?uentp->realname:username,ipshow,
				        (tmpmode == FIVE || tmpmode == BBSNET)?'@':pagerchar(isfriend,uentp->pager),msgchar(uentp),
				        (iswwwmode)?'@':' ',
				        (uentp->invisible == YEA)?"[36m":(tmpmode==POSTING||tmpmode==MARKET)?"[32m":((tmpmode==FIVE||tmpmode==BBSNET||tmpmode==10001)?"[33m":""),
				        ModeType(tmpmode),
#ifdef SHOW_IDLE_TIME
				        idle_str(uentp));
#else
				        ""
				       );
#endif

			}
			clrtoeol();
			prints("%s", user_info_str);
			j++;
		}
	}
	return 0;
}

int show_userlist()
{
	if (update_time + refreshtime < time(0))
	{
		fill_userlist();
		update_time = time(0);
	}
	if (range == 0 /* ||fill_userlist() == 0 */ )
	{
		move(2, 0);
		prints("Ã»ÓĞÊ¹ÓÃÕß£¨ÅóÓÑ£©ÔÚÁĞ±íÖĞ...\n");
		clrtobot();
		if (friendmode)
		{
			move(BBS_PAGESIZE + 3, 0);
			if (askyn("ÊÇ·ñ×ª»»³ÉÊ¹ÓÃÕßÄ£Ê½", YEA, NA) == YEA)
			{
				range = num_visible_users();
				page = -1;
				friendmode = NA;
				return 1;
			}
		}
		else
			pressanykey();
		return -1;
	}
	do_userlist();
	clrtobot();
	return 1;
}

int deal_key(char ch, int allnum, int pagenum) //»·¹ËËÄ·½´¦Àí°´¼ü
{
	char    buf[STRLEN], desc[5];
	static int msgflag;
	extern int friendflag;

	if (msgflag == YEA)
	{
		show_message(NULL);
		msgflag = NA;
	}
	switch (ch)
	{
	case '/'://down search id. added by bluetent 2002.11.7
		num=UseronlineSearch(num,1);
		break;
	case '?'://up search id. added by bluetent 2002.11.7
		num=UseronlineSearch(num,-1);
		break;
	case 'Y':
		if(HAS_PERM(PERM_CLOAK))
			x_cloak();
		break;
	case 'C':
	case 'c':
		if ( !strcmp(currentuser.userid,"guest") )
			return 1;
		else
		{
			if(ch == 'C' )
			{
				strcpy(genbuf,"±ä»»êÇ³Æ(²»ÊÇÁÙÊ±±ä»»)Îª: ");
			}
			else
			{
				strcpy(genbuf,"ÔİÊ±±ä»»êÇ³Æ(×î¶à10¸öºº×Ö): ");
			}
			strcpy(buf, "");
			getdata(BBS_PAGESIZE+3,0,genbuf,buf,(ch=='C')?NAMELEN:21,DOECHO,NA);
			if(buf[0] != '\0')
			{
				strcpy(uinfo.username, buf);
				if(ch == 'C')
				{
					set_safe_record();
					strcpy(currentuser.username,buf);
					substitute_record(PASSFILE, &currentuser,sizeof(currentuser),usernum);
				}
			}
			break;
		}
	case 'k':
	case 'K':
		if (!HAS_PERM(PERM_SYSOP) && (usernum!=user_record[allnum]->uid) &&
		        !HAS_PERM(PERM_ACCOUNTS) && !HAS_PERM(PERM_OBOARDS))
			return 1;
		if ( !strcmp(currentuser.userid,"guest") )
			return 1;
		if( user_record[allnum]->pid == uinfo.pid)
			strcpy(buf, "Äã×Ô¼ºÒª°Ñ¡¾×Ô¼º¡¿Ìß³öÈ¥Âğ");
		else
			sprintf(buf, "ÄãÒª°Ñ %s Ìß³öÕ¾ÍâÂğ", user_record[allnum]->userid);
		move(BBS_PAGESIZE + 2, 0);
		if (askyn(buf, NA, NA) == NA)
			break;
		if (kick_user(user_record[allnum]) == 1)
		{
			sprintf(buf, "%s ÒÑ±»Ìß³öÕ¾Íâ", user_record[allnum]->userid);
		}
		else
		{
			sprintf(buf, "%s ÎŞ·¨Ìß³öÕ¾Íâ", user_record[allnum]->userid);
		}
		user_record[allnum]->active = NA;
		update_time = 0;
		msgflag = YEA;
		break;
	case 'h':
	case 'H':
		show_help("help/userlisthelp");
		break;
	case 't':
	case 'T':
		if (!HAS_PERM(PERM_PAGE))
			return 1;
		if (user_record[allnum]->uid!=usernum)
			ttt_talk(user_record[allnum]);
		else
			return 1;
		break;
	case 'v':
		//      case 'V':
		if( !HAS_PERM(PERM_SYSOP))
			return 1;
		real_user_names = !real_user_names;
		break;
	case 'm':
	case 'M':
		if (!HAS_PERM(PERM_POST))
			return 1;
		m_send(user_record[allnum]->userid);
		break;
	case 'g':
	case 'G':
		if(!HAS_PERM(PERM_POST))
			return 1;
		sendGoodWish(user_record[allnum]->userid);
		break;
	case 'f':
	case 'F':
		if (friendmode)
			friendmode = NA;
		else
			friendmode = YEA;
		update_time = 0;
		break;
	case 'x':
	case 'X':
		if(!Personal(user_record[allnum]->userid))
		{
			sprintf(buf,"%s »¹Ã»ÓĞÉêÇë¸öÈËÎÄ¼¯", user_record[allnum]->userid);
			msgflag = YEA;
		}
		break;
	case Ctrl('K'):
					y_lockscreen();
		break; //add by yiyo ¿ìËÙÆÁÄ»Ëø¶¨
	case 's':
	case 'S':
		if (!strcmp("guest", currentuser.userid))
			return 0;
		if (!HAS_PERM(PERM_MESSAGE))
			return 1;
		if (!canmsg(user_record[allnum]))
{
			sprintf(buf, "%s ÒÑ¾­¹Ø±ÕÑ¶Ï¢ºô½ĞÆ÷", user_record[allnum]->userid);
			msgflag = YEA;
			break;
		}
		do_sendmsg(user_record[allnum], NULL, 0, user_record[allnum]->pid);
		break;
	case 'o':
	case 'O':
	case 'r':
	case 'R':
		if (!strcmp("guest", currentuser.userid))
			return 0;
		if (ch == 'o' || ch == 'O')
		{
			friendflag = YEA;
			strcpy(desc, "ºÃÓÑ");
		}
		else
		{
			friendflag = NA;
			strcpy(desc, "»µÈË");
		}
		sprintf(buf,"È·¶¨Òª°Ñ %s ¼ÓÈë%sÃûµ¥Âğ",
		        user_record[allnum]->userid, desc);
		move(BBS_PAGESIZE + 2, 0);
		if (askyn(buf, NA, NA) == NA)
			break;
		if (addtooverride(user_record[allnum]->userid) == -1)
		{
			sprintf(buf, "%s ÒÑÔÚ%sÃûµ¥", user_record[allnum]->userid, desc);
		}
		else
		{
			sprintf(buf, "%s ÁĞÈë%sÃûµ¥", user_record[allnum]->userid, desc);
		}
		msgflag = YEA;
		break;
		//      case 'P':
		//         t_pager();
		break;
	case 'd':
	case 'D':
		if (!strcmp("guest", currentuser.userid))
			return 0;
		sprintf(buf,"È·¶¨Òª°Ñ %s ´ÓºÃÓÑÃûµ¥É¾³ıÂğ",
		        user_record[allnum]->userid);
		move(BBS_PAGESIZE + 2, 0);
		if (askyn(buf, NA, NA) == NA)
			break;
		if (deleteoverride(user_record[allnum]->userid, "friends") == -1)
		{
			sprintf(buf, "%s ±¾À´¾Í²»ÔÚÅóÓÑÃûµ¥ÖĞ",user_record[allnum]->userid);
		}
		else
		{
			sprintf(buf, "%s ÒÑ´ÓÅóÓÑÃûµ¥ÒÆ³ı", user_record[allnum]->userid);
		}
		msgflag = YEA;
		break;
	default:
		return 0;
	}
	if (friendmode)
		modify_user_mode(FRIEND);
	else
		modify_user_mode(LUSERS);
	if (readplan == NA)
	{
		print_title();
		clrtobot();
		if (show_userlist() == -1)
			return -1;
		if (msgflag)
			show_message(buf);
		update_endline();
	}
	return 1;
}

int deal_key2(char ch, int allnum, int pagenum) //Ì½ÊÓÍøÓÑ´¦Àí°´¼ü
{
	char    buf[STRLEN];
	static int msgflag;

	if (msgflag == YEA)
	{
		show_message(NULL);
		msgflag = NA;
	}
	switch (ch)
	{
	case 'h':
	case 'H':
		show_help("help/usershelp");
		break;
	case 'm':
	case 'M':
		if (!HAS_PERM(PERM_POST))
			return 1;
		m_send(user_data[allnum - pagenum].userid);
		break;
	case 'o':
	case 'O':
		if (!strcmp("guest", currentuser.userid))
			return 0;
		sprintf(buf, "È·¶¨Òª°Ñ %s ¼ÓÈëºÃÓÑÃûµ¥Âğ",
		        user_data[allnum - pagenum].userid);
		move(BBS_PAGESIZE + 2, 0);
		if (askyn(buf, NA, NA) == NA)
			break;
		if (addtooverride(user_data[allnum - pagenum].userid) == -1)
		{
			sprintf(buf,"%s ÒÑÔÚÅóÓÑÃûµ¥",user_data[allnum-pagenum].userid);
			show_message(buf);
		}
		else
		{
			sprintf(buf, "%s ÁĞÈëÅóÓÑÃûµ¥", user_data[allnum - pagenum].userid);
			show_message(buf);
		}
		msgflag = YEA;
		if (!friendmode)
			return 1;
		break;
	case 'f':
	case 'F':
		toggle1++;
		if (toggle1 >= 3)
			toggle1 = 0;
		break;
	case 't':
	case 'T':
		toggle2++;
#ifdef ALLOWGAME

		if (toggle2 >= 3)
			toggle2 = 0;
#else

		if (toggle2 >= 2)
			toggle2 = 0;
#endif

		break;
	case 'd':
	case 'D':
		if (!strcmp("guest", currentuser.userid))
			return 0;
		sprintf(buf, "È·¶¨Òª°Ñ %s ´ÓºÃÓÑÃûµ¥É¾³ıÂğ",
		        user_data[allnum - pagenum].userid);
		move(BBS_PAGESIZE + 2, 0);
		if (askyn(buf, NA, NA) == NA)
			break;
		if (deleteoverride(user_data[allnum-pagenum].userid,"friends")==-1)
		{
			sprintf(buf, "%s ±¾À´¾Í²»ÔÚÅóÓÑÃûµ¥ÖĞ",
			        user_data[allnum - pagenum].userid);
			show_message(buf);
		}
		else
		{
			sprintf(buf, "%s ÒÑ´ÓÅóÓÑÃûµ¥ÒÆ³ı",
			        user_data[allnum - pagenum].userid);
			show_message(buf);
		}
		msgflag = YEA;
		if (!friendmode)
			return 1;
		break;
	default:
		return 0;
	}
	modify_user_mode(LAUSERS);
	if (readplan == NA)
	{
		print_title2();
		move(3, 0);
		clrtobot();
		if (Show_Users() == -1)
			return -1;
		update_endline();
	}
	redoscr();
	return 1;
}

int countusers(struct userec* uentp)
{
	static int totalusers;
	char    permstr[11];
	if (uentp == NULL)
	{
		int     c = totalusers;
		totalusers = 0;
		return c;
	}
	if (uentp->numlogins != 0 && uleveltochar(permstr, uentp->userlevel) != 0)
		totalusers++;
	return 0;
}
printuent(struct userec* uentp)
{
	static int i;
	char    permstr[11];
	//char	glo [3];
	char    msgstr[18];
	int     override;
	if (uentp == NULL)
	{
		printutitle();
		i = 0;
		return 0;
	}
	if (uentp->numlogins == 0 ||
	        uleveltochar(permstr, uentp->userlevel) == 0)
		return 0;
	if (i < page || i >= page + BBS_PAGESIZE || i >= range)
	{
		i++;
		if (i >= page + BBS_PAGESIZE || i >= range)
			return QUIT;
		else
			return 0;
	}
	uleveltochar(&permstr, uentp->userlevel);
	switch (toggle1)
	{
	case 0:
		getdatestring(uentp->lastlogin,NA);
		sprintf(msgstr, "%-.16s", datestring+6);
		break;
	case 1:
		if (HAS_PERM(PERM_SEEUSERIP))
			sprintf(msgstr, "%-.16s", uentp->lasthost);
		else
			sprintf(msgstr, "%-.16s",BBSIP_DOT);
		break;
	case 2:
	default:
		getdatestring(uentp->firstlogin,NA);
		sprintf(msgstr, "%-.14s",
		        datestring );
		break;
	}
	user_data[i - page] = *uentp;
	override = myfriend(searchuser(uentp->userid));
	/*
		if (uentp->nummedals > 6)
			strcpy (glo, "¡ï");
		else if (uentp->nummedals >0)
			strcpy (glo, "¡ø");
		else strcpy (glo, (override)?"¡Ì":"  ");
		if (override)
		{
			if (isfriend)
				sprintf (showglo, "\33[32m%s", glo);
			else sprintf (showglo, "%s\33[32m", glo);
		}
		else {
			if (isfriend)
				sprintf (showglo, "\33[33m%s", glo);
			else sprintf (showglo, "%s", glo);
		}
		*/
	prints(" %5d%2s%s%-12s%s %-17s %6d %4d %10s %-16s\n", i + 1,
	       (override) ? "¡Ì" : "",
	       /*
	       		glo,
	       */
	       (override) ? "[1;32m" : "", uentp->userid, (override) ? "[m" : "",
#if defined(ACTS_REALNAMES)
	       HAS_PERM(PERM_SYSOP) ? uentp->realname : uentp->username,
#else
	       uentp->username,
#endif
#ifdef ALLOWGAME
	       (toggle2 == 0) ? (uentp->numlogins) : (toggle2 == 1) ? (uentp->numposts) : (uentp->money),
	       (toggle2 == 0) ? uentp->stay / 3600 : (toggle2 == 1) ? (uentp->nummedals) : uentp->nummails,
#else
	       (toggle2 == 0) ? (uentp->numlogins) : (uentp->numposts),
	       (toggle2 == 0) ? uentp->stay / 3600 : uentp->nummails,
#endif
	       HAS_PERM(PERM_SEEULEVELS) ? permstr : "", msgstr);
	i++;
	usercounter++;
	return 0;
}

int allusers()
{
	countusers(NULL);
	if (apply_record(PASSFILE, countusers, sizeof(struct userec)) == -1)
	{
		return 0;
	}
	return countusers(NULL);
}

int mailto(struct userec *uentp)
{
	char    filename[STRLEN];
	sprintf(filename, "tmp/mailall.%s", currentuser.userid);
	if ((uentp->userlevel == PERM_BASIC && mailmode == 1) ||
	        (uentp->userlevel & PERM_POST && mailmode == 2) ||
	        (uentp->userlevel & PERM_BOARDS && mailmode == 3) ||
	        (uentp->userlevel & PERM_CHATCLOAK && mailmode == 4)||
		(uentp->userlevel & PERM_CLOAK && mailmode == 5)||
		(uentp->userlevel & PERM_SEECLOAK && mailmode == 6)||
		(uentp->userlevel & PERM_XEMPT && mailmode == 7)||
		(mailmode==8))
	{
		//mail_file(filename, uentp->userid, save_title);
		mail_announce(uentp->userid, save_title,annfname);
	}
	return 1;
}

mailtoall(int mode)
{
        char    filename[STRLEN],filepath[STRLEN];
        sprintf(filename, "tmp/mailall.%s", currentuser.userid);
        mailmode = mode;
        sprintf(annfname, "M.%d.A", time(NULL));

        sprintf(filepath, "%s/%s",ANNOUNCE_MAIL_DIR,annfname);

        f_cp(filename, filepath, O_CREAT);
        if (apply_record(PASSFILE, mailto, sizeof(struct userec)) == -1)
        {
                prints("No Users Exist");
                pressreturn();
                return 0;
        }
        return;
}

Show_Users()
{

	usercounter = 0;
	modify_user_mode(LAUSERS);
	printuent((struct userec *) NULL);
	if (apply_record(PASSFILE, printuent, sizeof(struct userec)) == -1)
	{
		prints("No Users Exist");
		pressreturn();
		return 0;
	}
	clrtobot();
	return 0;
}

setlistrange(int i)
{
	range = i;
}

/*bluetent*/



UseronlineSearch(int curr_num,int offset)
{
	static char method[2],queryID[IDLEN+2],queryIP[20],queryNick[NAMELEN+2];
	char ans[STRLEN+1], pmt[STRLEN];
	strcpy(ans, method);
	get_upper_str(ans,ans);
	sprintf(pmt,"ÄúÒªÒÀÄÄÏîÌõ¼ş%s²éÕÒÓÃ»§:(A)ID(B)êÇ³Æ(C)IP[%s]:", offset>0?"ÏòÏÂ":"ÏòÉÏ", ans);
	move(t_lines - 1, 0);
	clrtoeol();
	getdata(t_lines - 1, 0,pmt, ans, 2, DOECHO, YEA);
	if(!(ans[0]>='A'&&ans[0]<='C'||ans[0]>='a'&&ans[0]<='c'||ans[0]=='\0'))
		return curr_num;
	if(ans[0]!='\0')
		strcpy(method, ans);
	switch(method[0])
	{
	case 'A':
	case 'a':
		strcpy(ans, queryID);
		sprintf(pmt, "%s²éÕÒID [%s]: ", offset > 0 ? "ÏòÏÂ"
		        : "ÏòÉÏ",ans);
		move(t_lines - 1, 0);
		clrtoeol();
		getdata(t_lines - 1, 0, pmt, ans, IDLEN+1, DOECHO, YEA);
		if (ans[0] != '\0')
			strcpy(queryID, ans);
		return IDSearch(queryID,curr_num,offset);
	case 'B':
	case 'b':
		strcpy(ans, queryNick);
		sprintf(pmt, "%s²éÕÒêÇ³Æ[%s]: ", offset > 0 ? "ÏòÏÂ"
		        : "ÏòÉÏ",ans);
		move(t_lines - 1, 0);
		clrtoeol();
		getdata(t_lines - 1, 0, pmt, ans, NAMELEN+1, DOECHO, YEA
		       );
		if (ans[0] != '\0')
			strcpy(queryNick, ans);
		return NickSearch(queryNick,curr_num,offset);
	case 'C':
	case 'c':
		strcpy(ans, queryIP);
		sprintf(pmt, "%s²éÕÒIP[%s]: ", offset > 0 ? "ÏòÏÂ"
		        : "ÏòÉÏ",ans);
		move(t_lines - 1, 0);
		clrtoeol();
		getdata(t_lines - 1, 0, pmt, ans, 17, DOECHO, YEA);
		if (ans[0] != '\0')
			strcpy(queryIP, ans);

		return IPSearch(queryIP,curr_num,offset);
	default :
		return curr_num;
	}
}
int IDSearch(char query[STRLEN], int curr_num, int offset)
{
	char upper_ID[IDLEN+2], upper_query[IDLEN+2];
	int i;
	get_upper_str(upper_query, query);
	if(query[0]=='\0')
		return curr_num;
	if(offset>0)
	{
		for (i=curr_num+1;i<range;i++)
		{
			get_upper_str(upper_ID, user_record[i]->userid);
			if(!strncmp(upper_ID,upper_query,strlen(query)))
				return i;
		}
	}
	else if(offset<0)
	{
		for (i=curr_num-1;i>0;i--)
		{
			get_upper_str(upper_ID, user_record[i]->userid);
			if(!strncmp(upper_ID,upper_query,strlen(query)))
				return i;
		}
	}
	return curr_num;
}
int IPSearch(char query[20], int curr_num, int offset)
{
	int i;
	if(query[0]=='\0')
		return curr_num;
	if(offset>0)
	{
		for (i=curr_num+1;i<range;i++)
		{
			//yanglc 20021102
			if(!strncmp(user_record[i]->from,query,strlen(query))&&!(!HAS_PERM(PERM_SEEUSERIP)&&user_record[i]->from[22]=='H'))
				return i;
		}
	}
	else if(offset<0)
	{
		for (i=curr_num-1;i>0;i--)
		{
			//yanglc 20021102
			if(!strncmp(user_record[i]->from,query,strlen(query))&&!(!HAS_PERM(PERM_SEEUSERIP)&&user_record[i]->from[22]=='H'))
				return i;
		}
	}
	return curr_num;
}

int NickSearch(char query[STRLEN], int curr_num, int offset)
{
	int i;
	if (query[0]=='\0')
		return curr_num;
	if(offset>0)
	{
		for (i=curr_num+1;i<range;i++)
		{
			if(!strncmp(user_record[i]->username,query,strlen(query)
			           ))
				return i;
		}
	}
	else if(offset<0)
	{
		for (i=curr_num-1;i>0;i--)
		{
			if(!strncmp(user_record[i]->username,query,strlen(query)
			           ))
				return i;
		}
	}
	return curr_num;
}




/*bluetent*/
do_query(int star, int curr)
{
	if (user_record[curr]->userid != NULL)
	{
		clear();
		t_query(user_record[curr]->userid);
		move(t_lines - 1, 0);
		prints("[0;1;37;44mÁÄÌì[[1;32mt[37m] ¼ÄĞÅ[[1;32mm[37m] ËÍÑ¶Ï¢[[1;32ms[37m] ¼Ó,¼õÅóÓÑ[[1;32mo[37m,[1;32md[37m] Ñ¡ÔñÊ¹ÓÃÕß[[1;32m¡ü[37m,[1;32m¡ı[37m] ÇĞ»»Ä£Ê½ [[1;32mf[37m] Çó¾È[[1;32mh[37m][m");
	}
}
do_query2(int star, int curr)
{
	if (user_data[curr - star].userid != NULL)
	{
		t_query(user_data[curr - star].userid);
		move(t_lines - 1, 0);
		prints("[0;1;37;44m          ¼ÄĞÅ[[1;32mm[37m] ¼Ó,¼õÅóÓÑ[[1;32mo[37m,[1;32md[37m] ¿´ËµÃ÷µµ[[1;32m¡ú[37m,[1;32mRtn[37m] Ñ¡Ôñ[[1;32m¡ü[37m,[1;32m¡ı[37m] Çó¾È[[1;32mh[37m]          [m");
	}
}


/* ±¾Õ¾Ê¹ÓÃÕßÁĞ±í*/
Users()
{
	range = allusers();
	modify_user_mode(LAUSERS);
	clear();
	user_data = (struct userec *) calloc(sizeof(struct userec), BBS_PAGESIZE);
	choose(NA, 0, print_title2, deal_key2, Show_Users, do_query2);
	clear();
	free(user_data);
	return;
}


int t_friends()
{
	char    buf[STRLEN];

	modify_user_mode(FRIEND);
	real_user_names = 0;
	friendmode = YEA;
	setuserfile(buf, "friends");
	if (!dashf(buf))
	{
		move(1, 0);
		clrtobot();
		prints("ÄãÉĞÎ´ÀûÓÃ Info -> Override Éè¶¨ºÃÓÑÃûµ¥£¬ËùÒÔ...\n");
		range = 0;
	}
	else
	{
		num_alcounter();
		range = count_friends;
	}
	if (range == 0)
	{
		move(2, 0);
		clrtobot();
		prints("Ä¿Ç°ÎŞºÃÓÑÉÏÏß\n");
		move(BBS_PAGESIZE + 3, 0);
		if (askyn("ÊÇ·ñ×ª»»³ÉÊ¹ÓÃÕßÄ£Ê½", YEA, NA) == YEA)
		{

			range = num_visible_users();
			freshmode = 1;
			page = -1;
			friendmode = NA;
			update_time = 0;
			choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
			clear();
			return MODECHANGED;
		}
	}
	else
	{
		update_time = 0;
		choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
	}
	clear();
	friendmode = NA;
	return MODECHANGED;
}

/*»·¹ËËÄ·½*/
int t_users()
{
	friendmode = NA;
	modify_user_mode(LUSERS);
	real_user_names = 0;
	range = num_visible_users();
	if (range == 0)
	{
		move(3, 0);
		clrtobot();
		prints("Ä¿Ç°ÎŞÊ¹ÓÃÕßÉÏÏß\n");
	}
	update_time = 0;
	choose(YEA, 0, print_title, deal_key, show_userlist, do_query);
	clear();
	return 0;
}

int choose(int update, int defaultn, int (*title_show)(), 
	int (*key_deal)(), int (*list_show)(), int (*read)())
{
	int     ch, number, deal;
	int		savemode;
	readplan = NA;
	(*title_show) ();
	func_list_show = list_show;
	signal(SIGALRM, SIG_IGN);
	if (update == 1)
		update_data();
	page = -1;
	number = 0;
	num = defaultn;
	while (1)
	{
		if (num <= 0)
			num = 0;
		if (num >= range)
			num = range - 1;
		if (page < 0 || freshmode == 1)
		{
			freshmode = 0;
			page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
			move(3, 0);
			clrtobot();
			if ((*list_show) () == -1)
				return -1;
			update_endline();
		}
		if (num < page || num >= page + BBS_PAGESIZE)
		{
			page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
			if ((*list_show) () == -1)
				return -1;
			update_endline();
			continue;
		}
		if (readplan == YEA)
		{
			if ((*read) (page, num) == -1)
				return num;
			if (uinfo.mode == QMEDAL) {
				modify_user_mode (savemode);
				if ((*read) (page, num) == -1)
					return num;
			}
		}
		else
		{
			move(3 + num - page, 0);
			prints(">", number);
		}
		ch = egetch();
		if (readplan == NA)
			move(3 + num - page, 0);
		prints(" ");
		if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
		{
			if (readplan == YEA)
			{
				readplan = NA;
				move(1, 0);
				clrtobot();
				if ((*list_show) () == -1)
					return -1;
				(*title_show) ();
				continue;
			}
			break;
		}
		deal = (*key_deal) (ch, num, page);
		if (range == 0)
			break;
		if (deal == 1)
			continue;
		else if (deal == -1)
			break;
		switch (ch)
		{
			//		case 'b':
		case 'P':
		case KEY_PGUP:
		case Ctrl('B'):
						if (num == 0)
							num = range - 1;
				else
					num -= BBS_PAGESIZE;
			break;
		case ' ':
			if (readplan == YEA)
	{
				if (++num >= range)
					num = 0;
				break;
			}
		case 'N':
		case Ctrl('F'):
					case KEY_PGDN:
							if (num == range - 1)
								num = 0;
					else
						num += BBS_PAGESIZE;
			break;
		case 'p':
		case 'l':
		case KEY_UP:
			if (num-- <= 0)
				num = range - 1;
			break;
		case 'n':
		case 'j':
		case KEY_DOWN:
			if (++num >= range)
				num = 0;
			break;
		case KEY_TAB:
			if(st!=stState)
				st++;
			else
				st=stUserID;
			//if(!HAS_PERM(PERM_SEEUSERIP)&&(st==stIP))st++;//add by bluetent 2002.11.1
			fill_userlist();
			freshmode=1;
			break;
		case '$':
		case KEY_END:
			num = range - 1;
			break;
		case KEY_HOME:
			num = 0;
			break;
		case '\n':
		case '\r':
			if (number > 0)
			{
				num = number - 1;
				break;
			}
			/* fall through */
		case KEY_RIGHT:
			{
				if (readplan == YEA)
				{
					if (++num >= range)
						num = 0;
				}
				else {
					readplan = YEA;
				}
				break;
			}
		case 'L'://bluetent
			//   char bname[STRLEN]=currboard,bpath[STRLEN];
			savemode = uinfo.mode;
			m_read();
			modify_user_mode(savemode);
			print_title();
			fill_userlist();
			freshmode=1;
			break;
		case 'V'://bluetent
			savemode = uinfo.mode;
			m_new();
			modify_user_mode(savemode);
			print_title();
			fill_userlist();
			freshmode=1;
			break;
		case 'u':
			savemode = uinfo.mode;
			modify_user_mode(QUERY);
			t_query();
			modify_user_mode(savemode);
			print_title();
			fill_userlist();
			freshmode=1;
			break;
		case '.':
			userlist_count();
			print_title();
			fill_userlist();
			freshmode=1;
			break;
		case 'i':	// Efan: ²é¿´½±ÕÂ
			if (readplan == YEA) {
				savemode = uinfo.mode;
				modify_user_mode (QMEDAL);
			}
		default:
			;
		}
		if (ch >= '0' && ch <= '9')
		{
			number = number * 10 + (ch - '0');
			ch = '\0';
		}
		else
		{
			number = 0;
		}
	}
	signal(SIGALRM, SIG_IGN);
	return -1;
}
int userlist_count()
{
	int i;
	int total=0, www=0, guest=0, wwwguest=0;
	int MODEREADING=0, MODEDIGEST=0, MODEBBSNET=0, MODEPOSTING=0, MODECHAT=0, MODEUSER=0, MODEMAIL=0, MODELOCKSCREEN=0, MODEELSE=0;
	int dorm=0, in=0, out=0;
	char buf[1024];
	extern struct UTMPFILE *utmpshm;
	struct user_info *x;
	for(i=0; i<MAXACTIVE; i++)
	{
		x=&(utmpshm->uinfo[i]);
		if(x->active==0)
			continue;
		total++;
		if(x->mode>20000)
			www++;
		if(!strcmp(x->userid, "guest"))
			guest++;
		if(!strcmp(x->userid, "guest")&&x->mode>20000)
			wwwguest++;

		if(strstr(x->from, "10.10."))
			dorm++;
		else if((!strncmp(x->from, "202.113.", 8)&&((atoi(x->from+8)>=16 && atoi(x->from+8)<=31)||(atoi(x->from+8)>=224 &&atoi(x->from+8)<=239)))||(!strncmp(x->from, "202.113.", 8)&&((atoi(x->from+7)>=16 && atoi(x->from+7)<=31)||(atoi(x->from+7)>=224 && atoi(x->from+7)<=239))))
			in++;
		else
			out++;

		if(x->mode==READING||x->mode==READING+20000
		        ||x->mode==READNEW||x->mode==READNEW+20000)
		{
			MODEREADING++;
			continue;
		}
		if(x->mode==DIGEST||x->mode==DIGEST+20000
		        ||x->mode==EDITANN||x->mode==EDITANN+20000)
		{
			MODEDIGEST++;
			continue;
		}
		if(x->mode==POSTING||x->mode==POSTING+20000
		        ||x->mode==EDIT||x->mode==EDIT+20000)
		{
			MODEPOSTING++;
			continue;
		}
		if(x->mode==TALK||x->mode==TALK+20000
		        ||x->mode==TMENU||x->mode==TMENU+20000
		        ||x->mode==PAGE||x->mode==PAGE+20000
		        ||x->mode==CHAT1||x->mode==CHAT1+20000
		        ||x->mode==CHAT2||x->mode==CHAT2+20000
		        ||x->mode==MSG||x->mode==MSG+20000)
		{
			MODECHAT++;
			continue;
		}
		if(x->mode==LUSERS||x->mode==LUSERS+20000
		        ||x->mode==FRIEND||x->mode==FRIEND+20000
		        ||x->mode==LAUSERS)
		{
			MODEUSER++;
			continue;
		}
		if(x->mode==MAIL||x->mode==MAIL+20000
		        ||x->mode==SMAIL||x->mode==SMAIL+20000
		        ||x->mode==RMAIL||x->mode==RMAIL+20000)
		{
			MODEMAIL++;
			continue;
		}
		if(x->mode==BBSNET)
		{
			MODEBBSNET++;
			continue;
		}
		if(x->mode==LOCKSCREEN)
		{
			MODELOCKSCREEN++;
			continue;
		}
		MODEELSE++;
	}
	getdatestring(time(0));
	clear();
	move(0,0);
	sprintf(buf, "                     ÔÚÏßÓÃ»§Çé¿öÍ³¼Æ\n");
	sprintf(buf, "%sµ±Ç°Ê±¼ä£º%s\n\n", buf, datestring);
	sprintf(buf, "%sÄ¿Ç°ÔÚÏßÈËÊı£º%4d, ÆäÖĞWWW·½Ê½ÔÚÏßÈËÊı£º%4d, Õ¼%5.2f£¥\n", buf, total, www, www*100.0/total);
	sprintf(buf, "%sÄ¿Ç°¹²ÓĞguestÊı£º%3d, ÆäÖĞWWW·½Ê½guestÊı£º%3d, Õ¼%5.2f£¥\n\n", buf, guest, wwwguest, wwwguest*100.0/guest);
	sprintf(buf, "%sÓÃ»§¸÷Àà×´Ì¬ËùÕ¼±ÈÀıÈçÏÂ£º\n", buf);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "ÔÄ¶ÁÎÄÕÂ(Æ·Î¶ÎÄÕÂ¡¢ÔÄÀÀĞÂÎÄÕÂ)",		MODEREADING, MODEREADING*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "ÔÄ¶Á¾«»ªÇø(ä¯ÀÀ¾«»ªÇø¡¢±àĞŞ¾«»ª)",		MODEDIGEST, MODEDIGEST*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "·¢±íÎÄÕÂ(ÎÄºÀ»Ó±Ê¡¢ĞŞ¸ÄÎÄÕÂ)", MODEPOSTING, MODEPOSTING*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "Ê¹ÓÃÕßÑ¡µ¥(»·¹ËËÄ·½¡¢Ñ°ÕÒºÃÓÑ¡¢...)", MODEUSER, MODEUSER*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "´¦ÀíĞÅ¼ş(ÔÄÀÀĞÅ¼ã¡¢´¦ÀíĞÅ¼ã¡¢¼ÄÓïĞÅ¸ë)", MODEMAIL, MODEMAIL*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "ÁÄÌì(ÈµÇÅÏ¸Óï¡¢ËÍÑ¶Ï¢¡¢²é¿´Ñ¶Ï¢¡¢...)", MODECHAT, MODECHAT*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "ÍøÂç´©Ëó",
	        MODEBBSNET,	MODEBBSNET*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "Ó©Ä»Ëø¶¨",
	        MODELOCKSCREEN,	MODELOCKSCREEN*100.0/total);
	sprintf(buf, "%s%40s£º%3dÈË£¬Õ¼%5.2f£¥\n", buf, "ÆäËü×´Ì¬",
	        MODEELSE,	MODEELSE*100.0/total);
	sprintf(buf, "%s\nÓÃ»§À´Ô´·ÖÎö£º\n", buf);
	sprintf(buf, "%sÄÏ¿ªËŞÉáÄÚÍø£º%3d£¬Õ¼%5.2f£¥\n", buf, dorm, dorm*100.0/total);
	sprintf(buf, "%sĞ£ÄÚ½ÌÓıÍø¶Î£º%3d£¬Õ¼%5.2f£¥\n", buf, in, in*100.0/total);
	sprintf(buf, "%s    Ğ£ÍâÍø¶Î£º%3d£¬Õ¼%5.2f£¥\n", buf, out, out*100.0/total);
	sprintf(buf, "%s\n×¢£ºÒÔÉÏÍ³¼Æ¾ù°üÀ¨ÒşÉíÔÚÏßÓÃ»§\n", buf);
	prints(buf);
	pressanykey();
	return 1;
}
int beginwithstr(char* strsrc,char* strcmp)/* test where strsrc start with strcmp , by brew 2005.6.9 
				To Be Done*/
{
	register	int i;
	if(strsrc==NULL||strcmp==NULL)
		return NA;
	i=0;
	while(strsrc[i]==strcmp[i]&&strcmp[i]&&strsrc[i])i++;
	if(strcmp[i]=='\0')
		return YEA;
	return NA;
	
}

int fill_userlistex(char* searchstr)
{
	register int i, n, totalusernum;
	int friendno[MAXFRIENDS];
	extern struct UTMPFILE *utmpshm;
	//struct user_info *friends_online[MAXFRIENDS];
	resolve_utmp();
	totalusernum = 0;
	numf = 0;
	friendmode = 0;/* no friend mode support currently */
	
	for (i = 0; i < USHM_SIZE; i++)
	{
		if (  !utmpshm->uinfo[i].active
		        ||!utmpshm->uinfo[i].pid||isreject(&utmpshm->uinfo[i]))
			continue;
		if (  (utmpshm->uinfo[i].invisible)
		        &&(usernum != utmpshm->uinfo[i].uid)
		        &&(!(HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)||canseeme(&utmpshm->uinfo[i]))))
			continue;
		if( beginwithstr(utmpshm->uinfo[i].from,searchstr) == NA )
			continue;
		if (myfriend(utmpshm->uinfo[i].uid))
		{
		/*	if(numf>=MAXFRIENDS)
				goto out;*/
			if(numf<MAXFRIENDS)
				friendno[numf++] = totalusernum;

		}
		else if( friendmode )   
			continue; 
/*out:*/
		user_record[totalusernum++] = &utmpshm->uinfo[i];
	}


	if (!friendmode)
	{
		for (i=0,n=0; i < totalusernum; i++)
		{
			if( n >= numf )
				break;
			if (friendno[n] == i)
			{
				if( i != n )
					swap_user_record(n, i);
				n ++;
			}
		}
		if( numf > 2 )
		{
			sort_user_record(0, numf - 1);
		}
		else if( numf == 2 )
		{
			if(compare_user_record(user_record[0], user_record[1])>0)
				swap_user_record(0,1);
		}
		sort_user_record(numf, totalusernum - 1);
	}
	else
	{
		if( totalusernum > 2 )
		{
			sort_user_record(0, totalusernum - 1);
		}
		else if( totalusernum == 2 )
		{
			if(compare_user_record(user_record[0], user_record[1])>0)
				swap_user_record(0,1);
		}
	}
	range = totalusernum;
	return totalusernum == 0 ? -1 : 1;
}
char	g_str_ip[20];

int show_ipuserlist()
{
	if (update_time + refreshtime < time(0))
	{
		fill_userlistex(g_str_ip);
		update_time = time(0);
	}
	if (range == 0 /* ||fill_userlistex() == 0 */ )
	{
		move(2, 0);
		prints("Ã»ÓĞÊ¹ÓÃÕß£¨ÅóÓÑ£©ÔÚÁĞ±íÖĞ...\n");
		clrtobot();
		pressanykey();
		return -1;
	}
	do_userlist();
	clrtobot();
	return 1;

}

int t_lookipusers()
{
	int  savemode;

	
	savemode = uinfo.mode;
	move(10, 0);
        clrtobot();
        memset(g_str_ip,0,20);
        getdata( 13, 0,"ÇëÊäÈëÄúÒª²éÑ¯ipµØÖ·:", g_str_ip, 15, DOECHO, NULL, YEA);
        if(g_str_ip == NULL||g_str_ip[0] == '\0')
        {
                 return -1;
        }
        fill_userlistex(g_str_ip);
        if (range == 0 /* ||fill_userlist() == 0 */ )
        {
                move(13, 0);
                prints("Ã»ÓĞÊ¹ÓÃÕß£¨ÅóÓÑ£©ÔÚÁĞ±íÖĞ...\n");
		clrtobot();
		pressanykey();
		return -1;
        }
        else
        {
        	modify_user_mode(LUSERS);
                update_time = 0;
                choose(YEA, 0, print_title, deal_key, show_ipuserlist, do_query);
	}
	clear();
	
	friendmode = NA;

	modify_user_mode( savemode );
	return -1;
}

int canseeme(struct user_info *uentp)
{
	struct userec temp;
	if (uentp->uid == 0) return 0;
	get_record(PASSFILE, &temp, sizeof(temp), uentp->uid);
	sprintf(genbuf, "home/%c/%s/mylover", toupper(temp.userid[0]), temp.userid);
	if(dashf(genbuf))
		return seek_in_file(genbuf,currentuser.userid);
	return 0;

	
}//loveni:ÌØ¶¨Ãûµ¥¿´´©ÒşÉí



int edit_mylover()
{
	sprintf(genbuf,"home/%c/%s/mylover",toupper(currentuser.userid[0]),currentuser.userid);
	modify_user_mode(USERDEF);
	ListFileEdit(genbuf,"±à¼­¡º¿É¿´´©ÎÒÒşÉí¡»Ãûµ¥");
	return 0;
}





	

