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
int postable=0;//如果是1有外网发文权限

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
		return 0; /* add by yiyo 必须减少update_ulist的次数. */
	uinfo.mode = mode;
	update_ulist(&uinfo, utmpent);
	return 0;
}

int showperminfo(int pbits, int i)
{
	char    buf[STRLEN];
	//	sprintf(buf, "%c. %-30s %2s", 'A' + i, (use_define) ? user_definestr[i] : permstrings[i],
	//		((pbits >> i) & 1 ? "是" : "×"));
	sprintf(buf, "%c. %-30s %2s", permstr[i], (use_define) ? user_definestr[i] : permstrings[i],
	        ((pbits >> i) & 1 ? "是" : "×"));
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
	prints("请按下你要的代码来设定%s，按 Enter 结束.\n", prompt);
	move(6, 0);
	clrtobot();
	for (i = 0; i <= lastperm; i++)
	{
		(*showfunc) (pbits, i, NA);
	}
	while (!done)
	{
		sprintf(buf, "选择(ENTER 结束%s): ", ((strcmp(prompt, "权限") != 0)) ? "" : "，0 停权");
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
			prints("您没有权限更改此项");	
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
		prints("错误的使用者 ID...");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	use_define = 1;
	newlevel = setperms(lookupuser.userdefine, "参数", NUMDEFINES, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userdefine)
		prints("参数没有修改...\n");
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
		prints("新的参数设定完成...\n\n");
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
		prints("隐身术 (cloak) 已经%s了!",
		       (uinfo.invisible) ? "启动" : "停止");
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
	    {"个人说明档", "签名档", "自己的备忘录", "离站的画面","底部流动信息","登录地址限制","自选穿梭站点", NULL};
	modify_user_mode(GMENU);
	clear();
	move(1, 0);
	prints("编修个人档案\n\n");
	for (num = 0; e_file[num] != NULL && explain_file[num] != NULL; num++)
	{
		prints("[[1;32m%d[m] %s\n", num + 1, explain_file[num]);
	}
	prints("[[1;32m%d[m] 都不想改\n", num + 1);

	getdata(num + 5, 0, "你要编修哪一项个人档案: ", ans, 2, DOECHO, YEA);
	if (ans[0] - '0' <= 0 || ans[0] - '0' > num || ans[0] == '\n' || ans[0] == '\0')
		return;

	ch = ans[0] - '0' - 1;
	setuserfile(genbuf, e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)编辑 (D)删除 %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		confirm = askyn("你确定要删除这个档案", NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("取消删除行动\n");
			pressreturn();
			clear();
			return;
		}
		unlink(genbuf);
		move(5, 0);
		prints("%s 已删除\n", explain_file[ch]);
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
		prints("%s 更新过\n", explain_file[ch]);
		sprintf(buf, "edit %s", explain_file[ch]);
		if (!strcmp(e_file[ch], "signatures"))
		{
			set_numofsig();
			prints("系统重新设定以及读入你的签名档...");
		}
		report(buf);
	}
	else
		prints("%s 取消修改\n", explain_file[ch]);
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
		prints("错误的使用者代号");
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
		prints("不正确的讨论区名称");
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
		prints ("        faint...guest也想屏幕锁定？!!");
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
	//tdhlshx for 萤幕锁定 03.09.04.
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
				getdatestring(now,NA);//增加,修补显示的bug.
				prints("[1;36m萤幕已在[33m %s[36m 时被[32m %-12s [36m暂时锁住了...[m", datestring, currentuser.userid);
			//}
			move(20, 0);
			clrtobot();
			update_endline();
			//      move(19,0);
			//      clrtoeofl();
			getdata(20, 0, "请输入你的密码以解锁;按回车更换锁定画面:", buf, PASSLEN, NOECHO, YEA);
		}

	}
	else
	{//防止美工删除后没有建立lock文件
		move(9,0);		
		prints("系统读取锁定画面文件出现错误,请报告本站系统维护,谢谢!\n");
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
		prints("\n[1;36m萤幕已在[33m %s[36m 时被[32m %-12s [36m暂时锁住了...[m", datestring, currentuser.userid);
		while (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
		{
			move(18, 0);
			clrtobot();
			update_endline();
			getdata(19, 0, "请输入你的密码以解锁: ", buf, PASSLEN, NOECHO, YEA);
		}

	}
	/*while (*buf == '\0' || !checkpasswd(currentuser.passwd, buf)) {
	               if(*buf='\n')
		{
			num=(num+1)%locknumall;
			move(9,0);
			clrtobot();
			print_file("etc/lock",num*10+1,10);
			getdatestring(now,NA);//增加,修补显示的bug.
			prints("[1;36m萤幕已在[33m %s[36m 时被[32m %-12s [36m暂时锁住了...[m", datestring, currentuser.userid);
					
		}		
		move(20, 0);//注释
		clrtobot();//注释掉
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
/*void get_lockpic(char * filename1,int num,int numline,char * filename2)//将文件1的一N1行开始,N行写入文件2
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
	prints ("[1;31m以下是本站禁用词汇，发文请勿使用！[m");
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
		prints("太多人使用外部程式了，你等一下再用吧...");
		pressanykey();
		return;
	}
	if (!HAS_PERM(PERM_SYSOP) && heavyload())
	{
		clear();
		prints("抱歉，目前系统负荷过重，此功能暂时不能执行...");
		pressanykey();
		return;
	}
	if (!dashf(cmdfile))
	{
		prints("文件 [%s] 不存在！\n", cmdfile);
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
		prints("资源紧缺，fork() 失败，请稍后再使用");
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
	stand_title("本站使用者资料查询");
	ansimore("etc/showuser.msg", NA);
	getdata(20, 0, "Parameter: ", buf, 30, DOECHO, YEA);
	if ((buf[0] == '\0') || dashf("tmp/showuser.result"))
		return;
	securityreport("查询使用者资料");
	exec_cmd(SYSINFO, YEA, "bin/showuser", buf);
	sprintf(buf, "tmp/showuser.result");
	if (dashf(buf))
	{
		mail_file(buf, currentuser.userid, "使用者资料查询结果");
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

/* add by bluetent 自选穿梭 2003.01.18 */
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
		prints("您尚未设置自选穿梭列表，请进入“编修个人档案”选单修改\n");
		prints("以行为单位,格式如下例示:\n");
		prints("    站点说明    站点名称  站点地址\n");
		prints("    南开人社区   %s  %s\n",BBSNAME,BBSHOST);
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
	       + 86400;	/* 算出今天 0:0:00 的时间, 然後再往後加一天 */
	sprintf(genbuf, "纪念日更新, 下一次更新时间 %s", Cdate(&next));
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
	prints("[0;1;32m留言本[m\n您可以在这里给您的朋友送去您的祝福，");
	prints("\n也可以为您给他/她捎上一句悄悄话。");
	move(5,0);
	if(userid == NULL)
	{
		usercomplete("请输入他的 ID: ", uid);
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
		prints("[1m您输入的使用者代号( ID )不存在！[m\n");
		pressanykey();
		clear();
		return -1;
	}
	if(!strcmp(uid,"guest"))
	{
		 move(7, 0);
		 prints("不要给guest留言...\n");
		 pressanykey();
		 clear();
		 return -1;		 
	}
	move(5,0);
	clrtoeol();
	prints("[m【给 [1m%s[m 留言】", uid);
	move(6,0);
	prints("您的留言[直接按 ENTER 结束留言，最多 5 句，每句最长 50 字符]:");
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
	sprintf(genbuf, "你确定要发送这条留言给 [1m%s[m 吗", uid);
	move(9+count, 0);
	if (askyn(genbuf, YEA, NA) == NA)
	{
		clear();
		return 0;
	}
	sethomefile(genbuf,uid,"GoodWish");
	if ((fp = fopen(genbuf, "a")) == NULL)
	{
		prints("无法开启该用户的底部流动信息文件，请通知站长...\n");
		pressanykey();
		return NA;
	}
	now = time(0);
	timestr = ctime(&now) + 11;
	*(timestr + 5) = '\0';
	for(i = 0; i< count ; i++)
	{
		fprintf(fp, "%s(%s)[%d/%d]：%s\n",
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
	prints("已经帮您送出您的留言了。");
	sprintf(genbuf,"SendGoodWish to %s",uid);
	report(genbuf);
	pressanykey();
	clear();
	return 0;
}

int getdatestring( time_t now, int Chang)
{
	struct tm *tm;
	char weeknum[7][3]={"天","一","二","三","四","五","六"};

	tm = localtime(&now);
	sprintf(datestring,"%4d年%02d月%02d日%02d:%02d:%02d 星期%2s",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
	        tm->tm_hour,tm->tm_min,tm->tm_sec,
	        weeknum[tm->tm_wday]);
	return (tm->tm_sec % 10);
}
/* Added End. */

//add by yiyo 中英文字典
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
		prints("本字典 [1;33mCDICT5.0[m 收录单词近 [1;32m40000[m , 一般较常见的单词和缩写均已收录,\n");
		prints("但未收录词组以及各类专业词汇. 中英文输入均可, 暂不支持'[1;32m*[m'后缀.\r\n\r\n");
		getdata(3,0,"请输入要查询的中英文单词: ",buf,24,DOECHO,YEA);
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
			prints("错误的输入!\n");
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
			if(askyn("要将查询结果寄回到信箱吗", NA, NA)==YEA)
				mail_file(filebuf, currentuser.userid, "单词查询结果");
			unlink (filebuf);
		}
		else
			prints("\r\n没找到这个单词.");
		if(askyn("继续查询其它单词吗", YEA, YEA)!=YEA)
			break;
	}
	clear();
	//	prints("欢迎下次光临!");
	//	pressanykey();
}

//add by yiyo IP查询
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
	prints("IP地址 [%d.%d.%d.%d]\n", i4, i3, i2, i1);
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
			prints("该IP属于: %s\n", buf2);
			find++;
		}
	}
	fclose(fp);
	if(find==0)
		prints("没有找到匹配的记录！\n");
	else
		prints("一共找到 %d 个匹配记录。\n", find);
	prints("\n\n\n欢迎使用本查询系统。如果发现错误之处或有改进意见，请在sysop版发文。\n");
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
		getdata(3, 0, "请输入要查询地址的[1;32mip[m或者[1;32m域名[m(直接回车退出): ", buf1, 40, DOECHO, YEA);
		if(buf1[0]==0)
			return;
		if ((he = gethostbyname(buf1)) != NULL)
			addr=*(int*) he->h_addr;
		else
			addr=inet_addr(buf1);
		if(addr==-1)
			prints("不存在的地址\n\n");
		else
		{
			i.s_addr=addr;
			if(is_free(addr))
				prints("\n\n根据CERNET freeip v20010317\n%s(%s) 是一个免费ip.\n\n", buf1, inet_ntoa(i));
			else
				prints("\n\n根据CERNET freeip v20010317\n%s(%s) 不是免费ip.\n\n", buf1, inet_ntoa(i));
			search_ip(addr);
		}
		pressreturn();
	}
}

//add by yiyo 打字练习
int x_tt()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(TT,NA,"bin/tt",buf);
}
//add by yiyo 俄罗斯方块
int tetris()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(TETRIS,NA,"bin/tetris",buf);
}
//add by yiyo 五子棋人机对战
int x_five()
{
	char buf[80];
	sprintf(buf,"%s %s",currentuser.userid,currentuser.lasthost);
	exec_cmd(XFIVE,NA,"bin/wzq",buf);
}
//add by yiyo 命令行计算器
//int x_calc(){
//     exec_cmd(CALC,NA,"bin/calc","");
//}
//add by yiyo 搬箱子
int x_worker()
{
	exec_cmd(WORKER,NA,"bin/worker","");
}
//add by yiyo 邮编区号查询
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

	//邮编区号查询，programed by mok@smth
	//现在的数据不是很全，只是精确到了地区，对于具体的区级的资料没有
	//如果谁有更全的请联系我，或者把你们当地的一些资料提供给我，我们一起来
	//充实其中的数据　

	char content[50],buf[512], buf2[80];
	struct ybqh result[300];   //最多返回300条结果
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
		//	prints("欢迎您使用本站的邮编区号查询\n");
		//	prints("如有建议欢迎在[1;32;44msysop[m版提出\n");
		prints("\n[0;37;40m [0;1;37;40m   [31m◣◢◣◢[0;37;40m\n");
		prints("[0;1;37;40m    [31;41m▓▓▓▓[1;37;40m             欢迎光临本高僧掌管的－－ [1;31m国际/中国邮编区号查询系统[0m\n");
		prints("[0;1;37;40m    [47m [0;30;47m ::::  [37;40m\n");
		prints("[0;1;37;40m  [0;30;47m▊[0;1;37;47m [0;30;47m▁  ▁[0;1;37;47m [0;37;40m▎[0;1;37;40m      你想查么？朋友，你要是想查的话你就说话嘛，你不说我怎么[0;37;40m\n");
		prints("[0;1;37;40m  [0;30;47m▊[0;1;37;47m   [0;30;47m┃[0;1;37;47m   [0;37;40m▎\n");
		prints("[0;1;37;40m    [47m  [31m __[0;30;47m [0;1;37;47m  [40m        知道你要呢，虽然你很有诚意地看着我，可是你还是要跟我[0;37;40m\n");
		prints("[0;1;37;40m    [0;30;47m◣[0;5;1;31;47m ︸[0;30;47m ◢[37;40m\n");
		prints("[33m◢[0;1;37;43m  ◥◣◢[41m◤[33m╳[0;31;40m◣[0;1;37;40m    说你想查的。你真的想查吗？那你就写到下面吧！你不是真的想要[0;37;40m\n");
		prints("[30;43m [0;1;37;43m     ◢[41m◤[33m╳╳╳[0;37;40m\n");
		prints("[30;43m [0;1;37;43m   ◢[41m◤[33m╳╳╳╳[37;40m    吧？难道你真的想要查吗？那我就让那猴头帮你吧...[0m\n");
		prints("(programed by mok@smth)\n");
		getdata(16,0,"[1;33m请把要查询的内容告诉我[0m：",content,24,DOECHO,YEA);
		if(!content[0])
			break;

		sprintf(genbuf, "邮编区号查询 %s", content);
		report(genbuf);
		if (strlen(content)>maxlen)
		{
			prints("查询的内容超长！\n");
			pressanykey();
			return;
			;
		}

		fp=fopen(DataFile,"r");
		if (fp==0)
		{
			prints("打开数据文件出错! 如有疑问请到sysop发文\n");
			sprintf(genbuf, "邮编区号查询 打开%s出错",DataFile);
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
			//		if (strstr(result[count].diqu,content)||strstr(result[count].shengfen,content)||strstr(result[count].youbian,content)||strstr(result[count].quhao,content))　count++;
			// 上行的代码报错，而下面的可以，不知道是什么原因
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
				prints("查询结果过多，请优化一下查询条件\n");
				goto  lab;
			}
		}
		fclose(fp);
		if (count==0)
		{
			prints("\n咦,连那猴头都没有找到！如要提供数据请到sysop版报告\n");
			goto  lab;
		}
		while(1)
		{
			clear();
			refresh();
			move(0,0);
			prints("[0;1;36;44m唐僧  :[37m悟空，帮这位失主查一下%-22s                             [0m",content);
			//move(0,0); prints("                  查询\"[1;31m %s [m\"结果                   \n",content);
			move(1,0);
			prints("[1;46;37m移动[↑↓]  离开[q,←]  翻页[PGUP PGDN] 如有问题请到sysop版报告BUG %s   [m",BBSNAME);
			move(2,0);
			prints(" 编号  地 区                  省份/国家      邮 编    区 号\n");
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
			if (thispage<PAGESIZE && needclr)          //非满屏页清除不需要显示的记录,
				for(i=thispage;i<PAGESIZE;i++)
				{
					move(i+3,0);
					clrtoeol();
				}

			move(23,0);
			prints("[0;1;36;44m悟空 :[37m师傅，我帮这位失主找到了%4d条信息                                     [0m",count);

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
		if(askyn("继续查询其它内容吗", YEA, YEA)!=YEA)
			break;
	}
	return;
}

//add by yiyo 十大按人数统计
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

//add by yiyo 限制每天登陆次数
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
	//暂时解决logincheck 0字节的问题
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
     if(tt!=0) //限制每个人每天只可一发文、转载72篇
     {

           if(junkboard()) return 0;

       //    clear();
           security_report("使用灌水机?",4);
           sprintf(buff,"%s/home/%C/%s/postcheck",
               BBSHOME,toupper(currentuser.userid[0]), currentuser.userid);
            unlink(buff);
         //  move(10,0);

          // prints("此帐号24小时内发文次数过多, 请歇一歇，过一段时间再来, 请勿使用灌水机.\n");
          // getdatestring(tt, NA);
          // prints("恢复时间：%s\n", datestring);

          // pressanykey();

          // return 1;
     }
     return 0;
}


//add by yiyo 判断ip来源是否为宿舍网
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
}//判断这个变量

int checkpostperm(char *ip)
{
	postable=isAllowedIp2(ip);
}//上站时判断是否有发文权限

int isAllowedIp2(char *ip)
{//原来检测是否有发文权限的函数
   char tmpbuf[STRLEN];
   static struct stat filestat;
   int life;
   if(HAS_PERM(PERM_XEMPT)) return 1;
   if(HAS_PERM(PERM_CLOAK)) return 1;


   if(file_has_word("vipuser",currentuser.userid)) return 1;

  //add by istef @ 7/1/2005
  //外网已认证用户发文

   sprintf(tmpbuf,"%s/home/%c/%s/.webauth.done",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
   bzero(&filestat, sizeof(filestat));
   
   if(stat(tmpbuf,&filestat)!=-1) return 1;

   //add by brew 050706
   //永6和永9
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
  //外网已认证用户发文

   sprintf(tmpbuf,"%s/home/%c/%s/.webauth.done",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
   bzero(&filestat, sizeof(filestat));
   
   if(stat(tmpbuf,&filestat)!=-1) return 1;

   //add by brew 050706
   //永6和永9
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


//add by yiyo IP用户查询
int ip_user()
{
	FILE	*fp, *fout;
	struct userec aman;
	char    buf[256],ip[16],ipuser[500][20];
	int i,a=0,x;

	modify_user_mode(FREEIP);
	clear();
	getdata(3, 0, "请输入要查询的IP: ", ip, 16, DOECHO, YEA);
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
	fprintf(fout, "有%d个上次从\033[33m%s\033[m登陆的ID:\n",a,ip);
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
		prints("抱歉，目前系统负荷过重，此功能暂时不能执行...");
		pressanykey();
		return;
	}
	modify_user_mode( ARCHIE );
	clear();
	prints("\n[1;33m╭──╮╭──╮╭──╮     ╭──╮╭──╮╭──╮╭──╮╭──╮╭╮╭╮\n");
	prints("│  ─┤╰╮╭╯│ ○ │     │  ─┤│  ─┤│ ○ ││ ○ ││╭─╯│╰╯│\n");
	prints("│╭─╯  ││  │╭─╯     ├─  ││  ─┤│╭╮││╭╮┤│╰─╮│╭╮│\n");
	prints("╰╯      ╰╯  ╰╯         ╰──╯╰──╯╰╯╰╯╰╯╰╯╰──╯╰╯╰╯[0m\n");
	prints("\n\n欢迎使用%sFTP搜索引擎！",BBSNAME);
	prints("\n\n本功能将为您列出在哪个 FTP 站存有您欲寻找的文件！");
	prints("\n\n请输入欲搜寻的字串（至少三个字符）, 或直接按 <ENTER> 取消。");
	getdata(13,0,">",buf,20,DOECHO,YEA);
	if (buf[0]=='\0')
	{
		prints("\n取消搜寻.....\n");
		pressanykey();
		return;
	}
	if (strlen(buf)<3)
	{
		prints("\n至少要输入三个字符.....\n");
		pressanykey();
		return;
	}
	for(s=buf;*s != '\0';s++)
	{
		if(isspace(*s))
		{
			prints("\n一次只能搜寻一个字串啦, 不能太贪心喔!!");
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
		if(askyn("要将结果寄回信箱吗",NA,NA)==YEA)
			mail_file(filebuf,currentuser.userid,"FTP搜索引擎搜寻结果");
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
	prints("目前系统时间：%s", datestring);
	move(3, 1);
	prints("系统已连续运行：%d 天 %d 小时 %d 分钟", day, hour, min);
	move(5, 1);
	prints("最近1, 10, 15分钟的系统负荷分别为：%s", load);
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
	if(rule==1) sprintf(prompt,"%s", "选择:  \33[33m1)十大话题 \33[35m2)十大祝福 \33[36m3)站内公告 \33[34m4)校内公告 \33[32m5)天气预报 \33[31m6)本板板规;\33[m [1]:");
	else sprintf(prompt,"%s", "选择: \33[33m1)十大话题 \33[35m2)十大祝福 \33[36m3)站内公告 \33[34m4)校内公告 \33[32m5)天气预报\33[m [1]:");
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
}//loveni:祝福公告功能




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
		if(time(0)-currentuser.lastlogin>7*24*60*60)//如果连续隐身超过7天 
		{
			if(dashf(fname))
			{
				prints("对不起，由于作为板务的您连续隐身时间太长,收回您隐身权限。请到famehall申请\n");
				
				prints("                   如果有其他问题，请于系统维护联系\n");
				
				currentuser.userlevel &= ~PERM_CLOAK;
				substitute_record(PASSFILE, &currentuser, sizeof(struct userec),usernum);
				
				sprintf(secu, "因板务%s长期隐身取消隐身权限", currentuser.userid);
                      		security_report(secu,1);
				unlink(fname);
				pressanykey();
			}
			else 
			{
				prints("              Warning:如果连续板务隐身时间太长可能丢失隐身权限\n");
				pressanykey();
				FILE * fp;
				fp=fopen(fname,"w");
				fprintf(fp,"%s",currentuser.userid);
				fclose(fp);
			}
		}
		else 
			if(time(0)-currentuser.lastlogin>6*24*60*60)//如果连续隐身超过6天
			{
				if(dashf("etc/WarningBM"))
				{
				prints("              Warning:如果连续板务隐身时间太长可能丢失隐身权限\n");
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
                                        sprintf(genbuf,"用户id:%s\n",userid);
                                        prints(genbuf);
                                        sprintf(genbuf,"用户认证Email:%s\n",email);
                                        prints(genbuf);
                                        sprintf(genbuf,"用户认证ip:%s\n",ip);
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
		prints("对不起，您没有绑定邮箱！");
		pressanykey();
		return 0;
	}
	prints("警告：解除邮箱绑定可能导致不能外网发文！");
	if(askyn("你确定要解除邮箱绑定？",NA,NA)){
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
	prints("同步他人收藏夹、好友名单、坏人名单、拒收邮件名单、\n");
	prints("群信名单、未读标记、说明档、签名档、备忘录、定时备忘录、\n");
	prints("离站画面、底部流动信息、登录地址限制、自选穿梭站点.\n");
	prints("会导致本ID原有的相应信息丢失，请慎用！\n");
	prints("\33[1;31m输入他人密码错误记录会在其登录时看到，请勿在此试探他人密码！\n\33[0m");
	prints("请输入要搜索的用户帐号(自动搜索)：");
	move(6,34);
	usercomplete(NULL,uident);
	if(uident[0] == '\n' || uident[0] == 0)
	    return 0;
	if(!strcasecmp(currentuser.userid, uident))
	{   
	    move(10,20);
	    prints("开什么玩笑，自己的数据还要同步？");
	    pressanykey();
	    return 1;
	}
	if (getuser(uident) == 0 || strcasecmp(lookupuser.userid, uident))
	{
	    move(10,20);
	    prints("用户未找到!");
	    pressanykey();
	    return 1;
	}
	sprintf(genbuf,"\33[1;37m请输入%s的密码: \33[m", uident);
	getdata(7,0,genbuf,passbuf,PASSLEN,NOECHO,YEA);
	passbuf[8] = '\0';
	if (!checkpasswd(lookupuser.passwd, passbuf))
	{
	    logattempt(uident, currentuser.userid, passbuf);
	    move(10,20);
	    prints("\33[1;31m密码输入错误 (Password Error)...\33[m\n");
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
	prints("\33[1;32m同步成功!\33[0m");
	pressanykey();
	return 0;
}


