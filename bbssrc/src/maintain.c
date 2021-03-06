/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang,syssecurity Smallpig.bbs@bbs.cs.ccu.edu.tw
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
$Id: maintain.c,v 1.4 2010-07-05 04:58:44 madoldman Exp $
*/

#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

#ifndef NEW_CREATE_BRD
char    cexplain[STRLEN];
#endif
char    lookgrp[30];
FILE   *cleanlog;

int check_systempasswd()
{
	FILE   *pass;
	char    passbuf[20], prepass[STRLEN];
	clear();
	if ((pass = fopen("etc/.syspasswd", "r")) != NULL)
	{
		fgets(prepass, STRLEN, pass);
		fclose(pass);
		prepass[strlen(prepass) - 1] = '\0';
		getdata(1, 0, "请输入系统密码: ", passbuf, 19, NOECHO, YEA);
		if (passbuf[0] == '\0' || passbuf[0] == '\n')
			return NA;
		if (!checkpasswd(prepass, passbuf))
		{
			move(2, 0);
			prints("错误的系统密码...");
			securityreport("系统密码输入错误...");
			pressanykey();
			return NA;
		}
	}
	return YEA;
}


void autoreport(char* title,char* str, int toboard, char* userid)
{
	FILE *se;
	char fname[STRLEN];
	int  savemode;
	struct boardheader fh;
	extern int cmpbnames();
	search_record(BOARDS, &fh, sizeof(fh), cmpbnames, currboard);
	savemode = uinfo.mode;
	report(title);
	sprintf(fname,"tmp/%s.%s.%05d",BBSID,currentuser.userid,uinfo.pid);
	if((se=fopen(fname,"w"))!=NULL)
	{
		fprintf(se,"%s",str);
		fclose(se);
		if(userid != NULL)
			mail_file(fname,userid,title);
		if(toboard)
			Postfile( fname,currboard,title,1);
		if(userid!=NULL && fh.level == 0)
			Postfile(fname, "Punishment", title,2);//add by bluetent - 2002.11.5
		unlink(fname);
		modify_user_mode( savemode );
	}
}

int securityreport3(char *str,int mod)
{
	FILE   *se;
	char    fname[STRLEN];
	int     savemode;
	savemode = uinfo.mode;
	report(str);
	sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL)
	{
		char tmpbuf [256];
		sprintf (tmpbuf, "%d", mod);
		fprintf(se, "系统安全记录\n[1m原因：%s[m\n", str);
		if (mod >= 0)
		{
			fprintf (se, "方式: ");
			if (mod == 0)
				fprintf (se, "不减文章数\n");
			else
				fprintf (se, "文章数减 %d\n", mod);
		}
		fprintf(se, "以下是个人资料");
		getuinfo(se);
		fclose(se);
		Postfile(fname, "syssecurity2", str, 2);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

int securityreport(char *str)
{
	FILE   *se;
	char    fname[STRLEN];
	int     savemode;
	savemode = uinfo.mode;
	report(str);
	sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL)
	{
		fprintf(se, "系统安全记录\n[1m原因：%s[m\n", str);
		fprintf(se, "以下是个人资料");
		getuinfo(se);
		fclose(se);
		Postfile(fname, "syssecurity1", str, 2);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

int security_report(char *str,int i)
{
	FILE   *se;
        char    fname[STRLEN];
        int     savemode;
        savemode = uinfo.mode;
        report(str);
        sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
        if ((se = fopen(fname, "w")) != NULL)
        {
                fprintf(se, "系统安全记录\n\033[1m原因：%s\033[m\n", str);
                fprintf(se, "以下是个人资料");
                getuinfo(se);
                fclose(se);
                if(i==1)
			Postfile(fname, "syssecurity1", str, 2);
		else if(i==2)
			Postfile(fname, "syssecurity2",str, 2);
		else if(i==3)
			Postfile(fname, "syssecurity3",str, 2);
		else if(i==4)
			Postfile(fname, "syssecurity4",str, 2);
                unlink(fname);
                modify_user_mode(savemode);
        }
}
// add by istef @ 6/21/2005
// move the record of account management(s) to syssecurity3
int securityreport4(char *str)
{
	FILE   *se;
	char    fname[STRLEN];
	int     savemode;
	savemode = uinfo.mode;
	report(str);
	sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL)
	{
		fprintf(se, "系统安全记录\n[1m原因：%s[m\n", str);
		fprintf(se, "以下是个人资料");
		getuinfo(se);
		fclose(se);
		Postfile(fname, "syssecurity3", str, 2);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

/* add by yiyo 修改个人资料或权限详细记录 */
int securityreport1(char *str)
{
	FILE   *se;
	char    fname[STRLEN];
	int     savemode;
	savemode = uinfo.mode;
	report(str);
	sprintf(fname, "tmp/security.%s.%05d", currentuser.userid, uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL)
	{
		fprintf(se, "系统安全记录\n\033[1m原因：%s\033[m\n", str);
		fprintf(se, "以下是改动前后的个人资料");
		gethisinfo(se);
		fclose(se);
		Postfile(fname, "syssecurity1", str, 2);
		unlink(fname);
		modify_user_mode(savemode);
	}
}
/* add end */

/*
int
get_grp(seekstr)
char    seekstr[STRLEN];
{
	FILE   *fp;
	char    buf[STRLEN];
	char   *namep;
	if ((fp = fopen("0Announce/.Search", "r")) == NULL)
		return 0;
	while (fgets(buf, STRLEN, fp) != NULL) {
		namep = strtok(buf, ": \n\r\t");
		if (namep != NULL && ci_strcmp(namep, seekstr) == 0) {
			fclose(fp);
			strtok(NULL, "/");
			namep = strtok(NULL, "/");
			if (strlen(namep) < 30) {
				strcpy(lookgrp, namep);
				fclose(fp); // add by yiyo 
				return 1;
			} else{
				fclose(fp); // add by yiyo 
				return 0;
			}
		}
	}
	fclose(fp);
	return 0;
}
*/
int get_grp(char seekstr[STRLEN])
{
	FILE   *fp;
	char    buf[STRLEN];
	char   *namep;
	if ((fp = fopen("0Announce/.Search", "r")) == NULL)
		return 0;
	while (fgets(buf, STRLEN, fp) != NULL)
	{
		namep = strtok(buf, ": \n\r\t");
		if (namep != NULL && ci_strcmp(namep, seekstr) == 0)
		{
			fclose(fp);
			strtok(NULL, "/");
			namep = strtok(NULL, "/");
			if (strlen(namep) < 30)
			{
				strcpy(lookgrp, namep);
				return 1;
			}
			else
				return 0;
		}
	}
	fclose(fp);
	return 0;
}

void stand_title(char* title)
{
	clear();
	standout();
	prints("%s",title);
	standend();
}

int valid_brdname(char* brd)
{
	char    ch;
	ch = *brd++;
	if (!isalnum(ch) && ch != '_')
		return 0;
	while ((ch = *brd++) != '\0')
	{
		if (!isalnum(ch) && ch != '_')
			return 0;
	}
	return 1;
}
#ifndef NEW_CREATE_BRD
char* chgrp()
{
	int     i, ch;
	char    buf[STRLEN], ans[6];

	static char *explain[] = {
	                             "本站系统",
	                             "南开大学",
	                             "电脑技术",
	                             "视觉联盟",
	                             "文学艺术",
	                             "人文社会",
	                             "校园生活",
	                             "休闲娱乐",
	                             "知性感性",
	                             "体育运动",
	                             "社团群体",
	                             "兄弟院校",
	                             "游戏天堂",
	                             "新讨论区",
				     			 "四海一家",
	                             NULL
	                         };

	static char *groups[] = {
	                            "GROUP_0",
	                            "GROUP_1",
	                            "GROUP_2",
	                            "GROUP_3",
	                            "GROUP_4",
	                            "GROUP_5",
	                            "GROUP_6",
	                            "GROUP_7",
	                            "GROUP_8",
	                            "GROUP_9",
	                            "GROUP_10",
	                            "GROUP_11",
	                            "GROUP_12",
	                            "GROUP_13",
				    			"GROUP_14",
	                            NULL
	                        };
	clear();
	move(2, 0);
	prints("选择精华区的目录\n\n");
	for (i = 0;; i++)
	{
		if (explain[i] == NULL || groups[i] == NULL)
			break;
		prints("[1;32m%2d[m. %-20s%-20s\n", i, explain[i], groups[i]);
	}
	sprintf(buf, "请输入你的选择(0~%d): ", --i);
	while (1)
	{
		getdata(i + 6, 0, buf, ans, 4, DOECHO, YEA);
		if (!isdigit(ans[0]))
			continue;
		ch = atoi(ans);
		if (ch < 0 || ch > i || ans[0] == '\r' || ans[0] == '\0')
			continue;
		else
			break;
	}
	sprintf(cexplain, "%s", explain[ch]);

	return groups[ch];
}
#endif

char    curruser[IDLEN + 2];
extern int delmsgs[];
extern int delcnt;

void domailclean(struct fileheader* fhdrp)
{
	static int newcnt, savecnt, deleted, idc;
	char    buf[STRLEN];
	if (fhdrp == NULL)
	{
		fprintf(cleanlog, "new = %d, saved = %d, deleted = %d\n",
		        newcnt, savecnt, deleted);
		newcnt = savecnt = deleted = idc = 0;
		if (delcnt)
		{
			sprintf(buf, "mail/%c/%s/%s", toupper(curruser[0]), curruser, DOT_DIR);
			while (delcnt--)
				delete_record(buf, sizeof(struct fileheader), delmsgs[delcnt]);
		}
		delcnt = 0;
		return;
	}
	idc++;
	if (!(fhdrp->accessed[0] & FILE_READ))
		newcnt++;
	else if (fhdrp->accessed[0] & FILE_MARKED)
		savecnt++;
	else
	{
		deleted++;
		sprintf(buf, "mail/%c/%s/%s", toupper(curruser[0]), curruser, fhdrp->filename);
		unlink(buf);
		delmsgs[delcnt++] = idc;
	}
}

int cleanmail(struct userec* urec)
{
	struct stat statb;
	if (urec->userid[0] == '\0' || !strcmp(urec->userid, "new"))
		return 0;
	sprintf(genbuf, "mail/%c/%s/%s", toupper(urec->userid[0]), urec->userid, DOT_DIR);
	fprintf(cleanlog, "%s: ", urec->userid);
	if (stat(genbuf, &statb) == -1)
		fprintf(cleanlog, "no mail\n");
	else if (statb.st_size == 0)
		fprintf(cleanlog, "no mail\n");
	else
	{
		strcpy(curruser, urec->userid);
		delcnt = 0;
		apply_record(genbuf, domailclean, sizeof(struct fileheader));
		domailclean(NULL);
	}
	return 0;
}


void trace_state(int flag, char* name, int size)
{
	char    buf[STRLEN];
	if (flag != -1)
	{
		sprintf(buf, "ON (size = %d)", size);
	}
	else
	{
		strcpy(buf, "OFF");
	}
	prints("%s记录 %s\n", name, buf);
}


int scan_register_form(char* regfile)
{
	static char *field[] = {"usernum", "userid", "realname", "dept",
	                        "addr", "phone", "regip", NULL};
	static char *finfo[] = {"帐号位置", "申请帐号", "真实姓名", "学校系级",
	                        "目前住址", "连络电话", "注册ip  ", NULL};
	static char *reason[] = {"请确实填写真实姓名(不可使用拼音).", 
                                 "请详填学校院系与年级 (注明系和年级或者工作单位和工作部门).",
	                         "请填写完整的住址资料 (完整详细，能够通信).", 
                                 "请详填连络电话 (固定电话需填写区号).",
	                         "请勿穿梭注册.", 
                                 "请如实详细填写注册申请表.",
	                         "请用中文填写申请表.", 
                                 "同一个用户注册了过多ID.", NULL};
	struct userec uinfo;
	FILE   *fn, *fout, *freg;
	char    fdata[7][STRLEN];
	char    fname[STRLEN], buf[STRLEN];
	char    ans[5], *ptr, *uid;
	int     n, unum;
	uid = currentuser.userid;
	char numofreason[2];//tdhlshx for refuse.
	int total;
	int iis;
	int realnumofrea[8];//add end

	stand_title("依序设定所有新注册资料");
	sprintf(fname, "%s.tmp", regfile);
	move(2, 0);
	if (dashf(fname))
	{
		move(1, 0);
		prints("其他 SYSOP 正在查看注册申请单, 请检查使用者状态.\n");
		getdata(2, 0, "你确定没有其他 SYSOP 在审核注册单吗 ？ [y/N]: ", ans, 2, DOECHO, YEA);
		if (ans[0] == 'Y' || ans[0] == 'y')
			f_cp(fname, regfile, O_APPEND);
		else
		{
			pressreturn();
			return -1;
		}
	}
	rename(regfile, fname);
	if ((fn = fopen(fname, "r")) == NULL)
	{
		move(2, 0);//请管理员输入未通过原因:
		prints("系统错误, 无法读取注册资料档: %s\n", fname);
		pressreturn();
		return -1;
	}
	memset(fdata, 0, sizeof(fdata));
	while (fgets(genbuf, STRLEN, fn) != NULL)
	{
		if ((ptr = (char *) strstr(genbuf, ": ")) != NULL)
		{
			*ptr = '\0';
			for (n = 0; field[n] != NULL; n++)
			{
				if (strcmp(genbuf, field[n]) == 0)
				{
					strcpy(fdata[n], ptr + 2);
					if ((ptr = (char *) strchr(fdata[n], '\n')) != NULL)
						*ptr = '\0';
				}
			}
		}
		else if ((unum = getuser(fdata[1])) == 0)
		{
			move(2, 0);
			clrtobot();
			prints("系统错误, 查无此帐号.\n\n");
			for (n = 0; field[n] != NULL; n++)
				prints("%s     : %s\n", finfo[n], fdata[n]);
			pressreturn();
			memset(fdata, 0, sizeof(fdata));
		}
		else
		{
			memcpy(&uinfo, &lookupuser, sizeof(uinfo));
			move(1, 0);
			prints("帐号位置     : %d\n", unum);
			disply_userinfo(&uinfo);
			move(15, 0);
			printdash(NULL);
			for (n = 0; field[n] != NULL; n++)
				prints("%s     : %s\n", finfo[n], fdata[n]);
			if (uinfo.userlevel & PERM_LOGINOK)
			{
				move(t_lines - 1, 0);
				prints("此帐号不需再填写注册单.\n");
				igetkey();
				ans[0] = 'D';
			}
			else
			{
				getdata(t_lines - 1, 0, "是否接受此资料 (Y/N/Q/Del/Skip)? [S]: ",
				        ans, 3, DOECHO, YEA);
			}
			move(1, 0);
			clrtobot();
			switch (ans[0])
			{
			case 'D':
			case 'd':
				break;
			case 'Y':
			case 'y':
				prints("以下使用者资料已经更新:\n");
				n = strlen(fdata[5]);
				if (n + strlen(fdata[3]) > 60)
				{
					if (n > 40)
						fdata[5][n = 40] = '\0';
					fdata[3][60 - n] = '\0';
				}
				strncpy(uinfo.realname, fdata[2], NAMELEN);
				strncpy(uinfo.address, fdata[4], NAMELEN);
				sprintf(genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
				genbuf[STRLEN - 16] = '\0';
				strncpy(uinfo.reginfo, genbuf, STRLEN - 17);
#ifdef ALLOWGAME

				uinfo.money = 1000;
#endif

				uinfo.lastjustify = time(0);
				substitute_record(PASSFILE, &uinfo, sizeof(uinfo), unum);
				sethomefile(buf, uinfo.userid, "register");
				if (dashf(buf))
				{
					sethomefile(genbuf, uinfo.userid, "register.old");
					rename(buf, genbuf);
				}
				if ((fout = fopen(buf, "w")) != NULL)
				{
					for (n = 0; field[n] != NULL; n++)
						fprintf(fout, "%s: %s\n", field[n], fdata[n]);
					n = time(NULL);
					getdatestring(n,NA);
					fprintf(fout, "Date: %s\n", datestring);
					fprintf(fout, "Approved: %s\n", uid);
					fclose(fout);
				}
				mail_file("etc/s_fill", uinfo.userid, "恭禧你，你已经完成注册。");
				mail_file("etc/fornewcomers",uinfo.userid,"欢迎加入 [33m"BBSNAME"[m 大家庭");//tdhlshx 新手FAQ信件
				sethomefile(buf, uinfo.userid, "mailcheck");
				unlink(buf);
				sprintf(genbuf, "让 %s 通过身份确认.", uinfo.userid);
				securityreport4(genbuf);
				break;
			case 'Q':
			case 'q':
				if ((freg = fopen(regfile, "a")) != NULL)
				{
					for (n = 0; field[n] != NULL; n++)
						fprintf(freg, "%s: %s\n", field[n], fdata[n]);
					fprintf(freg, "----\n");
					while (fgets(genbuf, STRLEN, fn) != NULL)
						fputs(genbuf, freg);
					fclose(freg);
				}
				break;
			case 'N':
			case 'n':
				for(total=0;total<8;total++)
					realnumofrea[total]=0;
				for (n = 0; field[n] != NULL; n++)
					prints("%s: %s\n", finfo[n], fdata[n]);
				printdash(NULL);
				move(9, 0);
				prints("请依次输入退回申请表原因的序号并回车确认,直接回车结束.\n\n");
				//	prints("请选择/输入退回申请表原因, 按 <enter> 取消.\n\n");
				for (n = 0; reason[n] != NULL; n++)
					prints("%d) %s\n", n, reason[n]);
				getdata(12+n,0,"退回原因:",numofreason,2,DOECHO,YEA);
				//	getdata(12 + n, 0, "退回原因: ", buf, 60, DOECHO, YEA);
				if(numofreason[0]!='\0'&&numofreason[0]>='0'&&numofreason[0]<='7')
				{
					total=atoi(numofreason);
					realnumofrea[total]=1;
					move(14+n,0);
					prints("您已经选择的理由有:\n%d  ",total);
				}
				while(numofreason[0]!='\0')
				{
					getdata(12+n,0,"退回原因:",numofreason,2,DOECHO,YEA);
					if(numofreason[0]>='0'&&numofreason[0]<='7')
					{
						total=atoi(numofreason);
						if(realnumofrea[total]==0)
							realnumofrea[total]=1;
						else    
							realnumofrea[total]=0;
						move(14+n,0);
						clrtobot();
						prints("您已经选择的理由有:\n");
						for(iis=0;iis<8;iis++)
						{
							if(realnumofrea[iis]==1)
								prints("%d  ",iis);
						}
					}
				}
				system("cp /home/bbs/etc/f_fill  /home/bbs/tmp/fletter");
				char FL[STRLEN];
				sprintf(FL,"/home/bbs/tmp/fletter");
				FILE * fail=fopen(FL,"a");
				if(fail==NULL)
				{
					prints("Error,创建临时文件错误.请与系统维护联系或再次尝试!");
					pressanykey();
					return -1;
				}

				{
					fprintf(fail,"\n\n您被拒绝注册的原因大概如下:\n");
					for(total=0;total<8;total++)
					{
						if(realnumofrea[total]==1)
						{
							strcpy(buf,reason[total]);
							fprintf(fail,"\n%s",buf);
						}
					}
					sprintf(genbuf,"很抱歉的告诉您,您这次注册失败.:(");
					strncpy(uinfo.address, genbuf, NAMELEN);
					substitute_record(PASSFILE, &uinfo, sizeof(uinfo), unum);
					fclose(fail);
					mail_file(FL,uinfo.userid, uinfo.address);
					system("rm -f /home/bbs/tmp/fletter");

				}
				break;
				/*if (buf[0] != '\0') {
				if (buf[0] >= '0' && buf[0] < '0' + n) {
					strcpy(buf, reason[buf[0] - '0']);
				}
				sprintf(genbuf, "<注册失败> - %s", buf);
				strncpy(uinfo.address, genbuf, NAMELEN);
				substitute_record(PASSFILE, &uinfo, sizeof(uinfo), unum);
				mail_file("etc/f_fill", uinfo.userid, uinfo.address);*/
				/* user_display( &uinfo, 1 ); */
				/* pressreturn(); */
				/*	break;
				}
				move(10, 0);
				clrtobot();
				prints("取消退回此注册申请表.\n");*/
				/* run default -- put back to regfile */
			default:
				if ((freg = fopen(regfile, "a")) != NULL)
				{
					for (n = 0; field[n] != NULL; n++)
						fprintf(freg, "%s: %s\n", field[n], fdata[n]);
					fprintf(freg, "----\n");
					fclose(freg);
				}
			}
			memset(fdata, 0, sizeof(fdata));
		}
	}
	fclose(fn);
	unlink(fname);
	return (0);
}

#ifndef WITHOUT_ADMIN_TOOLS
extern int cmpuids();
extern int t_cmpuids();
int kick_user( struct user_info *userinfo )
{
	int     id, ind;
	struct user_info uin;
	struct userec kuinfo;
	char    kickuser[40], buffer[40];

	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND)
	{
		modify_user_mode(ADMIN);
		stand_title("踢使用者下站");
		move(1, 0);
		usercomplete("输入使用者帐号: ", kickuser);
		if (*kickuser == '\0')
		{
			clear();
			return 0;
		}
		if (!(id = getuser(kickuser)))
		{
			move(3, 0);
			prints("无效的用户 ID！");
			clrtoeol();
			pressreturn();
			clear();
			return 0;
		}
		move(1, 0);
		clrtoeol();
		sprintf(genbuf,"踢掉使用者 : [%s].", kickuser);
		move(2, 0);
		if (askyn(genbuf, NA, NA) == NA)
		{
			move(2, 0);
			prints("取消踢使用者..\n");
			pressreturn();
			clear();
			return 0;
		}
		search_record(PASSFILE, &kuinfo, sizeof(kuinfo), cmpuids, kickuser);
		ind = search_ulist(&uin, t_cmpuids, id);
	}
	else
	{
		uin = *userinfo;
		strcpy(kickuser, uin.userid);
		ind = YEA;
	}
	if (!ind||!uin.active||(uin.pid && kill(uin.pid, 0)==-1))
	{
		if(uinfo.mode!=LUSERS&&uinfo.mode!=OFFLINE&&uinfo.mode!=FRIEND)
		{
			move(3, 0);
			prints("用户 [%s] 不在线上",kickuser);
			clrtoeol();
			pressreturn();
			clear();
		}
		return 0;
	}
	kill(uin.pid, SIGHUP);
	//sprintf(buffer, "kicked %s", kickuser);
	//report(buffer);
	if(strcmp(currentuser.userid , kickuser))
	{
		char    secu[STRLEN];
		sprintf(secu, "将用户[%s]踢下站", kickuser);
		securityreport(secu);
	}
	sprintf(genbuf, "%s (%s)", kuinfo.userid, kuinfo.username);
	log_usies("KICK ", genbuf);
	move(2, 0);
	if(uinfo.mode!=LUSERS&&uinfo.mode!=OFFLINE&&uinfo.mode!=FRIEND)
	{
		prints("用户 [%s] 已经被踢下站.\n",kickuser);
		pressreturn();
		clear();
	}
	return 1;
}
#endif
