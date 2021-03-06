// deardragon 2000.09.26 over
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
$Id: userinfo.c,v 1.3 2009-09-14 05:59:18 madoldman Exp $
*/

#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

extern char *cexp(int);
extern char *cperf(int);
extern time_t login_start_time;
extern char fromhost[60];
char   * genpasswd();
char   *sysconf_str();
#define EMAIL_AUTH_EXPIRE_SECOND	60*60*24*3//三天
int PrintColorUser( char * userid_i)
{
	char filename[STRLEN];
	int		curtime;
	static struct stat filestat;
        char	userid[STRLEN];
	strcpy(userid,userid_i);
	for(curtime=0;curtime<strlen(userid);curtime++)
	   if(toupper(userid[curtime])<'A'||toupper(userid[curtime])>'Z')
		{
			userid[curtime]=0;
			break;
		}
	sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
	bzero(&filestat, sizeof(filestat));
	curtime=time(0);
	if(stat(filename, &filestat)==0)	
	{//..[31m═══..[m
		if(curtime-filestat.st_mtime>EMAIL_AUTH_EXPIRE_SECOND)
		{//红色表示过期的Email认证用户
			//return 33;
			;//
			prints("\033[31m%s",userid);
		}//..[31mJun 28..[m
		else
		{//黄色表示待Email认证的用户
			//return 33;//
			prints("\033[33m%s",userid);
		}
	}
	else//nomal
	{
		bzero(&filestat, sizeof(filestat));
		sprintf(filename, "home/%c/%s/.webauth.done", toupper(userid[0]), userid);
		if(stat(filename, &filestat)!=-1)
		{//绿色表示通过Email认证的用户..[36m联系..[m
			//return 36;//
			prints("\033[36m%s",userid);
		}
		else//普通的用户
		{
			//return 37;//
			prints("\033[37m%s",userid);
		}
	}
	return 37;
}

void disply_userinfo( struct userec *u )
{
	int     num, exp;
	char		buf[80];
	//#ifdef REG_EXPIRED
	time_t  now;
#ifndef AUTOGETPERM
	//#endif
#endif

	move(2, 0);
	clrtobot();
	now = time (0);
	set_safe_record();
//	prints("您的代号     : %-14s", u->userid);
//	prints("您的代号     : \033[%dm%-14s\033[37m%", GetAuthUserDispColor(u->userid),u->userid);
	prints("您的代号     : ");
	PrintColorUser(u->userid);
	prints("\033[37m");
	prints("     昵称 : %-20s", u->username);
	prints("     性别 : %s", (u->gender == 'M' ? "男" : "女"));
	prints("\n真实姓名     : %-40s", u->realname);
	prints("  出生日期 : %d/%d/%d",
	       u->birthmonth, u->birthday, u->birthyear + 1900);
	prints("\n居住住址     : %-38s", u->address);
	{
		int tyear,tmonth,tday;
		tyear = u->birthyear+1900;
		tmonth = u->birthmonth;
		tday = u->birthday;
		countdays(&tyear,&tmonth,&tday,now);
		prints("累计生活天数 : %d\n",abs(tyear));
	}
	prints("电子邮件信箱 : %s\n", u->email);
	prints("真实 E-mail  : %s\n", u->reginfo);
	if HAS_PERM (PERM_ADMINMENU)
		prints("Ident 资料   : %s\n", u->ident);
	prints("最近光临机器 : %-22s", u->lasthost);
	prints("终端机形态 : %s\n", u->termtype);
	getdatestring(u->firstlogin,NA);
	prints("帐号建立日期 : %s[距今 %d 天]\n",
	       datestring,(now-(u->firstlogin))/86400);
	getdatestring(u->lastlogin,NA);
	prints("最近光临日期 : %s[距今 %d 天]\n",
	       datestring,(now-(u->lastlogin))/86400);
#ifndef AUTOGETPERM
	/*#ifndef REG_EXPIRED
	   getdatestring(u->lastjustify,NA);
	   prints("身份确认日期 : %s\n",(u->lastjustify==0)? "未曾注册" :datestring);
	#else
	   if(u->lastjustify == 0) prints("身份确认     : 未曾注册\n");
	   else {
	      prints("身份确认     : 已完成，有效期限: ");
	      nextreg = u->lastjustify + REG_EXPIRED * 86400;
	      getdatestring(nextreg,NA);
	      sprintf(genbuf,"%14.14s[%s]，还有 %d 天\n", 
	         datestring ,datestring+23,(nextreg - now) / 86400);
	      prints(genbuf);
	   }      
	#endif*/
#endif
#ifdef ALLOWGAME

	prints("文章数目     : %-20d 奖章数目 : %d\n",u->numposts,u->nummedals);
	prints("私人信箱     : %d 封\n", u->nummails);
	prints("你的银行存款 : %d元  贷款 : %d元 (%s)\n",
	       u->money,u->bet,cmoney(u->money-u->bet));
#else

	prints("文章数目     : %-20d \n", u->numposts);
	
#ifdef ALLOW_MEDALS
	
	int sum = 0;
	int i;
	for(i = 1; i <= 7; i++){
		sum += (u -> nummedals >> (i << 2)) & 0x0f;
	}
	
	prints ("奖章数目     : %d\n", sum);
#endif

	prints("私人信箱     : %d 封 \n", u->nummails);
#endif

	prints("上站次数     : %d 次      ", u->numlogins);
	prints("上站总时数   : %d 小时 %d 分钟\n",u->stay/3600,(u->stay/60)%60);
	exp = countexp(u);
	prints("经验值       : %d  (%s)    ", exp, cexp(exp));
	exp = countperf(u);
	prints("表现值 : %d  (%s)\n", exp, cperf(exp));
	strcpy(genbuf, "bTCPRD#@XWBA#VS-DOM-F0s2345678");
	for (num = 0; num < strlen(genbuf) ; num++)
		if (!(u->userlevel & (1 << num)))
			genbuf[num] = '-';
	prints("使用者权限   : %s\n", genbuf);
	prints("\n");
	if (u->userlevel & PERM_SYSOP)
	{
		prints("  您是本站的站长, 感谢您的辛勤劳动.\n");
	}
	else if (u->userlevel & PERM_BOARDS)
	{
		prints("  您是本站的板主, 感谢您的付出.\n");
	}
	else if (u->userlevel & PERM_LOGINOK)
	{
		prints("  您的注册程序已经完成, 欢迎加入本站.\n");
	}
	else if (u->lastlogin - u->firstlogin < 3 * 86400)
	{
		prints("  新手上路, 请阅读 BBSHelp 讨论区.\n");
	}
	else
	{
		prints("  注册尚未成功, 请参考本站进站画面说明.\n");
	}
	if(HAS_PERM(PERM_LOOKADMIN))
	{
		sprintf(buf, "ADMINQuery '%s'", u->userid);
		report(buf);
	}
	/*  added report for all users */
	else	//if(HAS_PERM(PERM_LOOKADMIN))
	{
		sprintf(buf, "ADMINQuery2 '%s'", u->userid);
		report(buf);
	}

}

void uinfo_change1(int i, struct userec *u, struct userec *newinfo)
{
	char    buf[STRLEN], genbuf[128];

	sprintf(genbuf, "上线次数 [%d]: ", u->numlogins);
	getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->numlogins = atoi(buf);

	sprintf(genbuf, "发表文章数 [%d]: ", u->numposts);
	getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->numposts = atoi(buf);

	//	if (HAS_PERM (PERM_SYSOP))
	{
		sprintf (genbuf, "奖章数 [%d]: ", u->nummedals);
		getdata (i++, 0, genbuf, buf, 10, DOECHO, YEA);
		if (buf [0] != 0 && atoi (buf) >= 0)
			newinfo->nummedals = atoi (buf);
	}

	sprintf(genbuf, "上站时间 [%d]: ", u->stay);
	getdata(i++, 0, genbuf,buf, 10, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->stay = atoi(buf);
	/*
	//Silencer Added
	   sprintf(genbuf, "帐号建立时间 [%d]: ", u->firstlogin);
	   getdata(i++, 0, genbuf, buf,10, DOECHO, YEA);
	   if(atoi(buf) >0)
	     newinfo->firstlogin = atoi(buf);
	*/

#ifdef ALLOWGAME

	sprintf(genbuf, "银行存款 [%d]: ", u->money);
	getdata(i++, 0, genbuf, buf, 8, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->money = atoi(buf);

	sprintf(genbuf, "银行贷款 [%d]: ", u->bet);
	getdata(i++, 0, genbuf, buf, 8, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->bet = atoi(buf);

	sprintf(genbuf, "奖章数 [%d]: ", u->nummedals);
	getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
	if (atoi(buf) > 0)
		newinfo->nummedals = atoi(buf);
#endif
}

void check_uinfo(struct userec *u, int MUST)
{
	int changeIT = 0, changed = 0, pos = 2;
	char ans[5];

	while(1)
	{  // 检查昵称
		changeIT = MUST || (strlen(u->username) < 2)
		           ||(strstr(u->username, "  "))||(strstr(u->username, "　"));
		if(!changeIT)
		{
			if(changed)
			{
				pos ++;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		/*    getdata(pos, 0, "请输入您的昵称 (Enter nickname): ",
		      u->username, NAMELEN, DOECHO, YEA);
		 */
		move(pos,0);
		prints("请输入您的昵称 (Enter nickname) 要有个性一点啊");
		pos++;
		getdata(pos, 0, "> ",
		        u->username, NAMELEN, DOECHO, YEA);
		strcpy(uinfo.username, u->username);
		update_utmp();
	}



	while(1)
	{ // 检查真实姓名
		changeIT = MUST || (strlen(u->realname) < 4)
		           ||(strstr(u->realname, "  "))||(strstr(u->realname, "　"));
		if(!changeIT)
		{
			if(changed)
			{
				pos += 2;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		move(pos, 0);
		prints("请输入您的真实姓名 (Enter realname) 我们会为您保密的");
		getdata(pos+1, 0, "> ", u->realname, NAMELEN, DOECHO, YEA);
	}
	while(1)
	{ // 检查通讯地址
		changeIT = MUST||(strlen(u->address)<10)
		           ||(strstr(u->address,"  "))||(strstr(u->address, "　"));
		if(!changeIT)
		{
			if(changed)
			{
				pos += 2;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		move(pos, 0);
		prints("请输入您的通讯地址 (Enter home address) 详细到门牌或宿舍号\n");
		getdata(pos+1, 0, "> ", u->address, STRLEN - 20, DOECHO, YEA);
	}
	while(1)
	{ // 检查邮件地址
		changeIT = MUST||(strchr(u->email, '@') == NULL);
		if(!changeIT)
		{
#ifdef MAILCHECK
			if(changed)
			{
				pos += 4;
				changed = 0;
			}
#else
			if(changed)
			{
				pos += 3;
				changed = 0;
			}
#endif
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		move(pos, 0);
		prints("电子信箱格式为: [1;37muserid@your.domain.name[m\n");
#ifdef MAILCHECK

		prints( "[32m本站已经提供[33m邮件注册[32m功能, 您可以通过邮件快速地通过注册认证.[m\n");
#endif

		prints("请输入电子信箱 (不能提供者按 <Enter>)");
#ifdef MAILCHECK

		getdata(pos+3,0,"> ",u->email,48,DOECHO, YEA);
#else

		getdata(pos+2,0,"> ",u->email,48,DOECHO, YEA);
#endif

		if (strchr(u->email, '@') == NULL)
		{
			sprintf(genbuf, "%s.bbs@%s", u->userid, BBSHOST);
			strncpy(u->email, genbuf, 48);
		}
	}
	{ // 检查性别
		changeIT = MUST||(strchr("MF",u->gender) == NULL);
		if(changeIT)
		{
			getdata(pos, 0, "请输入你的性别: M.男 F.女 [M]: ", ans,2,DOECHO,YEA);
			if(ans[0]!='F'&& ans[0]!='f'||ans[0]=='m')
				u->gender = 'M';
			else
				u->gender = 'F';
			pos ++;
		}
	}
	while(1)
	{ // 检查出生年
		changeIT = MUST||(u->birthyear <20) ||(u->birthyear>98);
		if(!changeIT)
		{
			if(changed)
			{
				pos ++;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		getdata(pos, 0, "请输入您的生日年份(四位数): ", ans, 5, DOECHO, YEA);
		if(atoi(ans)<1920 || atoi(ans) > 1998 )
		{
			MUST = 1;
			continue;
		}
		u->birthyear = atoi(ans) -1900;
	}
	while(1)
	{ // 检查出生月
		changeIT = MUST||(u->birthmonth <1) ||(u->birthmonth>12);
		if(!changeIT)
		{
			if(changed)
			{
				pos ++;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		getdata(pos, 0, "请输入您的生日月份: ", ans, 3, DOECHO, YEA);
		u->birthmonth = atoi(ans);
	}
	while(1)
	{ // 检查出生日
		changeIT = MUST||(u->birthday <1) ||(u->birthday>31)
		           ||(u->birthmonth<8&&!(u->birthmonth%2)||u->birthmonth>7&&(u->birthmonth%2))&&u->birthday>30 || u->birthmonth==2&&u->birthday>29;
		if(!changeIT)
		{
			if(changed)
			{
				pos ++;
				changed = 0;
			}
			break;
		}
		else
		{
			MUST = 0;
			changed = 1;
		}
		getdata(pos, 0, "请输入您的出生日: ", ans, 3, DOECHO, YEA);
		u->birthday = atoi(ans);
	}
}


int uinfo_query(struct userec *u, int real, int unum)
{
	struct userec newinfo;
	char    ans[3], buf[STRLEN], genbuf[128];
	char    src[STRLEN], dst[STRLEN];
	int     i, fail = 0 ;
#ifdef MAILCHANGED

	int    netty_check = 0;
#endif

	time_t  now;
	struct tm *tmnow;
	memcpy(&newinfo, u, sizeof(currentuser));
	getdata(t_lines - 1, 0, real ?
	        "请选择 (0)结束 (1)修改资料 (2)设定密码 (3) 改 ID ==> [0]" :
	        "请选择 (0)结束 (1)修改资料 (2)设定密码 (3) 选签名档 ==> [0]",
	        ans, 2, DOECHO, YEA);
	clear();
	refresh();
	now = time(0);
	tmnow = localtime(&now);

	i = 3;
	move(i++, 0);
	if (ans[0] != '3' || real)
		prints("使用者代号: %s\n", u->userid);
	switch (ans[0])
	{
	case '1':
		move(1, 0);
		prints("请逐项修改,直接按 <ENTER> 代表使用 [] 内的资料。\n");
		sprintf(genbuf, "昵称 [%s]: ", u->username);
		getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.username, buf, NAMELEN);

		sprintf(genbuf, "真实姓名 [%s]: ", u->realname);
		getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.realname, buf, NAMELEN);

		sprintf(genbuf, "居住地址 [%s]: ", u->address);
		getdata(i++, 0, genbuf, buf, STRLEN - 10, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.address, buf, NAMELEN);

		sprintf(genbuf, "电子信箱 [%s]: ", u->email);
		getdata(i++, 0, genbuf, buf, 48, DOECHO, YEA);
		if (buf[0])
		{
#ifdef MAILCHECK
#ifdef MAILCHANGED
			if(u->uid == usernum)
				netty_check = 1;
#endif
#endif

			strncpy(newinfo.email, buf, 48);
		}

		sprintf(genbuf, "终端机形态 [%s]: ", u->termtype);
		getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
		if (buf[0])
			strncpy(newinfo.termtype, buf, 16);

		sprintf(genbuf, "出生年 [%d]: ", u->birthyear + 1900);
		getdata(i++, 0, genbuf, buf, 5, DOECHO, YEA);
		if (   buf[0] && atoi(buf) > 1920 && atoi(buf) < 1998)
			newinfo.birthyear = atoi(buf) - 1900;

		sprintf(genbuf, "出生月 [%d]: ", u->birthmonth);
		getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 12)
			newinfo.birthmonth = atoi(buf);

		sprintf(genbuf, "出生日 [%d]: ", u->birthday);
		getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
		if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 31)
			newinfo.birthday = atoi(buf);

		sprintf(genbuf, "性别(M.男)(F.女) [%c]: ", u->gender);
		getdata(i++, 0, genbuf, buf, 2, DOECHO, YEA);
		if (buf[0])
		{
			if (strchr("MmFf", buf[0]))
				newinfo.gender = toupper(buf[0]);
		}

		if (real)
			uinfo_change1(i,u,&newinfo);
		break;
	case '2':
		if (!real)
		{
			getdata(i++, 0, "请输入原密码: ", buf, PASSLEN, NOECHO, YEA);
			if (*buf == '\0' || !checkpasswd(u->passwd, buf))
			{
				prints("\n\n很抱歉, 您输入的密码不正确。\n");
				fail++;
				break;
			}
		}
		getdata(i++, 0, "请设定新密码: ", buf, PASSLEN, NOECHO, YEA);
		if (buf[0] == '\0')
		{
			prints("\n\n密码设定取消, 继续使用旧密码\n");
			fail++;
			break;
		}
		strncpy(genbuf, buf, PASSLEN);
		getdata(i++, 0, "请重新输入新密码: ", buf, PASSLEN, NOECHO, YEA);
		if (strncmp(buf, genbuf, PASSLEN))
		{
			prints("\n\n新密码确认失败, 无法设定新密码。\n");
			fail++;
			break;
		}
		buf[8] = '\0';
		strncpy(newinfo.passwd, genpasswd(buf), ENCPASSLEN);
		break;
	case '3':
		if (!real)
		{
			sprintf(genbuf, "目前使用签名档 [%d]: ", u->signature);
			getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
			if (atoi(buf) > 0)
				newinfo.signature = atoi(buf);
		}
		else
		{
			struct user_info uin;
			extern int t_cmpuids();
			if(t_search_ulist(&uin, t_cmpuids, unum, NA, NA)!=0)
			{
				prints("\n对不起，该用户目前正在线上。");
				fail++;
			}
			else if(!strcmp(lookupuser.userid,"SYSOP"))
			{
				prints("\n对不起，你不可以修改 SYSOP 的 ID。");
				fail++;
			}
			else
			{
				getdata(i++,0,"新的使用者代号: ",genbuf,IDLEN+1,DOECHO, YEA);
				if (*genbuf != '\0')
				{
					if (getuser(genbuf))
					{
						prints("\n对不起! 已经有同样 ID 的使用者\n");
						fail++;
					}
					else
					{
						strncpy(newinfo.userid, genbuf, IDLEN + 2);
					}
				}
				else
					fail ++;
			}
		}
		break;
	default:
		clear();
		return 0;
	}
	if (fail != 0)
	{
		pressreturn();
		clear();
		return 0;
	}
	if (askyn("确定要改变吗", NA, YEA) == YEA)
	{
		if (real)
		{
			char    secu[STRLEN];
			sprintf(secu, "修改 %s 的基本资料或密码。", u->userid);
			securityreport1(secu);
		}
		if (strcmp(u->userid, newinfo.userid))
		{
			sprintf(src, "mail/%c/%s", toupper(u->userid[0]), u->userid);
			sprintf(dst, "mail/%c/%s", toupper(newinfo.userid[0]), newinfo.userid);
			rename(src, dst);
			sethomepath(src, u->userid);
			sethomepath(dst, newinfo.userid);
			rename(src, dst);
			sethomefile(src, u->userid, "register");
			unlink(src);
			sethomefile(src, u->userid, "register.old");
			unlink(src);
			setuserid(unum, newinfo.userid);
		}
		if(!strcmp(u->userid, currentuser.userid))
		{
			extern int WishNum;
			strncpy(uinfo.username,newinfo.username,NAMELEN);
			WishNum = 9999;
		}
#ifdef MAILCHECK
#ifdef MAILCHANGED
		if ((netty_check == 1)&&!HAS_PERM(PERM_SYSOP))
		{
			sprintf(genbuf, "%s", BBSHOST);
			if ( (!strstr(newinfo.email, genbuf)) &&
			        (!invalidaddr(newinfo.email)) &&
			        (!invalid_email(newinfo.email)))
			{
				strcpy(u->email, newinfo.email);
				send_regmail(u);
			}
			else
			{
				move(t_lines - 5, 0);
				prints("\n您所填的电子邮件地址 【[1;33m%s[m】\n",newinfo.email);
				prints("恕不受本站承认，系统不会投递注册信，请把它修正好...\n");
				pressanykey();
				return 0;
			}
		}
#endif
#endif
		memcpy(u, &newinfo, (size_t)sizeof(currentuser));
#ifdef MAILCHECK
#ifdef MAILCHANGED

		if ((netty_check == 1)&&!HAS_PERM(PERM_SYSOP))
		{
			newinfo.userlevel &= ~(PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);
			sethomefile(src, newinfo.userid, "register");
			sethomefile(dst, newinfo.userid, "register.old");
			rename(src, dst);
		}
#endif
#endif
		substitute_record(PASSFILE, &newinfo, sizeof(newinfo), unum);
	}
	clear();
	return 0;
}

void x_info()
{
	if (!strcmp("guest", currentuser.userid))
		return;
	modify_user_mode(GMENU);
	disply_userinfo(&currentuser);
	uinfo_query(&currentuser, 0, usernum);
}

void getfield(int line, char *info, char *desc, char *buf, int len)
{
	char    prompt[STRLEN];
	sprintf(genbuf, "  原先设定: %-46.46s [1;32m(%s)[m",
	        (buf[0] == '\0') ? "(未设定)" : buf, info);
	move(line, 0);
	prints("%s",genbuf);
	sprintf(prompt, "  %s: ", desc);
	getdata(line + 1, 0, prompt, genbuf, len, DOECHO, YEA);
	if (genbuf[0] != '\0')
		strncpy(buf, genbuf, len);
	move(line, 0);
	clrtoeol();
	prints("  %s: %s\n", desc, buf);
	clrtoeol();
}

void
x_fillform()
{
	char    rname[NAMELEN], addr[STRLEN];
	char    phone[STRLEN], dept[STRLEN], assoc[STRLEN];
	char    ans[5], *mesg, *ptr;
	FILE   *fn;
	time_t  now;

	if (!strcmp("guest", currentuser.userid))
		return;
	modify_user_mode(NEW);
	clear();
	move(2, 0);
	clrtobot();
	if (currentuser.userlevel & PERM_LOGINOK)
	{
		prints("您已经完成本站的使用者注册手续, 欢迎加入本站的行列.");
		pressreturn();
		return;
	}
	/*
	#ifdef PASSAFTERTHREEDAYS
	   if (currentuser.lastlogin - currentuser.firstlogin < 3 * 86400) {
	      prints("您首次登入本站未满三天(72个小时)...\n");
	      prints("请先四处熟悉一下，在满三天以后再填写注册单。");
	      pressreturn();
	      return ;
	   }   
	#endif
	#ifdef PASSAFTERONEDAYS
	   if (currentuser.lastlogin - currentuser.firstlogin < 86400) {
	      prints("您首次登入本站未满一天(24个小时)...\n");
	      prints("请先四处熟悉一下，在满24小时以后再填写注册单。");
	      pressreturn();
	      return ;
	   }   
	#endif
	*/
	if ((fn = fopen("new_register", "r")) != NULL)
	{
		while (fgets(genbuf, STRLEN, fn) != NULL)
		{
			if ((ptr = strchr(genbuf, '\n')) != NULL)
				*ptr = '\0';
			if (   strncmp(genbuf, "userid: ", 8) == 0
			        && strcmp(genbuf + 8, currentuser.userid) == 0)
			{
				fclose(fn);
				prints("站长尚未处理您的注册申请单, 您先到处看看吧.");
				pressreturn();
				return;
			}
		}
		fclose(fn);
	}
	strncpy(rname, currentuser.realname, NAMELEN);
	strncpy(addr, currentuser.address, STRLEN-8);
	dept[0] = phone[0] = assoc[0] = '\0';
	while (1)
	{
		move(3, 0);
		clrtoeol();
		prints("%s 您好, 请据实填写以下的资料:\n", currentuser.userid);
		getfield(6, "请用中文", "真实姓名", rname, NAMELEN);
		getfield(8, "学校院系年级或公司职称", "学校系级", dept, STRLEN);
		getfield(10, "包括寝室或门牌号码", "目前住址", addr, STRLEN);
		getfield(12, "包括可联络时间", "联络电话", phone, STRLEN);
		//getfield(14, "校友会或毕业学校", "校 友 会", assoc, STRLEN);
		mesg = "以上资料是否正确, 按 Q 放弃注册 (Y/N/Quit)? [Y]: ";
		getdata(t_lines - 1, 0, mesg, ans, 3, DOECHO, YEA);
		if (ans[0] == 'Q' || ans[0] == 'q')
			return;
		if (ans[0] != 'N' && ans[0] != 'n')
			break;
	}
	strncpy(currentuser.realname, rname, NAMELEN);
	strncpy(currentuser.address, addr, STRLEN-8);
	if ((fn = fopen("new_register", "a")) != NULL)
	{
		now = time(NULL);
		getdatestring(now,NA);
		fprintf(fn, "usernum: %d, %s\n", usernum, datestring);
		fprintf(fn, "userid: %s\n", currentuser.userid);
		fprintf(fn, "realname: %s\n", rname);
		fprintf(fn, "dept: %s\n", dept);
		fprintf(fn, "addr: %s\n", addr);
		fprintf(fn, "phone: %s\n", phone);
		sprintf(assoc,"%s", fromhost);
		fprintf(fn, "regip: %s\n", assoc);
		fprintf(fn, "----\n");
		fclose(fn);
	}
	setuserfile(genbuf, "mailcheck");
	if ((fn = fopen(genbuf, "w")) == NULL)
	{
		fclose(fn);
		return;
	}
	fprintf(fn, "usernum: %d\n", usernum);
	fclose(fn);
}

void print_userstat(struct userec* user)
{
	FILE *fp;
	struct userec tmp;
	int logins=0, posts=0, stays=0, lifes=0, total=0, exps=0;
	clear();
	move(1,1);
	if(!strcasecmp(user->userid, "guest")){
		prints("faint! guest 也要查全站排名？\n");
		pressanykey();
		return;
	}
	fp=fopen(".PASSWDS", "r");
	while(1) {
		if(fread(&tmp, sizeof(tmp), 1, fp) <= 0)
			break;
		if(tmp.userid[0] < 'A')
			continue;
		if(tmp.userlevel == 0)
			continue;
	        if(tmp.numposts >= user->numposts)
			posts ++;
		if(tmp.numlogins >= user->numlogins)
			logins ++;
		if(tmp.stay >= user->stay)
			stays ++;
		if(tmp.firstlogin <= user->firstlogin)
			lifes++;
		if(countexp(&tmp) >= countexp(user))
			exps++;
		total++;
	}
	fclose(fp);
	prints("       %s -- 个人排名统计 [使用者: %s]\n\n\n",BBSNAME, user->userid);
	prints("            项目        数值    全站排名         相对比例\n");
	//prints不支持%f,懒得改了，直接算
	prints("        本站网龄  %8d天    %8d      TOP %3d.%2d%%\n",
		(time(0) - user->firstlogin)/86400, lifes, 
		lifes*100/total, (lifes - lifes/total*total)*10000/total %100);
	prints("        上站次数  %8d次    %8d      TOP %3d.%2d%%\n",
		user->numlogins, logins, logins*100/total, 
		(logins - logins/total*total)*10000/total %100);
	prints("        发表文章  %8d次    %8d      TOP %3d.%2d%%\n",
		user->numposts, posts, posts*100/total,
		(posts - posts/total*total)*10000/total %100);
	prints("        在线时间  %8d分    %8d      TOP %3d.%2d%%\n",
		user->stay/60, stays, stays*100/total,
		(stays - stays/total*total)*10000/total %100);
	prints("        总经验值  %8d点    %8d      TOP %3d.%2d%%\n",
               countexp(user) , exps , exps*100/total,
	       (exps - exps/total*total)*10000/total %100);
	prints("        总用户数    %8d\n", total);
	pressanykey();
	return;
}

int QueryUserstat()
{
	modify_user_mode(USERDEF);
	print_userstat(&currentuser);
	return 0;
}

int user_switchcase()
{
        char newuserid[IDLEN+2];
        modify_user_mode(USERDEF);
        clear();
        if (count_user( )>1)
        {
                move(1,0);
                prints("你目前的login不止一个,请退出其他login再重试一次\n");
                pressreturn();
                clear();
                return 1;
        }
        if(HAS_PERM(PERM_BOARDS)){
                move(1,0);
                prints("对不起，目前不支持板务修改用户名大小写，请先联系讨论区管理员,\n");
                prints("离任所有板务，然后进行修改，再重新任命板务。");
                pressreturn();
                clear();
                return 1;
        }
        getdata(0, 0, "请输入新的帐号名称 (Enter User ID): ",
                        newuserid, IDLEN + 1, DOECHO, YEA);
        if(newuserid[0] == '\0'){
                clear();
                return 0;
        }
        if(!strcmp(currentuser.userid, newuserid)){
                clear();
                return 0;
        }
        if(strcasecmp(currentuser.userid, newuserid)){
               move(1,0);
                prints("新的帐号名称和原有帐号不同，你仅能改变帐号的大小写！");
                pressreturn();
                clear();
                return 1;
        }
		if(HAS_PERM(PERM_PERSONAL))
	        dig_user_switchcase(newuserid);
		sprintf(genbuf,"mv %s/home/%C/%s %s/home/%C/%s",
				BBSHOME, toupper(currentuser.userid[0]),currentuser.userid,
				BBSHOME, toupper(newuserid[0]), newuserid);
		system(genbuf);
		sprintf(genbuf,"mv %s/mail/%C/%s %s/mail/%C/%s",
				BBSHOME, toupper(currentuser.userid[0]),currentuser.userid,
				BBSHOME, toupper(newuserid[0]), newuserid);
		system(genbuf);
        strcpy(currentuser.userid, newuserid);
        strcpy(uinfo.userid, newuserid);
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
        move(5,10);
        prints("修改帐号大小写成功。");
        pressreturn();
        return 0;
}

// deardragon 2000.09.26 over
