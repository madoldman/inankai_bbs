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
$Id: xyz.c,v 1.8 2009-01-07 02:48:07 madoldman Exp $
*/

#define EXTERN
#include "bbs.h"
#include <stdio.h>
#define PAGESIZE    (t_lines - 4)

int     use_define = 0;
int child_pid=0;
extern int iscolor;
extern int enabledbchar;
int postable=0;//Èç¹ûÊÇ1ÓĞÍâÍø·¢ÎÄÈ¨ÏŞ

#ifdef ALLOWSWITCHCODE
extern int switch_code();
extern int convcode;
#endif

char datestring[30];

extern struct BCACHE *brdshm;
#define TH_LOW	10
#define TH_HIGH	15

char *permstr="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#";

int modify_user_mode(int mode)
{
	if (uinfo.mode == mode)
		return 0; /* add by yiyo ±ØĞë¼õÉÙupdate_ulistµÄ´ÎÊı. */
	uinfo.mode = mode;
	update_ulist(&uinfo, utmpent);
	return 0;
}

int showperminfo(int pbits, int i)
{
	char    buf[STRLEN];
	//	sprintf(buf, "%c. %-30s %2s", 'A' + i, (use_define) ? user_definestr[i] : permstrings[i],
	//		((pbits >> i) & 1 ? "ÊÇ" : "¡Á"));
	sprintf(buf, "%c. %-30s %2s", permstr[i], (use_define) ? user_definestr[i] : permstrings[i],
	        ((pbits >> i) & 1 ? "ÊÇ" : "¡Á"));
	move(i + 6 - ((i > 15) ? 16 : 0), 0 + ((i > 15) ? 40 : 0));
	prints(buf);
	refresh();
	return YEA;
}

unsigned int setperms(unsigned int pbits, char* prompt, int numbers, int (*showfunc)())
{
	int     lastperm = numbers - 1;
	int     i, j , done = NA;
	char    choice[3], buf[80];
	move(4, 0);
	prints("Çë°´ÏÂÄãÒªµÄ´úÂëÀ´Éè¶¨%s£¬°´ Enter ½áÊø.\n", prompt);
	move(6, 0);
	clrtobot();
	for (i = 0; i <= lastperm; i++)
	{
		(*showfunc) (pbits, i, NA);
	}
	while (!done)
	{
		sprintf(buf, "Ñ¡Ôñ(ENTER ½áÊø%s): ", ((strcmp(prompt, "È¨ÏŞ") != 0)) ? "" : "£¬0 Í£È¨");
		getdata(t_lines - 1, 0, buf, choice, 2, DOECHO, YEA);
		*choice = toupper(*choice);
		/*		if (*choice == '0')
					return (0);
				else if (*choice == '\n' || *choice == '\0')
					done = YEA;
				else if (*choice < 'A' || *choice > 'A' + lastperm)
					bell();
				else {*/
		/*add by livebird 11.23 2004*/
	/*	if (!HAS_PERM(PERM_SYSOP) && (*choice != '3') && (*choice != '\n')&&( *choice != '\0')){
			move(2, 0);
			prints("ÄúÃ»ÓĞÈ¨ÏŞ¸ü¸Ä´ËÏî");	
			clrtoeol();
		}
		else
		{	*/
	   	    i=-1;
	  	    for(j=0;j<=lastperm;j++)
		    {
			if(choice[0]==permstr[j])
			{
				i=j;
				break;

			}
	 	    }
		//			i = *choice - 'A';
			if (*choice == '\n' || *choice == '\0')
			done = YEA;
			else if(i==-1)
			{}
			else
			{
				pbits ^= (1 << i);
				if ((*showfunc) (pbits, i, YEA) == NA)
				{
					pbits ^= (1 << i);
				}
			}
	//	}
	}
	return (pbits);
}

int x_userdefine()
{
	int     id;
	unsigned int newlevel;
	modify_user_mode(USERDEF);
	if (!(id = getuser(currentuser.userid)))
	{
		move(3, 0);
		prints("´íÎóµÄÊ¹ÓÃÕß ID...");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	use_define = 1;
	newlevel = setperms(lookupuser.userdefine, "²ÎÊı", NUMDEFINES, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userdefine)
		prints("²ÎÊıÃ»ÓĞĞŞ¸Ä...\n");
	else
	{
#ifdef ALLOWSWITCHCODE
/*		if ((!convcode&&!(newlevel&DEF_USEGB))
		        ||(convcode&&(newlevel&DEF_USEGB)))
			switch_code();*/
#endif

		lookupuser.userdefine = newlevel;
		currentuser.userdefine = newlevel;
		substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
		uinfo.pager |= FRIEND_PAGER;
		if (!(uinfo.pager & ALL_PAGER))
		{
			if (!DEFINE(DEF_FRIENDCALL))
				uinfo.pager &= ~FRIEND_PAGER;
		}
		uinfo.pager &= ~ALLMSG_PAGER;
		uinfo.pager &= ~FRIENDMSG_PAGER;
		if (DEFINE(DEF_DELDBLCHAR))
			enabledbchar=1;
		else
			enabledbchar=0;
		//uinfo.from[22] = DEFINE(DEF_NOTHIDEIP)?'S':'H';
		uinfo.from[22] = 'S';//loveni
		if (DEFINE(DEF_FRIENDMSG))
		{
			uinfo.pager |= FRIENDMSG_PAGER;
		}
		if (DEFINE(DEF_ALLMSG))
		{
			uinfo.pager |= ALLMSG_PAGER;
			uinfo.pager |= FRIENDMSG_PAGER;
		}
		update_utmp();
		prints("ĞÂµÄ²ÎÊıÉè¶¨Íê³É...\n\n");
	}
	//	iscolor = (DEFINE(DEF_COLOR)) ? 1 : 0;
	iscolor=1;
	pressreturn();
	clear();
	use_define = 0;
	return 0;
}

int x_cloak()
{
	modify_user_mode(GMENU);
	report("toggle cloak");
	uinfo.invisible = (uinfo.invisible) ? NA : YEA;
	update_utmp();
	if (!uinfo.in_chat)
	{
		move(1, 0);
		clrtoeol();
		prints("ÒşÉíÊõ (cloak) ÒÑ¾­%sÁË!",
		       (uinfo.invisible) ? "Æô¶¯" : "Í£Ö¹");
		pressreturn();
	}
	return 0;
}

void x_edits()
{
	int     aborted;
	char    ans[7], buf[STRLEN];
	int     ch, num, confirm;
	extern int WishNum;
	static char *e_file[] =
	    {"plans", "signatures", "notes", "logout", "GoodWish","badhost","bbsnet", NULL};
	static char *explain_file[] =
	    {"¸öÈËËµÃ÷µµ", "Ç©Ãûµµ", "×Ô¼ºµÄ±¸ÍüÂ¼", "ÀëÕ¾µÄ»­Ãæ","µ×²¿Á÷¶¯ĞÅÏ¢","µÇÂ¼µØÖ·ÏŞÖÆ","×ÔÑ¡´©ËóÕ¾µã", NULL};
	modify_user_mode(GMENU);
	clear();
	move(1, 0);
	prints("±àĞŞ¸öÈËµµ°¸\n\n");
	for (num = 0; e_file[num] != NULL && explain_file[num] != NULL; num++)
	{
		prints("[[1;32m%d[m] %s\n", num + 1, explain_file[num]);
	}
	prints("[[1;32m%d[m] ¶¼²»Ïë¸Ä\n", num + 1);

	getdata(num + 5, 0, "ÄãÒª±àĞŞÄÄÒ»Ïî¸öÈËµµ°¸: ", ans, 2, DOECHO, YEA);
	if (ans[0] - '0' <= 0 || ans[0] - '0' > num || ans[0] == '\n' || ans[0] == '\0')
		return;

	ch = ans[0] - '0' - 1;
	setuserfile(genbuf, e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)±à¼­ (D)É¾³ı %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		confirm = askyn("ÄãÈ·¶¨ÒªÉ¾³ıÕâ¸öµµ°¸", NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
			pressreturn();
			clear();
			return;
		}
		unlink(genbuf);
		move(5, 0);
		prints("%s ÒÑÉ¾³ı\n", explain_file[ch]);
		sprintf(buf, "delete %s", explain_file[ch]);
		report(buf);
		pressreturn();
		if(ch == 4)
			WishNum = 9999;
		clear();
		return;
	}
	modify_user_mode(EDITUFILE);
	aborted = vedit(genbuf, NA, YEA);
	clear();
	if (!aborted)
	{
		prints("%s ¸üĞÂ¹ı\n", explain_file[ch]);
		sprintf(buf, "edit %s", explain_file[ch]);
		if (!strcmp(e_file[ch], "signatures"))
		{
			set_numofsig();
			prints("ÏµÍ³ÖØĞÂÉè¶¨ÒÔ¼°¶ÁÈëÄãµÄÇ©Ãûµµ...");
		}
		report(buf);
	}
	else
		prints("%s È¡ÏûĞŞ¸Ä\n", explain_file[ch]);
	pressreturn();
	if(ch == 4)
		WishNum = 9999;
}

int gettheuserid(int x,char *title, int *id)
{
	move(x, 0);
	usercomplete( title , genbuf);
	if (*genbuf == '\0')
	{
		clear();
		return 0;
	}
	if (!(*id = getuser(genbuf)))
	{
		move(x + 3, 0);
		prints("´íÎóµÄÊ¹ÓÃÕß´úºÅ");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	return 1;
}

int gettheboardname(int x,char* title, int* pos,
	struct boardheader* fh, char* bname )
{
	extern int cmpbnames();
	move( x, 0);
	make_blist();
	namecomplete(title, bname);
	if (*bname == '\0')
	{
		return 0;
	}
	*pos = search_record(BOARDS, fh, sizeof(struct boardheader), cmpbnames,
	                     bname);
	if(!(*pos))
	{
		getboardbytitle(bname);
		*pos = search_record(BOARDS, fh, sizeof(struct boardheader), 
				cmpbnames, bname);

	}
	if (!(*pos))
	{
		move(x + 3, 0);
		prints("²»ÕıÈ·µÄÌÖÂÛÇøÃû³Æ");
		pressreturn();
		clear();
		return 0;
	}
	return 1;
}

void x_lockscreen()
{
	char    buf[PASSLEN + 1] ;
	time_t  now;

	if (!strcasecmp (currentuser.userid, "guest"))
	{
		move (3, 0);
		clrtobot ();
		update_endline ();
		move (9, 0);
		prints ("        faint...guestÒ²ÏëÆÁÄ»Ëø¶¨£¿!!");
		pressreturn ();
		return;
	}
	modify_user_mode(LOCKSCREEN);
	move(9, 0);
	clrtobot();
	update_endline();
	buf[0] = '\0';
	now = time(0);
	getdatestring(now,NA);
	//tdhlshx for Ó©Ä»Ëø¶¨ 03.09.04.
	time_t nowtime=-1;
	nowtime=now;
	int num;
	int locknumall=(countln("etc/lock"))/10;

	if(locknumall>0)
	{
		num=nowtime%locknumall;
		while (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
		{
			//if(*buf = '\n')
			//{
				num=(num+1)%locknumall;
				move(9,0);
				clrtobot();
				print_file("etc/lock",num*10+1,10);
				getdatestring(now,NA);//Ôö¼Ó,ĞŞ²¹ÏÔÊ¾µÄbug.
				prints("[1;36mÓ©Ä»ÒÑÔÚ[33m %s[36m Ê±±»[32m %-12s [36mÔİÊ±Ëø×¡ÁË...[m", datestring, currentuser.userid);
			//}
			move(20, 0);
			clrtobot();
			update_endline();
			//      move(19,0);
			//      clrtoeofl();
			getdata(20, 0, "ÇëÊäÈëÄãµÄÃÜÂëÒÔ½âËø;°´»Ø³µ¸ü»»Ëø¶¨»­Ãæ:", buf, PASSLEN, NOECHO, YEA);
		}

	}
	else
	{//·ÀÖ¹ÃÀ¹¤É¾³ıºóÃ»ÓĞ½¨Á¢lockÎÄ¼ş
		move(9,0);		
		prints("ÏµÍ³¶ÁÈ¡Ëø¶¨»­ÃæÎÄ¼ş³öÏÖ´íÎó,Çë±¨¸æ±¾Õ¾ÏµÍ³Î¬»¤,Ğ»Ğ»!\n");
		pressanykey();
		move(9,0);
		clrtobot();
		prints("[1;37m");
		prints("\n       _       _____   ___     _   _   ___     ___       __");
		prints("\n      ( )     (  _  ) (  _`\\  ( ) ( ) (  _`\\  (  _`\\    |  |");
		prints("\n      | |     | ( ) | | ( (_) | |/'/' | (_(_) | | ) |   |  |");
		prints("\n      | |  _  | | | | | |  _  | , <   |  _)_  | | | )   |  |");
		prints("\n      | |_( ) | (_) | | (_( ) | |\\`\\  | (_( ) | |_) |   |==|");
		prints("\n      (____/' (_____) (____/' (_) (_) (____/' (____/'   |__|\n[m");

		move(17,0);
		prints("\n[1;36mÓ©Ä»ÒÑÔÚ[33m %s[36m Ê±±»[32m %-12s [36mÔİÊ±Ëø×¡ÁË...[m", datestring, currentuser.userid);
		while (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
		{
			move(18, 0);
			clrtobot();
			update_endline();
			getdata(19, 0, "ÇëÊäÈëÄãµÄÃÜÂëÒÔ½âËø: ", buf, PASSLEN, NOECHO, YEA);
		}

	}
	/*while (*buf == '\0' || !checkpasswd(currentuser.passwd, buf)) {
	               if(*buf='\n')
		{
			num=(num+1)%locknumall;
			move(9,0);
			clrtobot();
			print_file("etc/lock",num*10+1,10);
			getdatestring(now,NA);//Ôö¼Ó,ĞŞ²¹ÏÔÊ¾µÄbug.
			prints("[1;36mÓ©Ä»ÒÑÔÚ[33m %s[36m Ê±±»[32m %-12s [36mÔİÊ±Ëø×¡ÁË...[m", datestring, currentuser.userid);
					
		}		
		move(20, 0);//×¢ÊÍ
		clrtobot();//×¢ÊÍµô
		*/
}
int print_file(char * filename,int line,int numofline)
{
	FILE * fp;
	int i,j;
	char inbuf[256];
	if((fp=fopen(filename,"r"))==NULL)
		return 1;
	for(i=1;i<line;i++)
	{
		if(!fgets(inbuf,sizeof(inbuf),fp))
		{
			fclose(fp);
			return 1;
		}
	}
	move(9,0);
	clrtobot();
	update_endline();

	for(j=line;j<line+numofline;j++)
	{
		move(9+j-line,0);
		fgets(inbuf,sizeof(inbuf),fp);
		prints("%s",inbuf);
	}
	prints("[m");
	fclose(fp);
	return 1;
}
/*void get_lockpic(char * filename1,int num,int numline,char * filename2)//½«ÎÄ¼ş1µÄÒ»N1ĞĞ¿ªÊ¼,NĞĞĞ´ÈëÎÄ¼ş2
{
}*/
int y_lockscreen()
{
	x_lockscreen();
	return FULLUPDATE;
}
int showbadwords()
{
	clear ();
	move (2, 10);
	prints ("[1;31mÒÔÏÂÊÇ±¾Õ¾½ûÓÃ´Ê»ã£¬·¢ÎÄÇëÎğÊ¹ÓÃ£¡[m");
	move (3, 0);
	ansimore2 ("etc/.badwords", YEA, 0, 0);
	return FULLUPDATE;
}

int heavyload()
{
#ifndef BBSD
	double  cpu_load[3];
	get_load(cpu_load);
	if (cpu_load[0] > 15)
		return 1;
	else
		return 0;
#else
 #ifdef chkload

	register int load;
	register time_t uptime;

	if ( time(0) > uptime )
	{
		load = chkload(load ? TH_LOW : TH_HIGH );
		uptime = time(0) + load + 45;
	}
	return load;
#else

	return 0;
#endif
#endif
}

//#define MY_DEBUG

void exec_cmd(int umode, int pager, char *cmdfile, char *param1)
{
	char    buf[160];
	char    *my_argv[18], *ptr;
	int     save_pager, i;

	signal(SIGALRM, SIG_IGN);
	modify_user_mode(umode);
	clear();
	move(2, 0);
	if (num_useshell() > MAX_USESHELL)
	{
		prints("Ì«¶àÈËÊ¹ÓÃÍâ²¿³ÌÊ½ÁË£¬ÄãµÈÒ»ÏÂÔÙÓÃ°É...");
		pressanykey();
		return;
	}
	if (!HAS_PERM(PERM_SYSOP) && heavyload())
	{
		clear();
		prints("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ıÖØ£¬´Ë¹¦ÄÜÔİÊ±²»ÄÜÖ´ĞĞ...");
		pressanykey();
		return;
	}
	if (!dashf(cmdfile))
	{
		prints("ÎÄ¼ş [%s] ²»´æÔÚ£¡\n", cmdfile);
		pressreturn();
		return;
	}
	save_pager = uinfo.pager;
	if (pager == NA)
	{
		uinfo.pager = 0;
	}
	sprintf(buf,"%s %d",cmdfile,getpid());
	report(buf);
	my_argv[0] = cmdfile;
	strcpy(buf,param1);
	if(buf[0] != '\0')
		ptr = strtok(buf," \t");
	else
		ptr = NULL;
	for(i = 1; i< 18; i++)
	{
		if(ptr)
		{
			my_argv[i] = ptr;
			ptr = strtok(NULL," \t");
		}
		else
			my_argv[i] = NULL;
	}
#ifdef MY_DEBUG
	for(i = 0; i< 18; i++)
	{
		if(my_argv[i] != NULL)
			prints("my_argv[%d] = %s\n",i, my_argv[i]);
		else
			prints("my_argv[%d] = (none)\n",i);
	}
	pressanykey();
#else

	child_pid = fork();
	if(child_pid == -1)
	{
		prints("×ÊÔ´½ôÈ±£¬fork() Ê§°Ü£¬ÇëÉÔºóÔÙÊ¹ÓÃ");
		child_pid = 0;
		pressreturn();
		return;
	}
	if(child_pid == 0)
	{
		execv(cmdfile,my_argv);
		exit(0);
	}
	else
		waitpid(child_pid, NULL, 0);
#endif

	child_pid = 0;
	uinfo.pager = save_pager;
	clear();
}

void x_showuser()
{
	char    buf[STRLEN];
	modify_user_mode(SYSINFO);
	clear();
	stand_title("±¾Õ¾Ê¹ÓÃÕß×ÊÁÏ²éÑ¯");
	ansimore("etc/showuser.msg", NA);
	getdata(20, 0, "Parameter: ", buf, 30, DOECHO, YEA);
	if ((buf[0] == '\0') || dashf("tmp/showuser.result"))
		return;
	securityreport("²éÑ¯Ê¹ÓÃÕß×ÊÁÏ");
	exec_cmd(SYSINFO, YEA, "bin/showuser", buf);
	sprintf(buf, "tmp/showuser.result");
	if (dashf(buf))
	{
		mail_file(buf, currentuser.userid, "Ê¹ÓÃÕß×ÊÁÏ²éÑ¯½á¹û");
		unlink(buf);
	}
}


int ent_bnet()
{
	char buf[80];
	sprintf(buf,"etc/bbsnet.ini %s",currentuser.userid);
	exec_cmd(BBSNET,NA,"bin/bbsnet",buf);
}

/* add by yiyo */
int ent_bnet1()
{
	char buf[80];
	sprintf(buf,"etc/bbsnet1.ini %s",currentuser.userid);
	exec_cmd(BBSNET,NA,"bin/bbsnet",buf);
}
/* add end by yiyo */

/* add by bluetent ×ÔÑ¡´©Ëó 2003.01.18 */
int ent_bnet2()
{
	char buf[256];
	FILE *fp;
	sprintf(buf,"home/%c/%s/bbsnet",toupper(currentuser.userid[0]),currentuser.userid);
	if(fp=fopen(buf,"r"))
	{
		fclose(fp);
		sprintf(buf,"%s %s",buf,currentuser.userid);
	}
	else
	{
		clear();
		move(10,10);
		prints("ÄúÉĞÎ´ÉèÖÃ×ÔÑ¡´©ËóÁĞ±í£¬Çë½øÈë¡°±àĞŞ¸öÈËµµ°¸¡±Ñ¡µ¥ĞŞ¸Ä\n");
		prints("ÒÔĞĞÎªµ¥Î»,¸ñÊ½ÈçÏÂÀıÊ¾:\n");
		prints("    Õ¾µãËµÃ÷    Õ¾µãÃû³Æ  Õ¾µãµØÖ·\n");
		prints("    ÄÏ¿ªÈËÉçÇø   %s  %s\n",BBSNAME,BBSHOST);
		pressanykey();
		return FULLUPDATE;
	}

	exec_cmd(BBSNET,NA,"bin/bbsnet",buf);
}

int winmine()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(WINMINE,NA,"bin/winmine",buf);
}

int winmine2()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(WINMINE2,NA,"bin/winmine2",buf);
}

void fill_date()
{
	time_t  now, next;
	char   *buf, *buf2, *index, index_buf[5], *t=NULL;
	char    h[3], m[3], s[3];
	int     foo, foo2, i;
	struct tm *mytm;
	FILE   *fp;
	now = time(0);
	resolve_boards();

	if (now < brdshm->fresh_date && strlen(brdshm->date) != 0)
		return;

	mytm = localtime(&now);
	strftime(h, 3, "%H", mytm);
	strftime(m, 3, "%M", mytm);
	strftime(s, 3, "%S", mytm);

	next = (time_t)time(0) - ((atoi(h) * 3600) + (atoi(m) * 60) + atoi(s))
	       + 86400;	/* Ëã³ö½ñÌì 0:0:00 µÄÊ±¼ä, È»ááÔÙÍùáá¼ÓÒ»Ìì */
	sprintf(genbuf, "¼ÍÄîÈÕ¸üĞÂ, ÏÂÒ»´Î¸üĞÂÊ±¼ä %s", Cdate(&next));
	report(genbuf);

	buf = (char *) malloc(80);
	buf2 = (char *) malloc(30);
	index = (char *) malloc(5);

	fp = fopen(DEF_FILE, "r");

	if (fp == NULL)
		return;

	now = time(0);
	mytm = localtime(&now);
	strftime(index_buf, 5, "%m%d", mytm);

	strcpy(brdshm->date,DEF_VALUE);

	while (fgets(buf, 80, fp) != NULL)
	{
		if (buf[0] == ';' || buf[0] == '#' || buf[0] == ' ')
			continue;

		buf[35] = '\0';
		strncpy(index,buf,4);
		index[4] = '\0';
		strcpy(buf2,buf+5);
		t = strchr(buf2,'\n');
		if(t)
			*t = '\0';

		if (index[0] == '\0' || buf2[0] == '\0')
			continue;

		if (strcmp(index, "0000") == 0||strcmp(index_buf, index) == 0)
		{
			foo = strlen(buf2);
			foo2 = (30 - foo) / 2;
			strcpy(brdshm->date, "");
			for (i = 0; i < foo2; i++)
				strcat(brdshm->date, " ");
			strcat(brdshm->date, buf2);
			for (i = 0; i < 30 - (foo + foo2); i++)
				strcat(brdshm->date, " ");
		}
	}

	fclose(fp);
	brdshm->fresh_date = next;

	free(buf);
	free(buf2);
	free(index);

	return;
}

int is_birth(struct userec user)
{
	struct tm *tm;
	time_t now;

	now = time(0);
	tm = localtime(&now);

	if ( strcasecmp(user.userid, "guest") == 0 )
		return NA;

	if ( user.birthmonth == (tm->tm_mon + 1) && user.birthday == tm->tm_mday )
		return YEA;
	else
		return NA;
}

int sendgoodwish(char *uid)
{
	return sendGoodWish(NULL);
}

int sendGoodWish(char *userid)
{
	FILE   *fp;
	int     tuid, i, count;
	time_t  now;
	char    buf[5][STRLEN], tmpbuf[STRLEN];
	char    uid[IDLEN+1],*timestr;

	modify_user_mode(GOODWISH);
	clear();
	move(1, 0);
	prints("[0;1;32mÁôÑÔ±¾[m\nÄú¿ÉÒÔÔÚÕâÀï¸øÄúµÄÅóÓÑËÍÈ¥ÄúµÄ×£¸££¬");
	prints("\nÒ²¿ÉÒÔÎªÄú¸øËû/ËıÉÓÉÏÒ»¾äÇÄÇÄ»°¡£");
	move(5,0);
	if(userid == NULL)
	{
		usercomplete("ÇëÊäÈëËûµÄ ID: ", uid);
		if (uid[0] == '\0')
		{
			clear();
			return 0;
		}
	}
	else
		strcpy(uid,userid);
	if (!(tuid = getuser(uid)))
	{
		move(7, 0);
		prints("[1mÄúÊäÈëµÄÊ¹ÓÃÕß´úºÅ( ID )²»´æÔÚ£¡[m\n");
		pressanykey();
		clear();
		return -1;
	}
	if(!strcmp(uid,"guest"))
	{
		 move(7, 0);
		 prints("²»Òª¸øguestÁôÑÔ...\n");
		 pressanykey();
		 clear();
		 return -1;		 
	}
	move(5,0);
	clrtoeol();
	prints("[m¡¾¸ø [1m%s[m ÁôÑÔ¡¿", uid);
	move(6,0);
	prints("ÄúµÄÁôÑÔ[Ö±½Ó°´ ENTER ½áÊøÁôÑÔ£¬×î¶à 5 ¾ä£¬Ã¿¾ä×î³¤ 50 ×Ö·û]:");
	for( count = 0; count < 5; count ++ )
	{
		getdata(7+count, 0, ": ", tmpbuf, 51, DOECHO, YEA);
		if(tmpbuf[0] == '\0')
			break;
		;
		if(killwordsp(tmpbuf)==0)
		{
			count --;
			continue;
		}
		strcpy(buf[count],tmpbuf);
	}
	if(count == 0)
		return 0;
	sprintf(genbuf, "ÄãÈ·¶¨Òª·¢ËÍÕâÌõÁôÑÔ¸ø [1m%s[m Âğ", uid);
	move(9+count, 0);
	if (askyn(genbuf, YEA, NA) == NA)
	{
		clear();
		return 0;
	}
	sethomefile(genbuf,uid,"GoodWish");
	if ((fp = fopen(genbuf, "a")) == NULL)
	{
		prints("ÎŞ·¨¿ªÆô¸ÃÓÃ»§µÄµ×²¿Á÷¶¯ĞÅÏ¢ÎÄ¼ş£¬ÇëÍ¨ÖªÕ¾³¤...\n");
		pressanykey();
		return NA;
	}
	now = time(0);
	timestr = ctime(&now) + 11;
	*(timestr + 5) = '\0';
	for(i = 0; i< count ; i++)
	{
		fprintf(fp, "%s(%s)[%d/%d]£º%s\n",
		        currentuser.userid,timestr, i+1,count,buf[i]);
	}
	fclose(fp);
	sethomefile(genbuf,uid,"HaveNewWish");
	if((fp = fopen(genbuf,"w+")) != NULL)
	{
		fputs("Have New Wish",fp);
		fclose(fp);
	}
	move(11+count, 0);
	prints("ÒÑ¾­°ïÄúËÍ³öÄúµÄÁôÑÔÁË¡£");
	sprintf(genbuf,"SendGoodWish to %s",uid);
	report(genbuf);
	pressanykey();
	clear();
	return 0;
}

int getdatestring( time_t now, int Chang)
{
	struct tm *tm;
	char weeknum[7][3]={"Ìì","Ò»","¶ş","Èı","ËÄ","Îå","Áù"};

	tm = localtime(&now);
	sprintf(datestring,"%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ĞÇÆÚ%2s",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
	        tm->tm_hour,tm->tm_min,tm->tm_sec,
	        weeknum[tm->tm_wday]);
	return (tm->tm_sec % 10);
}
/* Added End. */

//add by yiyo ÖĞÓ¢ÎÄ×Öµä
int x_dict()
{
	int n, c;
	char buf[32], filebuf[80];
	struct stat statbuf;
	modify_user_mode(DICT);
	while(1)
	{
		clear();
		refresh();
		prints("±¾×Öµä [1;33mCDICT5.0[m ÊÕÂ¼µ¥´Ê½ü [1;32m40000[m , Ò»°ã½Ï³£¼ûµÄµ¥´ÊºÍËõĞ´¾ùÒÑÊÕÂ¼,\n");
		prints("µ«Î´ÊÕÂ¼´Ê×éÒÔ¼°¸÷Àà×¨Òµ´Ê»ã. ÖĞÓ¢ÎÄÊäÈë¾ù¿É, Ôİ²»Ö§³Ö'[1;32m*[m'ºó×º.\r\n\r\n");
		getdata(3,0,"ÇëÊäÈëÒª²éÑ¯µÄÖĞÓ¢ÎÄµ¥´Ê: ",buf,24,DOECHO,YEA);
		if(!buf[0])
			break;
		for(n=0; n<strlen(buf); n++)
		{
			c=toupper(buf[n])%255;
			if(c>='A' && c<='Z')
				continue;
			if(c>=160 && c<=254)
				continue;
			if(c=='.' || c=='*')
				continue;
			prints("´íÎóµÄÊäÈë!\n");
			pressreturn();
			return;
		}
		sprintf(genbuf, "cdict %s", buf);
		report(genbuf);
		sprintf(filebuf, "tmp/%s.dict", currentuser.userid);
		sprintf(genbuf, "bin/dict '%s' > %s", buf, filebuf);
		system(genbuf);
		bzero(&statbuf, sizeof(statbuf));
		stat(filebuf, &statbuf);
		if(statbuf.st_size>1)
		{
			ansimore(filebuf, YEA);
			if(askyn("Òª½«²éÑ¯½á¹û¼Ä»Øµ½ĞÅÏäÂğ", NA, NA)==YEA)
				mail_file(filebuf, currentuser.userid, "µ¥´Ê²éÑ¯½á¹û");
			unlink (filebuf);
		}
		else
			prints("\r\nÃ»ÕÒµ½Õâ¸öµ¥´Ê.");
		if(askyn("¼ÌĞø²éÑ¯ÆäËüµ¥´ÊÂğ", YEA, YEA)!=YEA)
			break;
	}
	clear();
	//	prints("»¶Ó­ÏÂ´Î¹âÁÙ!");
	//	pressanykey();
}

//add by yiyo IP²éÑ¯
#include <netdb.h>
#include <netinet/in.h>
unsigned int free_addr[2000], free_mask[2000], free_num;

search_ip(unsigned int addr)
{
	FILE *fp;
	char buf[512], buf2[80];
	int b1, b2, b3, b4, c1, c2, c3, c4, find=0;
	unsigned int i1=addr/(256*256*256), i2=(addr/65536)%256;
	unsigned int i3=(addr/256)%256, i4=(addr%256);
	//        unsigned int i4=addr/(256*256*256), i3=(addr/65536)%256;
	//        unsigned int i2=(addr/256)%256, i1=(addr%256);
	unsigned int v=i4*65536*256+i3*256*256+i2*256+i1, v1, v2;
	fp=fopen("etc/ip_range.txt", "r");
	if(fp==0)
		return;
	prints("IPµØÖ· [%d.%d.%d.%d]\n", i4, i3, i2, i1);
	while(1)
	{
		buf[0]=0;
		if(fgets(buf, 500, fp)<=0)
			break;
		if(strlen(buf)<10)
			continue;
		sscanf(buf, "%d.%d.%d.%d %d.%d.%d.%d %s",&b1, &b2, &b3, &b4, &c1, &c2, &c3, &c4, buf2);
		v1=b1*65536*256+b2*256*256+b3*256+b4;
		v2=c1*65536*256+c2*256*256+c3*256+c4;
		if(v1<=v && v<=v2)
		{
			prints("¸ÃIPÊôÓÚ: %s\n", buf2);
			find++;
		}
	}
	fclose(fp);
	if(find==0)
		prints("Ã»ÓĞÕÒµ½Æ¥ÅäµÄ¼ÇÂ¼£¡\n");
	else
		prints("Ò»¹²ÕÒµ½ %d ¸öÆ¥Åä¼ÇÂ¼¡£\n", find);
	prints("\n\n\n»¶Ó­Ê¹ÓÃ±¾²éÑ¯ÏµÍ³¡£Èç¹û·¢ÏÖ´íÎóÖ®´¦»òÓĞ¸Ä½øÒâ¼û£¬ÇëÔÚsysop°æ·¢ÎÄ¡£\n");
}

int get_free_list()
{
	FILE *fp;
	char buf1[100], buf2[100], buf3[100];
	free_num= 0;
	fp=fopen("etc/free.txt", "r");
	if (fp== NULL)
		return;
	while(fscanf(fp, "%s%s%s", buf1, buf2, buf3)>0)
	{
		free_addr[free_num]= inet_addr(buf1);
		free_mask[free_num]= inet_addr(buf2);
		free_num++;
	}
	fclose(fp);
}

int is_free(unsigned int x)
{
	int n;
	for (n= 0; n< free_num; n++)
		if(((x^ free_addr[n])| free_mask[n])==free_mask[n])
			return 1;
	return 0;
}

free_ip()
{
	struct hostent *he;
	struct in_addr i;
	char buf1[200];
	int addr;
	report("free_ip lookup");
	get_free_list();
	modify_user_mode(FREEIP);
	while(1)
	{
		clear();
		getdata(3, 0, "ÇëÊäÈëÒª²éÑ¯µØÖ·µÄ[1;32mip[m»òÕß[1;32mÓòÃû[m(Ö±½Ó»Ø³µÍË³ö): ", buf1, 40, DOECHO, YEA);
		if(buf1[0]==0)
			return;
		if ((he = gethostbyname(buf1)) != NULL)
			addr=*(int*) he->h_addr;
		else
			addr=inet_addr(buf1);
		if(addr==-1)
			prints("²»´æÔÚµÄµØÖ·\n\n");
		else
		{
			i.s_addr=addr;
			if(is_free(addr))
				prints("\n\n¸ù¾İCERNET freeip v20010317\n%s(%s) ÊÇÒ»¸öÃâ·Ñip.\n\n", buf1, inet_ntoa(i));
			else
				prints("\n\n¸ù¾İCERNET freeip v20010317\n%s(%s) ²»ÊÇÃâ·Ñip.\n\n", buf1, inet_ntoa(i));
			search_ip(addr);
		}
		pressreturn();
	}
}

//add by yiyo ´ò×ÖÁ·Ï°
int x_tt()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(TT,NA,"bin/tt",buf);
}
//add by yiyo ¶íÂŞË¹·½¿é
int tetris()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(TETRIS,NA,"bin/tetris",buf);
}
//add by yiyo Îå×ÓÆåÈË»ú¶ÔÕ½
int x_five()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(XFIVE,NA,"bin/wzq",buf);
}
//add by yiyo ÃüÁîĞĞ¼ÆËãÆ÷
//int x_calc(){
//     exec_cmd(CALC,NA,"bin/calc","");
//}
//add by yiyo °áÏä×Ó
int x_worker()
{
	exec_cmd(WORKER,NA,"bin/worker","");
}
//add by yiyo ÓÊ±àÇøºÅ²éÑ¯
// for ybqh
struct ybqh
{
	char diqu[50];
	char shengfen[30];
	char youbian[10];
	char quhao[10];
};
//end

int ybqh()
{

	//ÓÊ±àÇøºÅ²éÑ¯£¬programed by mok@smth
	//ÏÖÔÚµÄÊı¾İ²»ÊÇºÜÈ«£¬Ö»ÊÇ¾«È·µ½ÁËµØÇø£¬¶ÔÓÚ¾ßÌåµÄÇø¼¶µÄ×ÊÁÏÃ»ÓĞ
	//Èç¹ûË­ÓĞ¸üÈ«µÄÇëÁªÏµÎÒ£¬»òÕß°ÑÄãÃÇµ±µØµÄÒ»Ğ©×ÊÁÏÌá¹©¸øÎÒ£¬ÎÒÃÇÒ»ÆğÀ´
	//³äÊµÆäÖĞµÄÊı¾İ¡¡

	char content[50],buf[512], buf2[80];
	struct ybqh result[300];   //×î¶à·µ»Ø300Ìõ½á¹û
	int maxlen=22,count=0,cursor=0;
	//	int PAGESIZE=(t_lines - 4);
	char DataFile[]="etc/ybqh.txt";
	int i,thispage,current,needclr=0;
	int ch;
	FILE *fp;

	modify_user_mode( YBQH );
	while(1)
	{
		clear();
		refresh();
		//	prints("»¶Ó­ÄúÊ¹ÓÃ±¾Õ¾µÄÓÊ±àÇøºÅ²éÑ¯\n");
		//	prints("ÈçÓĞ½¨Òé»¶Ó­ÔÚ[1;32;44msysop[m°æÌá³ö\n");
		prints("\n[0;37;40m [0;1;37;40m   [31m¨¨¨¨[0;37;40m\n");
		prints("[0;1;37;40m    [31;41m¨ˆ¨ˆ¨ˆ¨ˆ[1;37;40m             »¶Ó­¹âÁÙ±¾¸ßÉ®ÕÆ¹ÜµÄ£­£­ [1;31m¹ú¼Ê/ÖĞ¹úÓÊ±àÇøºÅ²éÑ¯ÏµÍ³[0m\n");
		prints("[0;1;37;40m    [47m [0;30;47m ::::  [37;40m\n");
		prints("[0;1;37;40m  [0;30;47m¨‚[0;1;37;47m [0;30;47m¨x  ¨x[0;1;37;47m [0;37;40m¨†[0;1;37;40m      ÄãÏë²éÃ´£¿ÅóÓÑ£¬ÄãÒªÊÇÏë²éµÄ»°Äã¾ÍËµ»°Âï£¬Äã²»ËµÎÒÔõÃ´[0;37;40m\n");
		prints("[0;1;37;40m  [0;30;47m¨‚[0;1;37;47m   [0;30;47m©§[0;1;37;47m   [0;37;40m¨†\n");
		prints("[0;1;37;40m    [47m  [31m __[0;30;47m [0;1;37;47m  [40m        ÖªµÀÄãÒªÄØ£¬ËäÈ»ÄãºÜÓĞ³ÏÒâµØ¿´×ÅÎÒ£¬¿ÉÊÇÄã»¹ÊÇÒª¸úÎÒ[0;37;40m\n");
		prints("[0;1;37;40m    [0;30;47m¨[0;5;1;31;47m ¦ñ[0;30;47m ¨[37;40m\n");
		prints("[33m¨[0;1;37;43m  ¨¨¨[41m¨[33m¨w[0;31;40m¨[0;1;37;40m    ËµÄãÏë²éµÄ¡£ÄãÕæµÄÏë²éÂğ£¿ÄÇÄã¾ÍĞ´µ½ÏÂÃæ°É£¡Äã²»ÊÇÕæµÄÏëÒª[0;37;40m\n");
		prints("[30;43m [0;1;37;43m     ¨[41m¨[33m¨w¨w¨w[0;37;40m\n");
		prints("[30;43m [0;1;37;43m   ¨[41m¨[33m¨w¨w¨w¨w[37;40m    °É£¿ÄÑµÀÄãÕæµÄÏëÒª²éÂğ£¿ÄÇÎÒ¾ÍÈÃÄÇºïÍ·°ïÄã°É...[0m\n");
		prints("(programed by mok@smth)\n");
		getdata(16,0,"[1;33mÇë°ÑÒª²éÑ¯µÄÄÚÈİ¸æËßÎÒ[0m£º",content,24,DOECHO,YEA);
		if(!content[0])
			break;

		sprintf(genbuf, "ÓÊ±àÇøºÅ²éÑ¯ %s", content);
		report(genbuf);
		if (strlen(content)>maxlen)
		{
			prints("²éÑ¯µÄÄÚÈİ³¬³¤£¡\n");
			pressanykey();
			return;
			;
		}

		fp=fopen(DataFile,"r");
		if (fp==0)
		{
			prints("´ò¿ªÊı¾İÎÄ¼ş³ö´í! ÈçÓĞÒÉÎÊÇëµ½sysop·¢ÎÄ\n");
			sprintf(genbuf, "ÓÊ±àÇøºÅ²éÑ¯ ´ò¿ª%s³ö´í",DataFile);
			report(genbuf);
			pressanykey();
			return ;
		}
		while(1)
		{
			buf[0]=0;
			if(fgets(buf, 500, fp)<=0)
				break;
			if(strlen(buf)<10)
				continue;
			sscanf(buf,"%s%s%s%s",result[count].diqu,result[count].shengfen,result[count].youbian,result[count].quhao,buf2);
			//		if (strstr(result[count].diqu,content)||strstr(result[count].shengfen,content)||strstr(result[count].youbian,content)||strstr(result[count].quhao,content))¡¡count++;
			// ÉÏĞĞµÄ´úÂë±¨´í£¬¶øÏÂÃæµÄ¿ÉÒÔ£¬²»ÖªµÀÊÇÊ²Ã´Ô­Òò
			if (strstr(result[count].diqu,content))
				count++;
			else if (strstr(result[count].shengfen,content))
				count++;
			else if (strstr(result[count].youbian,content))
				count++;
			else if (strstr(result[count].quhao,content))
				count++;

			if (count>=300)
			{
				prints("²éÑ¯½á¹û¹ı¶à£¬ÇëÓÅ»¯Ò»ÏÂ²éÑ¯Ìõ¼ş\n");
				goto  lab;
			}
		}
		fclose(fp);
		if (count==0)
		{
			prints("\nß×,Á¬ÄÇºïÍ·¶¼Ã»ÓĞÕÒµ½£¡ÈçÒªÌá¹©Êı¾İÇëµ½sysop°æ±¨¸æ\n");
			goto  lab;
		}
		while(1)
		{
			clear();
			refresh();
			move(0,0);
			prints("[0;1;36;44mÌÆÉ®  :[37mÎò¿Õ£¬°ïÕâÎ»Ê§Ö÷²éÒ»ÏÂ%-22s                             [0m",content);
			//move(0,0); prints("                  ²éÑ¯\"[1;31m %s [m\"½á¹û                   \n",content);
			move(1,0);
			prints("[1;46;37mÒÆ¶¯[¡ü¡ı]  Àë¿ª[q,¡û]  ·­Ò³[PGUP PGDN] ÈçÓĞÎÊÌâÇëµ½sysop°æ±¨¸æBUG %s   [m",BBSNAME);
			move(2,0);
			prints(" ±àºÅ  µØ Çø                  Ê¡·İ/¹ú¼Ò      ÓÊ ±à    Çø ºÅ\n");
			thispage=cursor>=(count/PAGESIZE*PAGESIZE)?(count-count/PAGESIZE*PAGESIZE):PAGESIZE;
			for(i=0;(i<thispage)&&(thispage>0);i++)
			{
				move(i+3,0);
				clrtoeol();
				current=i+(cursor/PAGESIZE)*PAGESIZE;
				if (i==cursor%PAGESIZE)
					prints("[0;1;34;47m%4d   %-25s %-10s %-10s %-10s[m\n",
					       current+1,result[current].diqu,result[current].shengfen,result[current].youbian,result[current].quhao);
				else
					prints("%4d   %-25s %-10s %-10s %-10s\n",
					       current+1,result[current].diqu,result[current].shengfen,result[current].youbian,result[current].quhao);
			}
			if (thispage<PAGESIZE && needclr)          //·ÇÂúÆÁÒ³Çå³ı²»ĞèÒªÏÔÊ¾µÄ¼ÇÂ¼,
				for(i=thispage;i<PAGESIZE;i++)
				{
					move(i+3,0);
					clrtoeol();
				}

			move(23,0);
			prints("[0;1;36;44mÎò¿Õ :[37mÊ¦¸µ£¬ÎÒ°ïÕâÎ»Ê§Ö÷ÕÒµ½ÁË%4dÌõĞÅÏ¢                                     [0m",count);

			ch = egetch();
			switch( ch )
			{
			case KEY_UP:
				if (cursor==0)
					needclr=1;
				cursor=(cursor+count-1)%count;
				break;
			case KEY_DOWN:
				if (cursor==count/PAGESIZE*PAGESIZE-1)
					needclr=1;
				cursor=(cursor+1)%count;
				break;
			case KEY_PGUP:
				cursor=(cursor-PAGESIZE)<0?0:(cursor-PAGESIZE);
				break;
			case KEY_PGDN:
			case (' '):
							cursor=(cursor+PAGESIZE)>=count-1?count-1:(cursor+PAGESIZE);
				break;
			case KEY_LEFT:
				case ('q'):
							case Ctrl('c'):
									count=0;
				cursor=0;
				needclr=0;
				goto lab;
			}

		}

		lab:
		if(askyn("¼ÌĞø²éÑ¯ÆäËüÄÚÈİÂğ", YEA, YEA)!=YEA)
			break;
	}
	return;
}

//add by yiyo Ê®´ó°´ÈËÊıÍ³¼Æ
int if_exist_title(char *title)
{
	static struct
	{
		int hash_ip;
		char title[64][60];
	}
	my_posts;
	char buf1[256];
	int n;
	FILE *fp;
	sethomefile(buf1, currentuser.userid, "my_posts");
	fp=fopen(buf1, "r+");
	if(fp==NULL)
		fp=fopen(buf1, "w+");
	if (fread(&my_posts, sizeof(my_posts), 1, fp) ==1)
		for(n=0; n<64; n++)
			if(!strncmp(my_posts.title[n], title, 50))
			{
				fclose(fp);
				return 1;
			}
	my_posts.hash_ip = (my_posts.hash_ip+1) & 63;
	strncpy(my_posts.title[my_posts.hash_ip], title, 50);
	fseek(fp, 0, SEEK_SET);
	fwrite(&my_posts, sizeof(my_posts), 1, fp);
	fclose(fp);
	return 0;
}

//add by yiyo ÏŞÖÆÃ¿ÌìµÇÂ½´ÎÊı
#include <sys/stat.h>
#include <unistd.h>
time_t check_limit(char *file, int value, int limit)
{
	FILE *fp;
	char buf[100];
	int num;
	struct stat buf2;
	if ( strcasecmp(currentuser.userid, "guest") == 0 || HAS_PERM(PERM_SYSOP))
		return 0;
	sethomefile(buf, currentuser.userid, file);
	if((fp = fopen(buf,"r")) == NULL){
		fp = fopen(buf,"w");
		fprintf(fp, "%d", value);
		fclose(fp);
		return 0;
	}
	//ÔİÊ±½â¾ölogincheck 0×Ö½ÚµÄÎÊÌâ
	fseek(fp, 0, SEEK_END);
	if(ftell(fp) == 0){
                char b[STRLEN];
                sprintf(b,"%s zero byte",file);
		securityreport(b);
		fclose(fp);
		unlink(buf);
		return 0;
	}
	fseek(fp, 0, SEEK_SET);
	fscanf(fp,"%d",&num);
	fclose(fp);
	stat(buf, &buf2);

	if(abs(buf2.st_mtime-time(0))>86400)
	{
		unlink(buf);
		return 0;
	}

        time_t tt=buf2.st_mtime+86400;

	if(abs(value-num)>limit)
	       return tt;	
        return 0;
}

int check_post_limit()
{
     
     //comment the following line to enable post limit
     //return 0;

     char buff[255];
     time_t tt=check_limit("postcheck", currentuser.numposts, 100);
     if(tt!=0) //ÏŞÖÆÃ¿¸öÈËÃ¿ÌìÖ»¿ÉÒ»·¢ÎÄ¡¢×ªÔØ72Æª
     {

           if(junkboard()) return 0;

       //    clear();
           security_report("Ê¹ÓÃ¹àË®»ú?",4);
           sprintf(buff,"%s/home/%C/%s/postcheck",
               BBSHOME,toupper(currentuser.userid[0]), currentuser.userid);
            unlink(buff);
         //  move(10,0);

          // prints("´ËÕÊºÅ24Ğ¡Ê±ÄÚ·¢ÎÄ´ÎÊı¹ı¶à, ÇëĞªÒ»Ğª£¬¹ıÒ»¶ÎÊ±¼äÔÙÀ´, ÇëÎğÊ¹ÓÃ¹àË®»ú.\n");
          // getdatestring(tt, NA);
          // prints("»Ö¸´Ê±¼ä£º%s\n", datestring);

          // pressanykey();

          // return 1;
     }
     return 0;
}


//add by yiyo ÅĞ¶ÏipÀ´Ô´ÊÇ·ñÎªËŞÉáÍø
int isdormip(char *ip)
{
	/* if (strstr(ip,"10.10.")==ip || strstr(ip,"10.22.")==ip || strstr(ip,"10.21.")==ip || strstr(ip,"10.13.")== ip)
		return 1;
	else */
		return 1;
}

//added by silencer
/*
int isAllowedIp(char *ip)
{
	if(strstr(ip,"211.102.114.")==ip || strstr(ip,"211.102.112.")==ip || strstr(ip,"61.181.245.238")==ip || strstr(ip,"211.102.110.")==ip || strstr(ip,"10.")==ip || strstr(ip,"202.113.16.")==ip || strstr(ip,"202.113.17.")==ip || strstr(ip,"202.113.18.")==ip || strstr(ip,"202.113.19.")==ip || strstr(ip,"202.113.20.")==ip || strstr(ip,"202.113.21.")==ip || strstr(ip,"202.113.22.")==ip || strstr(ip,"202.113.23.")==ip || strstr(ip,"202.113.24.")==ip || strstr(ip,"202.113.25.")==ip || strstr(ip,"202.113.26.")==ip || strstr(ip,"202.113.27.")==ip || strstr(ip,"202.113.28.")==ip || strstr(ip,"202.113.29.")==ip || strstr(ip,"202.113.30.")==ip || strstr(ip,"202.113.31.")==ip || strstr(ip,"202.113.224.")==ip || strstr(ip,"202.113.225.")==ip || strstr(ip,"202.113.226.")==ip || strstr(ip,"202.113.227.")==ip || strstr(ip,"202.113.228.")==ip || strstr(ip,"202.113.229.")==ip || strstr(ip,"202.113.230.")==ip || strstr(ip,"202.113.231.")==ip || strstr(ip,"202.113.232.")==ip || strstr(ip,"202.113.233.")==ip || strstr(ip,"202.113.234.")==ip || strstr(ip,"202.113.235.")==ip || strstr(ip,"202.113.236.")==ip || strstr(ip,"202.113.237.")==ip || strstr(ip,"202.113.238.")==ip || strstr(ip,"202.113.239.")==ip || strstr(ip,"61.136.13.22")==ip|| strstr(ip,"222.30.55.")==ip|| strstr(ip,"222.30.48.")==ip)
		return 1;
	else
		return 0;
}*/


int file_has_word(char *file, char *word) {
        FILE *fp;
        char buf[256], buf2[256];
        fp=fopen(file, "r");
        if(fp==0) return 0;
        while(1) {
                bzero(buf, 256);
                if(fgets(buf, 255, fp)==0) break;
                sscanf(buf, "%s", buf2);
                if(!strcasecmp(buf2, word)) {
                        fclose(fp);
                        return 1;
                }
        }
        fclose(fp);
        return 0;
}
/*modify by loveni*/
int isAllowedIp(char *ip)
{
	return postable;
}//ÅĞ¶ÏÕâ¸ö±äÁ¿

int checkpostperm(char *ip)
{
	postable=isAllowedIp2(ip);
}//ÉÏÕ¾Ê±ÅĞ¶ÏÊÇ·ñÓĞ·¢ÎÄÈ¨ÏŞ

int isAllowedIp2(char *ip)
{//Ô­À´¼ì²âÊÇ·ñÓĞ·¢ÎÄÈ¨ÏŞµÄº¯Êı
   char tmpbuf[STRLEN];
   static struct stat filestat;
   int life;
   if(HAS_PERM(PERM_XEMPT)) return 1;
   if(HAS_PERM(PERM_CLOAK)) return 1;


   if(file_has_word("vipuser",currentuser.userid)) return 1;

  //add by istef @ 7/1/2005
  //ÍâÍøÒÑÈÏÖ¤ÓÃ»§·¢ÎÄ

   sprintf(tmpbuf,"%s/home/%c/%s/.webauth.done",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
   bzero(&filestat, sizeof(filestat));
   
   if(stat(tmpbuf,&filestat)!=-1) return 1;

   //add by brew 050706
   //ÓÀ6ºÍÓÀ9
   life=compute_user_value(&currentuser);
   if(life==666||life==999)
	return 1;
  // int days=(time(0) - currentuser.firstlogin)/86400;
  // if(days>1096) return 1; 
 /*  if((!strcmp(ip,"202.113.16.116"))||(!strcmp(ip,"192.168.16.116")))
	return 0;*/

    FILE *fp=fopen("ip.allow","r");
    if(fp==NULL) return 1;
    char buf[80];
    char *p;
    while(1){
        if(fgets(buf, 80, fp)<=0) break;
        p=strtok(buf, " \n\t\r");
        if(p==NULL||*p=='#') continue;
        if(strstr(ip,p)==ip||( strstr(p,"/") && ipzone(ip,p)))
	{
           fclose(fp);
           return 1;
        }
    }
    fclose(fp);
    return 0;
}

/*
int isAllowedIp(char *ip)
{
   char tmpbuf[STRLEN];
   static struct stat filestat;
   int life;
   if(HAS_PERM(PERM_XEMPT)) return 1;
   if(HAS_PERM(PERM_CLOAK)) return 1;


   if(file_has_word("vipuser",currentuser.userid)) return 1;

  //add by istef @ 7/1/2005
  //ÍâÍøÒÑÈÏÖ¤ÓÃ»§·¢ÎÄ

   sprintf(tmpbuf,"%s/home/%c/%s/.webauth.done",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
   bzero(&filestat, sizeof(filestat));
   
   if(stat(tmpbuf,&filestat)!=-1) return 1;

   //add by brew 050706
   //ÓÀ6ºÍÓÀ9
   life=compute_user_value(&currentuser);
   if(life==666||life==999)
	return 1;
  // int days=(time(0) - currentuser.firstlogin)/86400;
  // if(days>1096) return 1; 


    FILE *fp=fopen("ip.allow","r");
    if(fp==NULL) return 1;
    char buf[80];
    char *p;
    while(1){
        if(fgets(buf, 80, fp)<=0) break;
        p=strtok(buf, " \n\t\r");
        if(p==NULL||*p=='#') continue;
        if(strstr(ip,p)==ip||( strstr(p,"/") && ipzone(ip,p)))
	{
           fclose(fp);
           return 1;
        }
    }
    fclose(fp);
    return 0;
}*/
int ipzone(char *ip, char *zone)
{
    int start,end,i;
    char s[80];
    char *p=index(zone,'/');
    if(p==NULL)  return 1;
    end=atoi(p+1);
    *p=0;
    p=rindex(zone,'.');
    if(p==NULL)  return 1;
    start=atoi(p+1);
    *p=0;

    for(i=start;i<=end;i++){
       sprintf(s,"%s.%d",zone,i);
       if(strstr(ip,s)==ip) return 1;
    }

    return 0;
}


//add by yiyo IPÓÃ»§²éÑ¯
int ip_user()
{
	FILE	*fp, *fout;
	struct userec aman;
	char    buf[256],ip[16],ipuser[500][20];
	int i,a=0,x;

	modify_user_mode(FREEIP);
	clear();
	getdata(3, 0, "ÇëÊäÈëÒª²éÑ¯µÄIP: ", ip, 16, DOECHO, YEA);
	if(ip[0]=='\0')
		return;
	sprintf(buf, "%s/.PASSWDS", BBSHOME);
	if ((fp = fopen(buf, "rb")) == NULL)
	{
		printf("Can't open record data file.\n");
		return;
	}
	for (i = 0;; i++)
	{
		if (fread(&aman, sizeof(struct userec), 1, fp) <= 0)
			break;
		if(strcmp(aman.lasthost,ip)==0)
			strcpy(ipuser[a++],aman.userid);
	}
	fclose(fp);
	sprintf(buf, "%s/tmp/ip_user", BBSHOME);
	if ((fout = fopen(buf, "w")) == NULL)
	{
		printf("Can't write to badbms file.\n");
		return;
	}
	fprintf(fout, "ÓĞ%d¸öÉÏ´Î´Ó\033[33m%s\033[mµÇÂ½µÄID:\n",a,ip);
	for(x=0;x<a;x++)
	{
		if(x%4==0)
			fprintf(fout, "\n");
		fprintf(fout, "  %-16s",ipuser[x]);
	}
	fprintf(fout, "\n\n");
	fclose(fout);
	ansimore(buf,YEA);
	unlink(buf);
}

//add by bluetent for archie 2003.1.1
void x_excearchie()
{
	char buf[STRLEN],filebuf[STRLEN],genbuf[STRLEN];
	char *s;

	if(heavyload())
	{
		clear();
		prints("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ıÖØ£¬´Ë¹¦ÄÜÔİÊ±²»ÄÜÖ´ĞĞ...");
		pressanykey();
		return;
	}
	modify_user_mode( ARCHIE );
	clear();
	prints("\n[1;33m¨q©¤©¤¨r¨q©¤©¤¨r¨q©¤©¤¨r     ¨q©¤©¤¨r¨q©¤©¤¨r¨q©¤©¤¨r¨q©¤©¤¨r¨q©¤©¤¨r¨q¨r¨q¨r\n");
	prints("©¦  ©¤©È¨t¨r¨q¨s©¦ ¡ğ ©¦     ©¦  ©¤©È©¦  ©¤©È©¦ ¡ğ ©¦©¦ ¡ğ ©¦©¦¨q©¤¨s©¦¨t¨s©¦\n");
	prints("©¦¨q©¤¨s  ©¦©¦  ©¦¨q©¤¨s     ©À©¤  ©¦©¦  ©¤©È©¦¨q¨r©¦©¦¨q¨r©È©¦¨t©¤¨r©¦¨q¨r©¦\n");
	prints("¨t¨s      ¨t¨s  ¨t¨s         ¨t©¤©¤¨s¨t©¤©¤¨s¨t¨s¨t¨s¨t¨s¨t¨s¨t©¤©¤¨s¨t¨s¨t¨s[0m\n");
	prints("\n\n»¶Ó­Ê¹ÓÃ%sFTPËÑË÷ÒıÇæ£¡",BBSNAME);
	prints("\n\n±¾¹¦ÄÜ½«ÎªÄúÁĞ³öÔÚÄÄ¸ö FTP Õ¾´æÓĞÄúÓûÑ°ÕÒµÄÎÄ¼ş£¡");
	prints("\n\nÇëÊäÈëÓûËÑÑ°µÄ×Ö´®£¨ÖÁÉÙÈı¸ö×Ö·û£©, »òÖ±½Ó°´ <ENTER> È¡Ïû¡£");
	getdata(13,0,">",buf,20,DOECHO,YEA);
	if (buf[0]=='\0')
	{
		prints("\nÈ¡ÏûËÑÑ°.....\n");
		pressanykey();
		return;
	}
	if (strlen(buf)<3)
	{
		prints("\nÖÁÉÙÒªÊäÈëÈı¸ö×Ö·û.....\n");
		pressanykey();
		return;
	}
	for(s=buf;*s != '\0';s++)
	{
		if(isspace(*s))
		{
			prints("\nÒ»´ÎÖ»ÄÜËÑÑ°Ò»¸ö×Ö´®À², ²»ÄÜÌ«Ì°ĞÄà¸!!");
			pressanykey();
			return;
		}
	}
	//    exec_cmd( ARCHIE, YEA, "bin/archie.sh", buf );
	sprintf(filebuf,"tmp/archie.%s_%d",currentuser.userid,uinfo.pid);
	sprintf(genbuf, "bin/archie.sh '%s' > %s", buf, filebuf);
	system(genbuf);
	if (dashf(filebuf))
	{
		ansimore( filebuf, YEA);
		if(askyn("Òª½«½á¹û¼Ä»ØĞÅÏäÂğ",NA,NA)==YEA)
			mail_file(filebuf,currentuser.userid,"FTPËÑË÷ÒıÇæËÑÑ°½á¹û");
		unlink (filebuf);
	}
}
//add by bluetent end

int server_info()
{
	// 11:15am  up 56 day(s), 21:11,  7 users,  load average: 0.30, 0.38, 0.43
	FILE *fp;
	char buf[256];
	int day, hour, min;
	int i=0;
	char load[80];
	//	char *ptr;
	system("uptime > tmp/uptime.tmp");
	sprintf(buf, "%s/%s", BBSHOME, "tmp/uptime.tmp");
	if((fp=fopen(buf, "r"))==NULL)
	{
		pressanykey();
		return 0;
	}
	modify_user_mode(XMENU);
	fgets(buf, 256, fp);
	clear();
	getdatestring(time(0), NA);
	day=atoi(buf+13);
	hour=atoi(buf+24);
	min=atoi(buf+27);
	//	ptr=strtok(buf+56, ",");
	/*	for(i=0;i<3;i++)
		{
			if(ptr)
			{
				load[i] = ptr;
				ptr = strtok(NULL,",");
			}
		}*/
	while(strncmp(buf+i,"ge",2))
		i++;
	i+=4;
	strcpy(load,buf+i);
	move(1, 1);
	prints("Ä¿Ç°ÏµÍ³Ê±¼ä£º%s", datestring);
	move(3, 1);
	prints("ÏµÍ³ÒÑÁ¬ĞøÔËĞĞ£º%d Ìì %d Ğ¡Ê± %d ·ÖÖÓ", day, hour, min);
	move(5, 1);
	prints("×î½ü1, 10, 15·ÖÖÓµÄÏµÍ³¸ººÉ·Ö±ğÎª£º%s", load);
	fclose(fp);
	pressanykey();
	return 1;
}

int no_count_post()
{
	char buf[80];
	modify_user_mode(XMENU);
	sprintf(buf, "%s/etc/junkboards", BBSHOME);
	ansimore(buf);
	return 1;
}

int game_killer()
{
	killer_main();
}


int read_hot_info()
{
      char ans[4];
	int rule=0;
	char prompt[160];
       move(t_lines - 1, 0);
       clrtoeol();
	char fname[80];
	sprintf(fname,"boards/%s/boardrule",currboard);
	if (uinfo.mode == READING && dashf(fname)) rule=1;
	if(rule==1) sprintf(prompt,"%s", "Ñ¡Ôñ:  \33[33m1)Ê®´ó»°Ìâ \33[35m2)Ê®´ó×£¸£ \33[36m3)Õ¾ÄÚ¹«¸æ \33[34m4)Ğ£ÄÚ¹«¸æ \33[32m5)ÌìÆøÔ¤±¨ \33[31m6)±¾°å°å¹æ;\33[m [1]:");
	else sprintf(prompt,"%s", "Ñ¡Ôñ: \33[33m1)Ê®´ó»°Ìâ \33[35m2)Ê®´ó×£¸£ \33[36m3)Õ¾ÄÚ¹«¸æ \33[34m4)Ğ£ÄÚ¹«¸æ \33[32m5)ÌìÆøÔ¤±¨\33[m [1]:");
    getdata(t_lines - 1, 0, prompt, ans, 3, DOECHO, YEA);
    switch (ans[0])
	{
    	case '2':
             show_help("etc/bless/day");
        break;
    case '3':
		show_help("etc/bbsannounce");
        break;
    case '4':
		show_help("etc/campus");
        break;
    case '5':
             show_help("0Announce/weather/index");
        break;

	case '6':
		if(rule==1) show_help (fname);
	  break;
	case '1':
	default:
		show_topten("etc/posts/day");
		//show_help("etc/posts/day");
    }
    return FULLUPDATE;
}//loveni:×£¸£¹«¸æ¹¦ÄÜ




void CheckBmCloak()
{
	char fname[STRLEN];
	char secu[STRLEN];
	if(file_has_word("etc/specbm",currentuser.userid)) return ;
	if(HAS_PERM( PERM_SYSOP )) return;
	if(!(currentuser.flags[0] & CLOAK_FLAG)) return;
	if(!dashf("etc/BMCLOAK")) return;
	if( HAS_PERM( PERM_CLOAK )  && HAS_PERM( PERM_BOARDS ))
	{
		sprintf(fname, "home/%c/%s/warning", toupper(currentuser.userid[0]), currentuser.userid);
		if(time(0)-currentuser.lastlogin>7*24*60*60)//Èç¹ûÁ¬ĞøÒşÉí³¬¹ı7Ìì 
		{
			if(dashf(fname))
			{
				prints("¶Ô²»Æğ£¬ÓÉÓÚ×÷Îª°åÎñµÄÄúÁ¬ĞøÒşÉíÊ±¼äÌ«³¤,ÊÕ»ØÄúÒşÉíÈ¨ÏŞ¡£Çëµ½famehallÉêÇë\n");
				
				prints("                   Èç¹ûÓĞÆäËûÎÊÌâ£¬ÇëÓÚÏµÍ³Î¬»¤ÁªÏµ\n");
				
				currentuser.userlevel &= ~PERM_CLOAK;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				
				sprintf(secu, "Òò°åÎñ%s³¤ÆÚÒşÉíÈ¡ÏûÒşÉíÈ¨ÏŞ", currentuser.userid);
                      		security_report(secu,1);
				unlink(fname);
				pressanykey();
			}
			else 
			{
				prints("              Warning:Èç¹ûÁ¬Ğø°åÎñÒşÉíÊ±¼äÌ«³¤¿ÉÄÜ¶ªÊ§ÒşÉíÈ¨ÏŞ\n");
				pressanykey();
				FILE * fp;
				fp=fopen(fname,"w");
				fprintf(fp,"%s",currentuser.userid);
				fclose(fp);
			}
		}
		else 
			if(time(0)-currentuser.lastlogin>6*24*60*60)//Èç¹ûÁ¬ĞøÒşÉí³¬¹ı6Ìì
			{
				if(dashf("etc/WarningBM"))
				{
				prints("              Warning:Èç¹ûÁ¬Ğø°åÎñÒşÉíÊ±¼äÌ«³¤¿ÉÄÜ¶ªÊ§ÒşÉíÈ¨ÏŞ\n");
				pressanykey();
				}
				unlink(fname);
			}
			else unlink(fname);
	}
}

int ReleaseEmail(void)
{
        char username[80];
        char email[80];
        char ip[80];
        char userid[80];
        int len=180;
        char fname[80];
        char genbuf[80];
        char *fileinfo=NULL;
        int find=0;
        int i;
        char findid[15];
        FILE * fp;
	modify_user_mode(USERDEF);
	strtolower(username,currentuser.userid);
        for (i='A';i<='Z'&&(find==0);i++)
        {
                sprintf(fname,"/home/bbs/mail/%c/email.auth",i);
                if(dashf(fname))
                {   
                        fp=fopen(fname,"r");
                        while (getline(&fileinfo, &len, fp)!= -1 &&find==0)
                        {
                                strcpy(email,strtok(fileinfo,","));
                                strcpy(userid,strtok(NULL,","));
                                strcpy(ip,strtok(NULL,","));
                                strtolower(findid,userid);
                                if(!strcmp(findid,username))
                                {
                                        find=1;
                                        move(2,0);
                                        clrtobot ();
                                        sprintf(genbuf,"ÓÃ»§id:%s\n",userid);
                                        prints(genbuf);
                                        sprintf(genbuf,"ÓÃ»§ÈÏÖ¤Email:%s\n",email);
                                        prints(genbuf);
                                        sprintf(genbuf,"ÓÃ»§ÈÏÖ¤ip:%s\n",ip);
                                        prints(genbuf);
                                }
                        }
                }
	}
	if (fileinfo) 
		free(fileinfo);			
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("¶Ô²»Æğ£¬ÄúÃ»ÓĞ°ó¶¨ÓÊÏä£¡");
		pressanykey();
		return 0;
	}
	prints("¾¯¸æ£º½â³ıÓÊÏä°ó¶¨¿ÉÄÜµ¼ÖÂ²»ÄÜÍâÍø·¢ÎÄ£¡");
	if(askyn("ÄãÈ·¶¨Òª½â³ıÓÊÏä°ó¶¨£¿",NA,NA)){
		sprintf(genbuf, "mail/%c/email.auth",
			toupper(GetAuthUserMailF(currentuser.userid)));
		del_authrow_has_userid(genbuf, currentuser.userid);
		strcat(genbuf, ".U");	
		del_authrow_has_userid(genbuf, currentuser.userid);
		sprintf(genbuf, "%s/home/%c/%s/.webauth.done", BBSHOME,
			toupper(currentuser.userid[0]),currentuser.userid);
		if(dashf(genbuf))	
			unlink(genbuf);
		sprintf(genbuf,"%s Release Email %s",currentuser.userid, email);
		report(genbuf);
	}
	return 1;
}

int IsInternalUser(char * ip)
{
	if(!dashf("NOPOST.external")) return 1;
	if(file_has_word("vipuser",currentuser.userid)) return 1;
	if((!strcmp(ip,"202.113.16.116"))||(!strcmp(ip,"192.168.16.116")))
        return 0;

	FILE *fp=fopen("ip.allow","r");
	if(fp==NULL) return 1;
	char buf[80];
	char *p;
	while(1)
	{
		if(fgets(buf, 80, fp)<=0) break;
		p=strtok(buf, " \n\t\r");
		if(p==NULL||*p=='#') continue;
		if(strstr(ip,p)==ip||( strstr(p,"/") && ipzone(ip,p)))
		{
		   fclose(fp);
	   	return 1;
		}
	}
	fclose(fp);
	return 0;
}

int syncuserdata(){
	int i;
	char uident[STRLEN];
	char passbuf[PASSLEN];
	char filetosync[14][STRLEN] = {
	    ".boardrc",
	    "friends",
	    ".goodbrd",
	    "badhost",
	    "GoodWish",
	    ".lastread",
	    "maillist*",
	    "plans",
	    "signatures",
	    "bbsnet",
	    "deny_user",
	    "rejects",
	    "logout",
	    "notes"
	};
	modify_user_mode(USERDEF);
	move(1, 0);
	clrtobot();
	prints("Í¬²½ËûÈËÊÕ²Ø¼Ğ¡¢ºÃÓÑÃûµ¥¡¢»µÈËÃûµ¥¡¢¾ÜÊÕÓÊ¼şÃûµ¥¡¢\n");
	prints("ÈºĞÅÃûµ¥¡¢Î´¶Á±ê¼Ç¡¢ËµÃ÷µµ¡¢Ç©Ãûµµ¡¢±¸ÍüÂ¼¡¢¶¨Ê±±¸ÍüÂ¼¡¢\n");
	prints("ÀëÕ¾»­Ãæ¡¢µ×²¿Á÷¶¯ĞÅÏ¢¡¢µÇÂ¼µØÖ·ÏŞÖÆ¡¢×ÔÑ¡´©ËóÕ¾µã.\n");
	prints("»áµ¼ÖÂ±¾IDÔ­ÓĞµÄÏàÓ¦ĞÅÏ¢¶ªÊ§£¬ÇëÉ÷ÓÃ£¡\n");
	prints("\33[1;31mÊäÈëËûÈËÃÜÂë´íÎó¼ÇÂ¼»áÔÚÆäµÇÂ¼Ê±¿´µ½£¬ÇëÎğÔÚ´ËÊÔÌ½ËûÈËÃÜÂë£¡\n\33[0m");
	prints("ÇëÊäÈëÒªËÑË÷µÄÓÃ»§ÕÊºÅ(×Ô¶¯ËÑË÷)£º");
	move(6,34);
	usercomplete(NULL,uident);
	if(uident[0] == '\n' || uident[0] == 0)
	    return 0;
	if(!strcasecmp(currentuser.userid, uident))
	{   
	    move(10,20);
	    prints("¿ªÊ²Ã´ÍæĞ¦£¬×Ô¼ºµÄÊı¾İ»¹ÒªÍ¬²½£¿");
	    pressanykey();
	    return 1;
	}
	if (getuser(uident) == 0 || strcasecmp(lookupuser.userid, uident))
	{
	    move(10,20);
	    prints("ÓÃ»§Î´ÕÒµ½!");
	    pressanykey();
	    return 1;
	}
	sprintf(genbuf,"\33[1;37mÇëÊäÈë%sµÄÃÜÂë: \33[m", uident);
	getdata(7,0,genbuf,passbuf,PASSLEN,NOECHO,YEA);
	passbuf[8] = '\0';
	if (!checkpasswd(lookupuser.passwd, passbuf))
	{
	    logattempt(uident, currentuser.userid, passbuf);
	    move(10,20);
	    prints("\33[1;31mÃÜÂëÊäÈë´íÎó (Password Error)...\33[m\n");
	    pressanykey();
	    return 1;
	}
	for(i = 0;i < 14; i ++)
	{
	    sprintf(genbuf,"cp -fr %s/home/%c/%s/%s %s/home/%c/%s/", 
		    BBSHOME,toupper(uident[0]), uident, filetosync[i],
		    BBSHOME, toupper(currentuser.userid[0]), currentuser.userid);
	    system(genbuf);
	}
	move(10,20);
	prints("\33[1;32mÍ¬²½³É¹¦!\33[0m");
	pressanykey();
	return 0;
}


