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
$Id: delete.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif
int seclusionday=0;
FILE *fp;
char tmp[80];
void mail_info(int mode,char *lastword);


//int offline()
void d_offline(int mode)
{
	int i;
	char    buf[STRLEN], lastword[640];

	modify_user_mode(mode?RECLUSION:OFFLINE);
	clear();
	if (count_user( )>1)
	{/*Added by Tim 1999/10/14. avoid suiciding or fading when multi-login
		            this is a serious bug..vectra is a victim due to the bug.
		            wuwuw....now, vectra is a newcomer...:(*/
		move(1,0);
		prints("你目前的login不止一个,请退出其他login再重试一次\n");
		pressreturn();
		clear();
		return;
	}

	if (HAS_PERM(PERM_BOARDS) || HAS_PERM(PERM_ADMINMENU) || HAS_PERM(PERM_CHATCLOAK))
	{
		move(1, 0);
		prints("\n\n您有重任在身, 不能随便%s啦!!\n",mode?"归隐":"自杀");
		pressreturn();
		clear();
		return;
	}
	if (!HAS_PERM(PERM_BASIC)||!HAS_PERM(PERM_CHAT)||!HAS_PERM(PERM_PAGE)||!HAS_PERM(PERM_POST)||!HAS_PERM(PERM_LOGINOK)||!HAS_PERM(PERM_MESSAGE))
	{
		move(1, 0);
		prints("\n\n对不起, 您还未够资格执行此命令!!\n");
		prints("请 mail 给 SYSOP 说明原因, 谢谢。\n");
		pressreturn();
		clear();
		return;
	}  /*loveni:隐居自杀不再限制*/
	getdata(1, 0, "请输入你的密码: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n很抱歉, 您输入的密码不正确。\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "请问你叫什么名字? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname))
	{
		prints("\n\n很抱歉, 我并不认识你。\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	if(mode)
	{
		//add by bluetent 2002.12.05
		int usrexp=0;
		prints("目前规定：\n新手上路可以隐居3-5天，一般战友可以隐居3-15天，\n中级站友可以隐居3-30天，高级站友可以隐居3-60天，\n老站友可以隐居3-90天，长老级可以隐居3-120天，\n本站元老及其以上等级可以隐居3-180天。\n有关等级描述请去BBSHelp板查看精华区。\n另外，在隐居期间，您可以使用pop3服务收取站内信件.\n");
		//	move(8,0);
		sprintf( genbuf, "请选择隐居时间(天) [%d]: ", seclusionday);
		while(1)
		{
			getdata( 8, 0, genbuf, buf, 5, DOECHO,YEA);
			seclusionday = atoi( buf );
			if(seclusionday==0)
				return;
			else if(seclusionday>2&&seclusionday<181)
				break;
		}
		usrexp = currentuser.numposts + currentuser.numlogins / 5 + (time(0) - currentuser.firstlogin) / 86400 + currentuser.stay / 3600;
		if (usrexp<=100)
			seclusionday=(seclusionday>5?5:seclusionday);
		if (usrexp<=450)
			seclusionday=(seclusionday>15?15:seclusionday);
		if (usrexp<=850)
			seclusionday=(seclusionday>30?30:seclusionday);
		if (usrexp<=1500)
			seclusionday=(seclusionday>60?60:seclusionday);
		if (usrexp<=2500)
			seclusionday=(seclusionday>90?90:seclusionday);
		if (usrexp<=3000)
			seclusionday=(seclusionday>120?120:seclusionday);
		if (usrexp>5000)
			seclusionday=(seclusionday>180?180:seclusionday);

		//add end
		move(10,0);
		prints("[1;5;31m警告[0;1;31m： 归隐后, 您将%d天无法再用此帐号进入本站！！\n", seclusionday);
		prints("[1;32m但帐号在 %d 天後你可以解除归隐状态 :( .....[m", seclusionday);
		pressanykey();
	}
	else
	{
		prints("警告：自杀后, 您的生命力将减少到14天，14天后您的帐号自动消失");
		move(2,0);
		prints("在这14天内若改变主意的话，则可以通过登录本站一次恢复原生命力");
		move(3, 0);
		prints("[1;31m自杀用户将丢失所有[33m特殊权限[31m！！！[m");
	}
	clear();
	move(5,0);
	i = 0;
	if(askyn("真是舍不得你，你走之前有什么话想说么",NA,NA)==YEA)
	{
		strcpy(lastword,">\n>");
		buf[0] = '\0';
		for(i = 0; i< 8; i++)
		{
			getdata(i+6, 0, ": ", buf, 77, DOECHO, YEA);
			if(buf[0] == '\0')
				break;
			strcat(lastword,buf);
			strcat(lastword,"\n>");
		}
		if(i == 0)
			lastword[0] = '\0';
		else
			strcat(lastword,"\n\n");
		move( i + 8, 0);
		if( i == 0 )
			prints("哎，你还是什么都不愿意说，是不是还有心思未了？");
		else if( i <= 4 )
			prints("看着你憔悴的脸，我心都碎了 ... ");
		else
			prints("我会记得你的，朋友，我也知道你的离开也是没有办法的事，好走了");
	}
	else
	{
		lastword[0] = '\0';
	}
	move( i + 10, 0);
	if (askyn("你确定要离开这个大家庭", NA, NA) == 1)
	{
		clear();
		mail_info(mode,lastword);
		if(mode)
		{
			char buf[80];
			sprintf(buf, "home/%c/%s/Reclusion", toupper(currentuser.userid[0]), currentuser.userid);
			fp=fopen(buf,"w+");
			if(fp)
			{
				//	      flock(fileno(fp),LOCK_EX);
				fprintf(fp,"%d", seclusionday);
				//	      flock(fileno(fp),LOCK_UN);
				fclose(fp);
			}
			currentuser.userlevel|=PERM_RECLUSION;
		}
		else
		{
			if((currentuser.userlevel & PERM_PERSONAL) == 0)
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE;
			else
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE|PERM_PERSONAL;
		}   //  by livebird:  自杀中管理文集权限保留
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
		//      mail_info(mode,lastword);
		modify_user_mode(OFFLINE);
		kick_user(&uinfo);
		exit(0);
	}
}

getuinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%s的代号     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"她":"他",currentuser.userid);
	fprintf(fn, "%s的昵称     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"她":"他",currentuser.username);
	fprintf(fn, "真实姓名     : %s\n", currentuser.realname);
	fprintf(fn, "居住住址     : %s\n", currentuser.address);
	fprintf(fn, "电子邮件信箱 : %s\n", currentuser.email);
	fprintf(fn, "真实 E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "帐号注册地址 : %s\n", currentuser.ident);
	getdatestring(currentuser.firstlogin,NA);
	fprintf(fn, "帐号建立日期 : %s\n", datestring);
	getdatestring(currentuser.lastlogin,NA);
	fprintf(fn, "最近光临日期 : %s\n", datestring);
	fprintf(fn, "最近光临机器 : %s\n", currentuser.lasthost);
	fprintf(fn, "上站次数     : %d 次\n", currentuser.numlogins);
	fprintf(fn, "文章数目     : %d\n", currentuser.numposts);
	fprintf(fn, "上站总时数   : %d 小时 %d 分钟\n",
	        currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "使用者权限   : %s\n\n", buf);
}

/* add by yiyo 记录被查询ID个人信息 */
gethisinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%s的代号     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"她":"他",lookupuser.userid);
	fprintf(fn, "%s的昵称     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"她":"他",lookupuser.username);
	fprintf(fn, "真实姓名     : %s\n", lookupuser.realname);
	fprintf(fn, "居住住址     : %s\n", lookupuser.address);
	fprintf(fn, "电子邮件信箱 : %s\n", lookupuser.email);
	fprintf(fn, "真实 E-mail  : %s\n", lookupuser.reginfo);
	fprintf(fn, "帐号注册地址 : %s\n", lookupuser.ident);
	getdatestring(lookupuser.firstlogin,NA);
	fprintf(fn, "帐号建立日期 : %s\n", datestring);
	getdatestring(lookupuser.lastlogin,NA);
	fprintf(fn, "最近光临日期 : %s\n", datestring);
	fprintf(fn, "最近光临机器 : %s\n", lookupuser.lasthost);
	fprintf(fn, "上站次数     : %d 次\n", lookupuser.numlogins);
	fprintf(fn, "文章数目     : %d\n", lookupuser.numposts);
	fprintf(fn, "上站总时数   : %d 小时 %d 分钟\n",
	        lookupuser.stay / 3600, (lookupuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(lookupuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "使用者权限   : %s\n\n", buf);
}
/* add end */
void mail_info(int mode,char *lastword)
{
	FILE   *fn;
#ifndef TIMECOUNTER

	time_t  now;
#endif

	char    filename[STRLEN];

#ifndef TIMECOUNTER

	now = time(0);
	getdatestring(now,NA);
#else

	getnowdatestring();
#endif

	sprintf(filename, "tmp/suicide.%s", currentuser.userid);
	if ((fn = fopen(filename, "w")) != NULL)
	{
		if(mode)
		{
			fprintf(fn, "\033[1m%s\033[m 已经在 \033[1m%s\033[m 登记归隐了，其资料，敬请保留...", currentuser.userid ,datestring);
		}
		else
		{
			fprintf(fn, "[1m%s[m 已经在 [1m%s[m 登记自杀了，其资料，敬请保留...", currentuser.userid ,datestring);
		}
		getuinfo(fn);
		fclose(fn);
		if(mode)
			Postfile(filename,"syssecurity3","归隐通知(立即生效)...",2);
		else
			Postfile(filename, "syssecurity3", "登记自杀通知(15天後生效)...", 2);
		unlink(filename);
	}
	if ((fn = fopen(filename, "w")) != NULL)
	{
		fprintf(fn, "大家好,\n\n");
		fprintf(fn,"我是 %s (%s)。我己经决定%s\n\n",
		        currentuser.userid, currentuser.username,mode?"暂时离开一段时间了。":"在 15 天后离开这里了。");
		//getdatestring(currentuser.firstlogin,NA);
		/*fprintf(fn, "自注册至今，我已经来此 %d 次了，在这总计 %d 分钟的网络生命中，\n",
		        currentuser.numlogins,currentuser.stay/60);
		fprintf(fn, "我又如何会轻易舍弃呢？但是我得走了...  点点滴滴－－尽在我心中！\n\n");;*/
		fprintf(fn,"%s",lastword);
		if (!mode)
			fprintf(fn,"朋友们，请把 %s 从你们的好友名单中拿掉吧。因为我己经决定离开这里了!\n\n", currentuser.userid);
		else
			fprintf(fn,"朋友们，请不要为我伤心，\n\n");
		sprintf(tmp, "%d天之后", seclusionday);
		fprintf(fn, "%s我会回来的。 珍重!! 再见!!\n\n\n", mode?tmp:"或许有朝一日");
#ifndef TIMECOUNTER

		getdatestring(now,NA);
#else

		getnowdatestring();
#endif

		fprintf(fn, "%s 于 %s 留.\n\n", currentuser.userid, datestring);
		fclose(fn);
		Postfile(filename, "notepad", (mode?"归隐留言...":"登记自杀留言..."), 2);
		unlink(filename);
	}
}
offline()
{
	d_offline(0);
}

fade_out()
{
	d_offline(1);
}
