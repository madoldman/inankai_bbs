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
$Id: vote.c,v 1.4 2008-12-26 06:45:55 madoldman Exp $
*/

#include "bbs.h"
#include "vote.h"

extern  cmpbnames();
extern int page, range;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
static char *vote_type[] = {"是非", "单选", "复选", "数字", "问答"};
struct votebal currvote;
char    controlfile[STRLEN];
unsigned int result[33];
int     vnum;
int     voted_flag;
FILE   *sug;
int makevote(struct votebal *ball, char *bname);
int setvoteperm(struct votebal *ball);

int cmpvuid(char* userid, struct ballot* uv)
{
	return !strcmp(userid, uv->uid);
}

int setvoteflag(int bname, int flag)
{
	int     pos;
	struct boardheader fh;
	pos = search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname);
	if(  ((fh.flag&VOTE_FLAG)&&(flag !=0))
	        ||((fh.flag&VOTE_FLAG==0)&&(flag ==0)))
		return ;
	if (flag == 0)
		fh.flag = fh.flag & ~VOTE_FLAG;
	else
		fh.flag = fh.flag | VOTE_FLAG;
	if (substitute_record(BOARDS, &fh, sizeof(fh), pos) == -1)
		prints("Error updating BOARDS file...\n");
}

void b_report(char* str)
{
	char    buf[STRLEN];
	sprintf(buf, "%s %s", currboard, str);
	report(buf);
}

void makevdir(char* bname)
{
	struct stat st;
	char    buf[STRLEN];
	sprintf(buf, "vote/%s", bname);
	if (stat(buf, &st) != 0)
		mkdir(buf, 0755);
}

void setvfile(char* buf, char* bname, char* filename)
{
	sprintf(buf, "vote/%s/%s", bname, filename);
}

void setcontrolfile()
{
	setvfile(controlfile, currboard, "control");
}

int b_notes_edit()
{
	char    buf[STRLEN], buf2[STRLEN];
	char    ans[4];
	int     aborted, notetype;

	if (!chk_currBM(currBM) && !HAS_PERM(PERM_WELCOME))
		return 0;
	clear();
	move(1, 0);
	prints("编辑/删除本板备忘录");
	getdata(3,0,"编辑或删除 (0)离开 (1)一般备忘录 (2)秘密备忘录? [1] ",
	        ans, 2, DOECHO, YEA);
	if (ans[0] == '0')
		return FULLUPDATE;
	if (ans[0] != '2')
	{
		setvfile(buf, currboard, "notes");
		notetype = 1;
	}
	else
	{
		setvfile(buf, currboard, "secnotes");
		notetype = 2;
	}
	sprintf(buf2, "[%s] (E)编辑 (D)删除 (A)取消 [E]: ",
	        (notetype==1)?"一般备忘录":"秘密备忘录");
	getdata(5, 0, buf2, ans, 2, DOECHO, YEA);
	if(ans[0] == 'A' || ans[0] == 'a')
	{
		aborted = -1;
	}
	else if (ans[0] == 'D' || ans[0] == 'd')
	{
		move(6, 0);
		sprintf(buf2, "真的要删除[%s]",(notetype==1)?"一般备忘录":"秘密备忘录");
		if (askyn(buf2, NA, NA) == YEA)
		{
			move(7, 0);
			unlink(buf);
			prints("[%s]已经删除...\n",(notetype==1)?"一般备忘录": "秘密备忘录");
			pressanykey();
			aborted = 1;
		}
		else
			aborted = -1;
	}
	else
		aborted = vedit(buf, NA, YEA);
	if (aborted == -1)
	{
		pressreturn();
	}
	else
	{
		if (notetype == 1)
			setvfile(buf, currboard, "noterec");
		else if(notetype == 2)
			setvfile(buf, currboard, "notespasswd");
		unlink(buf);
	}
	return FULLUPDATE;
}

int b_notes_passwd()
{
	FILE   *pass;
	char    passbuf[20], prepass[20];
	char    buf[STRLEN];
	if (!chk_currBM(currBM))
	{
		return 0;
	}
	clear();
	move(1, 0);
	prints("设定/更改/取消「秘密备忘录」密码...");
	setvfile(buf, currboard, "secnotes");
	if (!dashf(buf))
	{
		move(3, 0);
		prints("本讨论区尚无「秘密备忘录」。\n\n");
		prints("请先用 W 编好「秘密备忘录」再来设定密码...");
		pressanykey();
		return FULLUPDATE;
	}
	if (!check_notespasswd())
		return FULLUPDATE;
	getdata(3, 0, "请输入新的秘密备忘录密码(Enter 取消密码): ", passbuf, 19, NOECHO, YEA);
	if(passbuf[0] == '\0')
	{
		setvfile(buf, currboard, "notespasswd");
		unlink(buf);
		prints("已经取消备忘录密码。");
		pressanykey();
		return FULLUPDATE;
	}
	getdata(4, 0, "确认新的秘密备忘录密码: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
	{
		prints("\n密码不相符, 无法设定或更改....");
		pressanykey();
		return FULLUPDATE;
	}
	setvfile(buf, currboard, "notespasswd");
	if ((pass = fopen(buf, "w")) == NULL)
	{
		move(5, 0);
		prints("备忘录密码无法设定....");
		pressanykey();
		return FULLUPDATE;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(5, 0);
	prints("秘密备忘录密码设定完成....");
	pressanykey();
	return FULLUPDATE;
}

int b_suckinfile(FILE* fp, char* fname)
{
	char    inbuf[256];
	FILE   *sfp;
	if ((sfp = fopen(fname, "r")) == NULL)
		return -1;
	while (fgets(inbuf, sizeof(inbuf), sfp) != NULL)
		fputs(inbuf, fp);
	fclose(sfp);
	return 0;
}
/*Add by SmallPig*/
int catnotepad(FILE* fp, char* fname)
{
	char    inbuf[256];
	FILE   *sfp;
	int     count;
	count = 0;
	if ((sfp = fopen(fname, "r")) == NULL)
	{
		fprintf(fp, "[1;34m  □[44m__________________________________________________________________________[m \n\n");
		return -1;
	}
	while (fgets(inbuf, sizeof(inbuf), sfp) != NULL)
	{
		if (count != 0)
			fputs(inbuf, fp);
		else
			count++;
	}
	fclose(sfp);
	return 0;
}

int b_closepolls()
{
	char    buf[80];
	time_t  now, nextpoll;
	int     i, end;
	now = time(0);
	resolve_boards();


	if (now < brdshm->pollvote)
	{
		return;
	}
	move(t_lines - 1, 0);
	prints("对不起，系统关闭投票中，请稍候...");
	refresh();
	nextpoll = now + 7 * 3600;
	brdshm->pollvote = nextpoll;
	strcpy(buf, currboard);
	//for (i = 0; i < brdshm->number; i++)
	for (i = 0; i <= brdshm->number; i++)
	{
		if(i<brdshm->number)
			strcpy(currboard, (&bcache[i])->filename);
		else
			strcpy(currboard,"NanKaiBBS");//全站投票自动结束
		setcontrolfile();
		end = get_num_records(controlfile, sizeof(currvote));
		for (vnum = end; vnum >= 1; vnum--)
		{
			time_t  closetime;
			get_record(controlfile, &currvote, sizeof(currvote), vnum);
			closetime = currvote.opendate + currvote.maxdays * 86400;
			if (now > closetime)
				mk_result(vnum);
			else if (nextpoll > closetime)
			{
				nextpoll = closetime + 300;
				brdshm->pollvote = nextpoll;
			}
		}
	}
	strcpy(currboard, buf);

	return 0;
}

int count_result(struct ballot* ptr)
{
	int     i;
	/*	if (ptr == NULL) {
			if (sug != NULL) {
				fclose(sug);
				sug == NULL;
			}
			return 0;
		}
	*/	
	if (ptr->msg[0][0] != '\0')
	{
		if (currvote.type == VOTE_ASKING)
		{
			fprintf(sug, "[1m%s [m的作答如下：\n", ptr->uid);
		}
		else
			fprintf(sug, "[1m%s [m的建议如下：\n", ptr->uid);
		for (i = 0; i < 3; i++)
			fprintf(sug, "%s\n", ptr->msg[i]);
	}
	result[32]++;
	if (currvote.type == VOTE_ASKING)
	{
		return 0;
	}
	if (currvote.type != VOTE_VALUE)
	{
		for (i = 0; i < 32; i++)
		{
			if ((ptr->voted >> i) & 1)
				(result[i])++;
		}

	}
	else
	{
		result[31] += ptr->voted;
		result[(ptr->voted * 10) / (currvote.maxtkt + 1)]++;
	}
	return 0;
}
get_result_title()
{
	char    buf[STRLEN];
	getdatestring(currvote.opendate,NA);
	fprintf(sug, "⊙ 投票开启于：[1m%s[m  类别：[1m%s[m\n",
	        datestring,vote_type[currvote.type - 1]);
	fprintf(sug, "⊙ 主题：[1m%s[m\n", currvote.title);
	if (currvote.type == VOTE_VALUE)
		fprintf(sug, "⊙ 此次投票的值不可超过：[1m%d[m\n\n", currvote.maxtkt);
	fprintf(sug, "⊙ 票选题目描述：\n\n");
	sprintf(buf, "vote/%s/desc.%d", currboard, currvote.opendate);
	b_suckinfile(sug, buf);
}

int mk_result(int num)
{
	char    fname[STRLEN], nname[STRLEN];
	char    sugname[STRLEN];
	char    title[STRLEN];
	int     i;
	unsigned int total = 0;
	struct boardheader fh;
	setcontrolfile();
	sprintf(fname, "vote/%s/flag.%d", currboard, currvote.opendate);
	/*	count_result(NULL); */
	sug = NULL;
	sprintf(sugname, "vote/%s/tmp.%d", currboard, uinfo.pid);
	if ((sug = fopen(sugname, "w")) == NULL)
	{
		report("open vote tmp file error");
		prints("Error: 结束投票错误...\n");
		pressanykey();
	}
	(void) memset(result, 0, sizeof(result));
	if (apply_record(fname, count_result, sizeof(struct ballot)) == -1)
	{
		report("Vote apply flag error");
	}
	fprintf(sug, "[1;44;36m——————————————┤使用者%s├——————————————[m\n\n\n", (currvote.type != VOTE_ASKING) ? "建议或意见" : "此次的作答");
	fclose(sug);
	sprintf(nname, "vote/%s/results", currboard);
	if ((sug = fopen(nname, "w")) == NULL)
	{
		report("open vote newresult file error");
		prints("Error: 结束投票错误...\n");
	}
	get_result_title(sug);

	fprintf(sug, "** 投票结果:\n\n");
	if (currvote.type == VOTE_VALUE)
	{
		total = result[32];
		for (i = 0; i < 10; i++)
		{
			fprintf(sug, "[1m  %4d[m 到 [1m%4d[m 之间有 [1m%4d[m 票  约占 [1m%d%%[m\n",
			        (i * currvote.maxtkt) / 10 + ((i == 0) ? 0 : 1), ((i + 1) * currvote.maxtkt) / 10, result[i]
			        ,(result[i] * 100) / ((total <= 0) ? 1 : total));
		}
		fprintf(sug, "此次投票结果平均值是: [1m%d[m\n", result[31] / ((total <= 0) ? 1 : total));
	}
	else if (currvote.type == VOTE_ASKING)
	{
		total = result[32];
	}
	else
	{
		for (i = 0; i < currvote.totalitems; i++)
		{
			total += result[i];
		}
		for (i = 0; i < currvote.totalitems; i++)
		{
			fprintf(sug, "(%c) %-40s  %4d 票  约占 [1m%d%%[m\n", 'A' + i, currvote.items[i], result[i], (result[i] * 100) / ((total <= 0) ? 1 : total));
		}
	}
	fprintf(sug, "\n投票总人数 = [1m%d[m 人\n", result[32]);
	fprintf(sug, "投票总票数 =[1m %d[m 票\n\n", total);
	fprintf(sug, "[1;44;36m——————————————┤使用者%s├——————————————[m\n\n\n", (currvote.type != VOTE_ASKING) ? "建议或意见" : "此次的作答");
	b_suckinfile(sug, sugname);
	unlink(sugname);
	fclose(sug);

	sprintf(title, "[公告] %s 板的投票结果", currboard);
/*
clear();        
prints("%s",genbuf);
pressanykey();
*/

		search_record(BOARDS, &fh, sizeof(fh), cmpbnames, currboard);
        setbfile(genbuf, currboard, "board.allow");
        if(!dashf(genbuf) && (fh.level==0))
			if(strcmp (currboard, "Headquarters"))
				Postfile(nname, "vote", title, 1);
/*	
clear();
prints("%s %s",genbuf,currboard);
pressanykey();
*/	
	if (strcmp (currboard, "vote") )
		Postfile(nname, currboard, title, 1);
	dele_vote(num);
	return;
}

int get_vitems(struct votebal* bal)
{
	int     num;
	char    buf[STRLEN];
	move(3, 0);
	prints("请依序输入可选择项, 按 ENTER 完成设定.\n");
	num = 0;
	for (num = 0; num < 32; num++)
	{
		sprintf(buf, "%c) ", num + 'A');
		getdata((num % 16) + 4, (num / 16) * 40, buf, bal->items[num], 36, DOECHO, YEA);
		if (strlen(bal->items[num]) == 0)
		{
			if (num != 0)
				break;
			num = -1;
		}
	}
	bal->totalitems = num;
	return num;
}

int vote_maintain(char *bname)
{
	char    buf[STRLEN],ans[2];
	struct boardheader fh;
	struct votebal *ball = &currvote;
	setcontrolfile();
	if (!HAS_PERM(PERM_OVOTE))
		if (!chk_currBM(currBM))
			return 0;
	stand_title("开启投票箱");
	makevdir(bname);
	for (;;)
	{
		getdata(2, 0, "(1)是非 (2)单选 (3)复选 (4)数值 (5)问答 (6)取消 ? [6]",
		        ans, 2, DOECHO, YEA);
		ans[0] -= '0';
		if (ans[0] < 1 || ans[0] > 5)
		{
			prints("取消此次投票\n");
			return FULLUPDATE;
		}
		ball->type = (int) ans[0];
		break;
	}
	ball->opendate = time(0);
	if(makevote(ball,bname))
		return FULLUPDATE;
	setvoteflag(currboard, 1);
	clear();
	strcpy(ball->userid, currentuser.userid);
	if (append_record(controlfile, ball, sizeof(*ball)) == -1)
	{
		prints("发生严重的错误，无法开启投票，请通告站长");
		b_report("Append Control file Error!!");
	}
	else
	{
		char    votename[STRLEN];
		int     i;
		b_report("OPEN");
		prints("投票箱开启了！\n");
		range++;
		sprintf(votename, "tmp/votetmp.%s.%05d", currentuser.userid, uinfo.pid);
		if ((sug = fopen(votename, "w")) != NULL)
		{
			sprintf(buf, "[通知] %s 举办投票：%s", currboard, ball->title);
			get_result_title(sug);
			if (ball->type != VOTE_ASKING && ball->type != VOTE_VALUE)
			{
				fprintf(sug, "\n【[1m选项如下[m】\n");
				for (i = 0; i < ball->totalitems; i++)
				{
					fprintf(sug, "([1m%c[m) %-40s\n", 'A' + i, ball->items[i]);
				}
			}
			fclose(sug);
			search_record(BOARDS, &fh, sizeof(fh), cmpbnames, currboard);
                setbfile(genbuf, currboard, "board.allow");
                        if(!dashf(genbuf) && fh.level == 0 && strcmp(currboard,"Headquarters") )
				Postfile(votename, "vote", buf, 1);
			unlink(votename);
		}
	}
	pressreturn();
	return FULLUPDATE;
}


int makevote(struct votebal *ball, char *bname)
{
	char    buf[STRLEN];
	int     aborted;

	prints("请按任何键开始编辑此次 [投票的描述]: \n");
	igetkey();
	setvfile(genbuf, bname, "desc");
	sprintf(buf, "%s.%d", genbuf, ball->opendate);
	aborted = vedit(buf, NA, YEA);
	if (aborted)
	{
		clear();
		prints("取消此次投票设定\n");
		pressreturn();
		return 1;
	}
	clear();
	getdata(0, 0, "此次投票所须天数[1]: ", buf, 3, DOECHO, YEA);
	if (*buf == '\n' || atoi(buf) == 0 || *buf == '\0')
		ball->maxdays = 1;
	else
		ball->maxdays = atoi(buf);
	while(1)
	{
		getdata(1, 0, "投票箱的标题: ", ball->title, 51, DOECHO, YEA);
		if (killwordsp(ball->title)!=0)
			break;
		bell();
	}
	switch (ball->type)
	{
	case VOTE_YN:
		ball->maxtkt = 0;
		strcpy(ball->items[0], "赞成  （是的）");
		strcpy(ball->items[1], "不赞成（不是）");
		strcpy(ball->items[2], "没意见（不清楚）");
		ball->maxtkt = 1;
		ball->totalitems = 3;
		break;
	case VOTE_SINGLE:
		get_vitems(ball);
		ball->maxtkt = 1;
		break;
	case VOTE_MULTI:
		get_vitems(ball);
		for (;;)
		{
			getdata(21, 0, "一个人最多几票? [1]: ", buf, 5, DOECHO, YEA);
			ball->maxtkt = atoi(buf);
			if (ball->maxtkt <= 0)
				ball->maxtkt = 1;
			if (ball->maxtkt > ball->totalitems)
				continue;
			break;
		}
		break;
	case VOTE_VALUE:
		for (;;)
		{
			getdata(3, 0, "输入数值最大不得超过 [100] : ", buf, 4, DOECHO, YEA);
			ball->maxtkt = atoi(buf);
			if (ball->maxtkt <= 0)
				ball->maxtkt = 100;
			break;
		}
		break;
	case VOTE_ASKING:
		ball->maxtkt = 0;
		currvote.totalitems = 0;
		break;
	default:
		ball->maxtkt = 1;
		break;
	}
	return 0;
}

int setvoteperm(struct votebal *ball)
{
	int  showperminfo(int, int);
	int	flag = NA, changeit = YEA;
	char buf[6],msgbuf[STRLEN];

	if(!HAS_PERM(PERM_OVOTE))
		return ;
	clear();
	prints("当前投票标题：[\033[1;33m%s\033[m]",ball->title);
	prints("\n您是本站的投票管理员，您可以对投票进行投票受限设定\n");
	prints("\n投票受限设定共有三种限制，这三种限制可以做综合限定。\n");
	prints("    (1) 权限限制 [\033[32m用户必须拥有某些权限才可以投票\033[m]\n");
	prints("    (2) 投票名单 [\033[35m存在于投票名单中的人才可以投票\033[m]\n");
	prints("    (3) 条件审核 [\033[36m用户必须满足一定的条件才可投票\033[m]\n");
	prints("\n下面系统将一步一步引导您来设置投票受限设定。\n\n");
	if(askyn("您确定要开始对该投票进行投票受限设定吗",NA,NA)==NA)
		return 0;
	flag = (ball->level & ~(LISTMASK | VOTEMASK))?YEA:NA;
	prints("\n\(\033[36m1\033[m) 权限限制  [目前状态: \033[32m%s\033[m]\n\n",
	       flag ?"有限制":"无限制");
	if(flag == YEA)
	{
		if(askyn("需要取消投票者的权限限制吗",NA,NA) == YEA)
			flag = NA;
		else
			changeit = askyn("您需要修改投票者的权限限制吗",NA,NA);
	}
	else
		flag = askyn("您希望投票者必须具备某种权限吗",NA,NA);
	if(flag == NA)
	{
		ball->level = ball->level&(LISTMASK | VOTEMASK);
		prints("\n\n当前投票\033[32m无权限限制\033[m！");
	}
	else if(changeit == NA)
	{
		prints("\n当前投票\033[32m【保留】权限限制\033[m！");
	}
	else
	{
		clear();
		prints("\n设定\033[32m投票者必需\033[m的权限。");
		ball->level = setperms(ball->level, "投票权限", NUMPERMS, showperminfo);
		move(1,0);
		if(ball->level & ~(LISTMASK | VOTEMASK))
			prints("您已经\033[32m设定了\033[m投票的必需权限，系统接受您的设定");
		else
			prints("您现在\033[32m取消了\033[m投票的权限限定，系统接受您的设定");
	}
	prints("\n\n\n\033[33m【下一步】\033[m将继续进行投票限制的设定\n");
	pressanykey();
	clear();
	flag = (ball->level & LISTMASK)?YEA:NA;
	prints("\n(\033[36m2\033[m) 投票名单  [目前状态: \033[32m%s\033[m]\n\n",
	       flag ?"在名单中的 ID 才可投票":"不受名单限制");
	if(askyn("您想改变本项设定吗",NA,NA)==YEA)
	{
		if(flag)
		{
			ball->level &= ~LISTMASK;
			prints("\n您的设定：\033[32m该投票不受投票名单的限定\033[m\n");
		}
		else
		{
			ball->level |= LISTMASK;
			prints("\n您的设定：\033[32m只有在名单中的 ID 才可以投票\033[m\n");
			prints("[\033[33m提示\033[m]设定完毕后，用 Ctrl+E 来编辑投票名单\n");
		}
	}
	else
	{
		prints("\n您的设定：\033[32m[保留设定]在名单中的 ID 才可以投票\033[m\n");
	}
	flag = (ball->level & VOTEMASK)?YEA:NA;
	prints("\n(\033[36m3\033[m) 条件审核  [目前状态: \033[32m%s\033[m]\n\n",
	       flag ?"受条件限制":"不受条件限制");
	if(flag == YEA)
	{
		if(askyn("您想取消投票者的上站次数、上站时间等的限制吗",NA,NA)==YEA)
			flag = NA;
		else
			changeit = askyn("您想重新设定条件限制吗",NA,NA);
	}
	else
	{
		changeit = YEA;
		flag = askyn("投票者是否需要受到上站次数等条件的限制",NA,NA);
	}
	if(flag == NA)
	{
		ball->level &= ~VOTEMASK;
		prints("\n您的设定：\033[32m不受条件限定\033[m\n\n投票受限设定完毕\n\n");
	}
	else if( changeit == NA)
	{
		prints("\n您的设定：\033[32m保留条件限定\033[m\n\n投票受限设定完毕\n\n");
	}
	else
	{
		ball->level |= VOTEMASK;
		sprintf(msgbuf,"上站次数至少达到多少次？[%d]",ball->x_logins);
		getdata(11,4,msgbuf,buf,5,DOECHO,YEA);
		if(buf[0]!='\0')
			ball->x_logins = atoi(buf);
		sprintf(msgbuf,"发表的文章至少有多少篇？[%d]",ball->x_posts);
		getdata(12,4,msgbuf,buf,5,DOECHO,YEA);
		if(buf[0] != '\0')
			ball->x_posts = atoi(buf);
		sprintf(msgbuf,"在本站的累计上站时间至少有多少小时？[%d]",ball->x_stay);
		getdata(13,4,msgbuf,buf,5,DOECHO,YEA);
		if(buf[0] != '\0')
			ball->x_stay = atoi(buf);
		sprintf(msgbuf,"该帐号注册时间至少有多少天？[%d]",ball->x_live);
		getdata(14,4,msgbuf,buf,5,DOECHO,YEA);
		if(buf[0] != '\0')
			ball->x_live = atoi(buf);
		prints("\n投票受限设定完毕\n\n");
	}
	if(askyn("您确定要修改投票受限吗",NA,NA)==YEA)
	{
		sprintf(msgbuf,"修改 %s 板投票[投票受限]",currboard);
		security_report(msgbuf,2);
		return 1;
	}
	else
		return 0;
}


int vote_flag(char* bname, char val, int mode)
{
	char    buf[STRLEN], flag;
	int     fd, num, size;
	num = usernum - 1;
	switch (mode)
	{
	case 2:
		sprintf(buf, "Welcome.rec");	/* 进站的 Welcome 画面 */
		break;
	case 1:
		setvfile(buf, bname, "noterec");	/* 讨论区备忘录的旗标 */
		break;
	default:
		return -1;
	}
	if (num >= MAXUSERS)
	{
		report("Vote Flag, Out of User Numbers");
		return -1;
	}
	if ((fd = open(buf, O_RDWR | O_CREAT, 0600)) == -1)
	{
		return -1;
	}
	flock(fd, LOCK_EX);
	size = (int) lseek(fd, 0, SEEK_END);
	memset(buf, 0, sizeof(buf));
	while (size <= num)
	{
		write(fd, buf, sizeof(buf));
		size += sizeof(buf);
	}
	lseek(fd, (off_t) num, SEEK_SET);
	read(fd, &flag, 1);
	if ((flag == 0 && val != 0))
	{
		lseek(fd, (off_t) num, SEEK_SET);
		write(fd, &val, 1);
	}
	flock(fd, LOCK_UN);
	close(fd);
	return flag;
}


int vote_check(int bits)
{
	int     i, count;
	for (i = count = 0; i < 32; i++)
	{
		if ((bits >> i) & 1)
			count++;
	}
	return count;
}

int showvoteitems(unsigned int pbits, int i, int flag)
{
	char    buf[STRLEN];
	int     count;
	if (flag == YEA)
	{
		count = vote_check(pbits);
		if (count > currvote.maxtkt)
			return NA;
		move(2, 0);
		clrtoeol();
		prints("您已经投了 [1m%d[m 票", count);
	}
	sprintf(buf, "%c.%2.2s%-36.36s", 'A' + i,
	        ((pbits >> i) & 1 ? "√" : "  "), currvote.items[i]);
	move(i + 6 - ((i > 15) ? 16 : 0), 0 + ((i > 15) ? 40 : 0));
	prints(buf);
	refresh();
	return YEA;
}

void show_voteing_title()
{
	time_t  closedate;
	char    buf[STRLEN];
	if (currvote.type != VOTE_VALUE && currvote.type != VOTE_ASKING)
		sprintf(buf, "可投票数: [1m%d[m 票", currvote.maxtkt);
	else
		buf[0] = '\0';
	closedate = currvote.opendate + currvote.maxdays * 86400;
	getdatestring(closedate,NA);
	prints("投票将结束于: [1m%s[m  %s  %s\n",
	       datestring, buf, (voted_flag) ? "([5;1m修改前次投票[m)" : "");
	prints("投票主题是: [1m%-50s[m类型: [1m%s[m \n", currvote.title, vote_type[currvote.type - 1]);
}

int getsug(struct ballot* uv)
{
	int     i, line;
	move(0, 0);
	clrtobot();
	if (currvote.type == VOTE_ASKING)
	{
		show_voteing_title();
		line = 3;
		prints("请填入您的作答(三行):\n");
	}
	else
	{
		line = 1;
		prints("请填入您宝贵的意见(三行):\n");
	}
	move(line, 0);
	for (i = 0; i < 3; i++)
	{
		prints(": %s\n", uv->msg[i]);
	}
	for (i = 0; i < 3; i++)
	{
		getdata(line + i, 0, ": ", uv->msg[i], STRLEN - 2, DOECHO, NA);
		if (uv->msg[i][0] == '\0')
			break;
	}
	return i;
}


int multivote(struct ballot* uv)
{
	unsigned int i;
	i = uv->voted;
	move(0, 0);
	show_voteing_title();
	uv->voted = setperms(uv->voted, "选票", currvote.totalitems, showvoteitems);
	if (uv->voted == i)
		return -1;
	return 1;
}

int valuevote(struct ballot* uv)
{
	unsigned int chs;
	char    buf[10];
	chs = uv->voted;
	move(0, 0);
	show_voteing_title();
	prints("此次作答的值不能超过 [1m%d[m", currvote.maxtkt);
	if (uv->voted != 0)
		sprintf(buf, "%d", uv->voted);
	else
		memset(buf, 0, sizeof(buf));
	do
	{
		getdata(3, 0, "请输入一个值? [0]: ", buf, 5, DOECHO, NA);
		uv->voted = abs(atoi(buf));
	}
	while (uv->voted > currvote.maxtkt && buf[0] != '\n' && buf[0] != '\0');
	if (buf[0] == '\n' || buf[0] == '\0' || uv->voted == chs)
		return -1;
	return 1;
}

int user_vote(int num)
{
	char    fname[STRLEN], bname[STRLEN];
	char    buf[STRLEN];
	struct ballot uservote, tmpbal;
	int     votevalue, result = NA;
	int     aborted = NA, pos;

	move(t_lines - 2, 0);
	get_record(controlfile, &currvote, sizeof(struct votebal), num);
	if (currentuser.firstlogin > currvote.opendate)
	{
		prints("对不起, 本投票在您帐号申请之前开启，您不能投票\n");
	}
	else if(!HAS_PERM(currvote.level&~(LISTMASK|VOTEMASK)))
	{
		prints("对不起，您目前尚无权在本票箱投票\n");
	}
	else if(currvote.level & LISTMASK )
	{
		char listfilename[STRLEN];
		setvfile(listfilename, currboard, "vote.list");
		if(!dashf(listfilename))
			prints("对不起，本票箱需要设定好投票名册方可进行投票\n");
		else if(!seek_in_file(listfilename,currentuser.userid))
			prints("对不起, 投票名册上找不到您的大名\n");
		else
			result = YEA;
	}
	else if(currvote.level & VOTEMASK )
	{
		if(  currentuser.numlogins < currvote.x_logins
		        ||currentuser.numposts < currvote.x_posts
		        ||currentuser.stay < currvote.x_stay * 3600
		        ||currentuser.firstlogin>currvote.opendate-currvote.x_live*86400)
			prints("对不起，您目前尚不够资格在本票箱投票\n");
		else
			result = YEA;
	}
	else
		result = YEA;
	if(result == NA)
	{
		pressanykey();
		return;
	}
	sprintf(fname, "vote/%s/flag.%d", currboard, currvote.opendate);
	if ((pos = search_record(fname, &uservote, sizeof(uservote),
	                         cmpvuid, currentuser.userid)) <= 0)
	{
		(void) memset(&uservote, 0, sizeof(uservote));
		voted_flag = NA;
	}
	else
	{
		voted_flag = YEA;
	}
	strcpy(uservote.uid, currentuser.userid);
	sprintf(bname, "desc.%d", currvote.opendate);
	setvfile(buf, currboard, bname);
	ansimore(buf, YEA);
	move(0, 0);
	clrtobot();
	switch (currvote.type)
	{
	case VOTE_SINGLE:
	case VOTE_MULTI:
	case VOTE_YN:
		votevalue = multivote(&uservote);
		if (votevalue == -1)
			aborted = YEA;
		break;
	case VOTE_VALUE:
		votevalue = valuevote(&uservote);
		if (votevalue == -1)
			aborted = YEA;
		break;
	case VOTE_ASKING:
		uservote.voted = 0;
		aborted = !getsug(&uservote);
		break;
	}
	clear();
	if (aborted == YEA)
	{
		prints("保留 【[1m%s[m】原来的的投票。\n", currvote.title);
	}
	else
	{
		if (currvote.type != VOTE_ASKING)
			getsug(&uservote);
		pos = search_record(fname, &tmpbal, sizeof(tmpbal),
		                    cmpvuid, currentuser.userid);
		if (pos)
		{
			substitute_record(fname, &uservote, sizeof(uservote), pos);
		}
		else if (append_record(fname, &uservote, sizeof(uservote)) == -1)
		{
			move(2, 0);
			clrtoeol();
			prints("投票失败! 请通知站长参加那一个选项投票\n");
			pressreturn();
		}
		prints("\n已经帮您投入票箱中...\n");
	}
	pressanykey();
	return;
}

voteexp()
{
	clrtoeol();
	prints("[1;44m编号 开启投票箱者 开启日 %-40s类别 天数 人数[m\n", "投票主题");
}

int printvote(struct votebal* ent)
{
	static int i;
	struct ballot uservote;
	char    buf[STRLEN + 10];
	char    flagname[STRLEN];
	int     num_voted;
	if (ent == NULL)
	{
		move(2, 0);
		voteexp();
		i = 0;
		return 0;
	}
	i++;
	if (i > page + 20 || i > range)
		return QUIT;
	else if (i <= page)
		return 0;
	sprintf(buf, "flag.%d", ent->opendate);
	setvfile(flagname, currboard, buf);
	if (search_record(flagname, &uservote, sizeof(uservote), cmpvuid,
	                  currentuser.userid) <= 0)
	{
		voted_flag = NA;
	}
	else
		voted_flag = YEA;
	num_voted = get_num_records(flagname, sizeof(struct ballot));
	getdatestring(ent->opendate,NA);
	sprintf(buf, " %s%3d %-12.12s %6.6s%s%-40.40s%-4.4s %3d  %4d[m\n", (voted_flag == NA) ? "[1m" : "", i, ent->userid,
	        datestring+6, ent->level ? "\033[33ms\033[37m":" ",ent->title, vote_type[ent->type - 1], ent->maxdays, num_voted);
	prints("%s", buf);
}

int dele_vote(int num)
{
	char    buf[STRLEN];
	sprintf(buf, "vote/%s/flag.%d", currboard, currvote.opendate);
        if(strcmp(currboard,"FameHall")){
      	unlink(buf);
	sprintf(buf, "vote/%s/desc.%d", currboard, currvote.opendate);
	unlink(buf);
        }

	if (delete_record(controlfile, sizeof(currvote), num) == -1)
	{
		prints("发生错误，请通知站长....");
		pressanykey();
	}
	range--;
	if (get_num_records(controlfile, sizeof(currvote)) == 0)
	{
		setvoteflag(currboard, 0);
	}
}

int vote_results(char* bname)
{
	char    buf[STRLEN];
	setvfile(buf, bname, "results");
	if (ansimore(buf, YEA) == -1)
	{
		move(3, 0);
		prints("目前没有任何投票的结果。\n");
		clrtobot();
		pressreturn();
	}
	else
		clear();
	return FULLUPDATE;
}

int b_vote_maintain()
{
	return vote_maintain(currboard);
}

void vote_title()
{

	docmdtitle("[投票箱列表]",
	           "[[1;32m←[m,[1;32me[m] 离开 [[1;32mh[m] 求助 [[1;32m→[m,[1;32mr <cr>[m] 进行投票 [[1;32m↑[m,[1;32m↓[m] 上,下选择 [1m高亮度[m表示尚未投票");
	update_endline();
}

#define maxrecp 300
int ListFileEdit(char *fname, char *msg)
{
	char listfile[STRLEN], uident[13], ans[3];
	int     cnt, i, n, fmode = NA;

	if(fname == NULL)
		return 0;
	strcpy(listfile, fname);
	clear();
	cnt = listfilecontent(listfile,3);
	while (1)
	{
		if(msg != NULL)
		{
			move(0,0);
			prints("\033[0;1;47;31m%s\033[m",msg);
		}
		if (cnt > maxrecp - 10)
		{
			move(2, 0);
			prints("目前限制本名单最多 ^[[1m%d^[[m 人", maxrecp);
		}
		getdata(1,0,"(A)增加 (D)删除 (I)引入好友 (C)清除目前名单 (E)确定 [E]: ",
		        ans, 2, DOECHO, YEA);
		if (  ans[0]!='a'&&ans[0]!='A'&&ans[0]!='d'&&ans[0]!='D'
		        &&ans[0]!='i'&&ans[0]!='I'&&ans[0]!='c'&&ans[0]!='C')
		{
			clear();
			return cnt;
		}
		if (ans[0]=='a'||ans[0]=='d'||ans[0]=='A'||ans[0]=='D')
		{
			move(2, 0);
			if (ans[0]=='a'||ans[0]=='A')
				usercomplete("请依次输入使用者代号(只按 ENTER 结束输入): ", uident);
			else
				namecomplete("请依次输入使用者代号(只按 ENTER 结束输入): ", uident);
			move(2, 0);
			clrtoeol();
			if (uident[0]=='\0')
				continue;
		}
		switch (ans[0])
		{
		case 'A':
		case 'a':
			if (!getuser(uident))
			{
				move(2, 0);
				prints("这个使用者代号是错误的.");
				break;
			}
			if(!(lookupuser.userlevel & PERM_READMAIL))
			{
				move(2, 0);
				prints("[%s] 目前被停权\n", uident);
				break;
			}
			if(SeekInNameList(uident))
			{
				move(2, 0);
				prints("[%s] 已经在名单中\n", uident);
				break;
			}
			add_to_file(listfile, uident);
			cnt++;
			break;
		case 'D':
		case 'd':
			if (seek_in_file(listfile, uident))
			{
				del_from_file(listfile, uident);
				DelFromNameList(uident);
				cnt--;
			}
			break;
		case 'I':
		case 'i':
			n = 0;
			clear();
			for (i = cnt; i < maxrecp && n < uinfo.fnum; i++)
			{
				int     key;
				move(2, 0);
				getuserid(uident, uinfo.friend[n]);
				prints("%s\n", uident);
				move(3, 0);
				n++;
				prints("(A)全部加入 (Y)加入 (N)不加入 (Q)结束? [Y]:");
				if (!fmode)
					key = igetkey();
				else
					key = 'Y';
				if (key == 'q' || key == 'Q')
					break;
				if (key == 'A' || key == 'a')
				{
					fmode = YEA;
					key = 'Y';
				}
				if (key == '\0' || key == '\n' || key == 'y' || key == 'Y')
				{
					if (!getuser(uident))
					{
						move(4, 0);
						prints("这个使用者代号是错误的.\n");
						i--;
						continue;
					}
					else if(!(lookupuser.userlevel & PERM_READMAIL))
					{
						move(4, 0);
						prints("[%s] 目前被停权\n", uident);
						i--;
						continue;
					}
					else if (SeekInNameList(uident))
					{
						move(4, 0);
						prints("[%s] 已经在名单中\n", uident);
						i--;
						continue;
					}
					add_to_file(listfile, uident);
					cnt++;
				}
			}
			fmode = NA;
			clear();
			break;
		case 'C':
		case 'c':
			unlink(listfile);
			cnt = 0;
		}
		move(5, 0);
		clrtobot();
		move(3, 0);
		clrtobot();
		cnt = listfilecontent(listfile,3);
	}
}

int vote_key(int ch, int allnum, int pagenum)
{
	int     deal = 0;
	char    buf[STRLEN];

	switch (ch)
	{
	case 'v':
	case 'V':
	case '\n':
	case '\r':
	case 'r':
	case KEY_RIGHT:
		user_vote(allnum + 1);
		deal = 1;
		break;
	case 'R':
		vote_results(currboard);
		deal = 1;
		break;
	case 'H':
	case 'h':
		show_help("help/votehelp");
		deal = 1;
		break;
	case 'A':
	case 'a':
		if (!chk_currBM(currBM))
			return YEA;
		vote_maintain(currboard);
		deal = 1;
		break;
	case 'O':
	case 'o':
		if (!chk_currBM(currBM))
			return YEA;
		clear();
		deal = 1;
		get_record(controlfile,&currvote,sizeof(struct votebal),allnum+1);
		prints("[5;1;31m警告!![m\n投票箱标题：[1m%s[m\n",
		       currvote.title);
		if(askyn("你确定要提早结束这个投票吗", NA, NA)!= YEA)
		{
			move(2, 0);
			prints("取消提早结束投票行动\n");
			pressreturn();
			clear();
			break;
		}
		mk_result(allnum + 1);
		sprintf(buf, "提早结束投票 %s", currvote.title);
		security_report(buf,2);
		break;
	case 'T':
		if(!chk_currBM(currBM))
			return YEA;
		deal = 1;
		get_record(controlfile,&currvote,sizeof(struct votebal),allnum+1);
		getdata(t_lines-1,0,"新投票主题：",currvote.title, 51, DOECHO, YEA);
		if(currvote.title[0] != '\0')
		{
			substitute_record(controlfile, &currvote,
			                  sizeof(struct votebal) , allnum + 1);
		}
		break;
	case 's':
		get_record(controlfile,&currvote,sizeof(struct votebal),allnum+1);
		if(currvote.level == 0 )
			break;
		deal = 1;
		move(t_lines-8,0);
		clrtoeol();
		prints("\033[47;30m『\033[31m%s\033[30m』投票受限说明：\033[m\n",currvote.title);
		if(currvote.level & ~(LISTMASK | VOTEMASK))
		{
			int num, len;
			strcpy(genbuf, "bTCPRD#@XWBA#VS-DOM-F0s2345678");
			len = strlen(genbuf);
			for (num = 0; num < len; num++)
				if (!(currvote.level & (1 << num)))
					genbuf[num] = '-';
			prints("权限受限：[\033[32m%s\033[m]\n",genbuf);
		}
		else
			prints("权限受限：[\033[32m本票箱投票不受权限限制\033[m]\n");
		if(currvote.level & LISTMASK)
			prints("名单受限：[\033[35m只有投票名单中的 ID 才可投票  \033[m]\n");
		else
			prints("名单受限：[\033[35m本票箱投票不受投票名单限制    \033[m]\n");
		if(currvote.level&VOTEMASK)
		{
			prints("          ┏ ①、在本站的上站次数至少 [%-4d] 次\n",currvote.x_logins);
			prints("条件受限：┣ ②、在本站发表文章至少   [%-4d] 篇\n",currvote.x_posts);
			prints("          ┣ ③、实际累计上站时间至少 [%-4d] 小时\n",currvote.x_stay);
			prints("          ┗ ④、该 ID 的注册时间至少 [%-4d] 天\n",currvote.x_live);
		}
		else
			prints("条件受限：[\033[36m本票箱投票不受个人条件限制\033[m    ]\n");
		pressanykey();
		break;
	case 'S':
		if(!HAS_PERM(PERM_OVOTE))
			return YEA;
		deal = 1;
		get_record(controlfile,&currvote,sizeof(struct votebal),allnum+1);
		if( setvoteperm(&currvote) != 0 )
		{
			substitute_record(controlfile, &currvote,
			                  sizeof(struct votebal) , allnum + 1);
		}
		break;
	case Ctrl('E'):
					if(!HAS_PERM(PERM_OVOTE))
						return YEA;
		deal = 1;
		setvfile(genbuf, currboard, "vote.list");
		ListFileEdit(genbuf,"编辑『投票名单』");
		break;
	case 'E':
		if(!chk_currBM(currBM))
			return YEA;
		deal = 1;
		get_record(controlfile,&currvote,sizeof(struct votebal),allnum+1);
		setvfile(genbuf, currboard, "desc");
		sprintf(buf, "%s.%d", genbuf, currvote.opendate);
		vedit(buf, NA, YEA);
		break;
	case 'M':
	case 'm':
		if (!chk_currBM(currBM))
			return YEA;
		clear();
		deal = 1;
		get_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
		prints("[5;1;31m警告!![m\n");
		prints("投票箱标题：[1m%s[m\n", currvote.title);
		if(askyn("你确定要修改这个投票的设定吗", NA, NA)!=YEA)
{
			move(2, 0);
			prints("取消修改投票行动\n");
			pressreturn();
			clear();
			break;
		}
		makevote(&currvote,currboard);
		substitute_record(controlfile,&currvote,
		                  sizeof(struct votebal),allnum+1);
		sprintf(buf, "修改投票设定 %s", currvote.title);
		security_report(buf,2);
		break;
	case 'D':
	case 'd':
		if (!HAS_PERM(PERM_OVOTE))
		{
			if (!chk_currBM(currBM))
				return YEA;
		}
		deal = 1;
		get_record(controlfile, &currvote, sizeof(struct votebal), allnum + 1);
		clear();
		prints("[5;1;31m警告!![m\n");
		prints("投票箱标题：[1m%s[m\n", currvote.title);
		if(askyn("您确定要强制关闭这个投票吗", NA, NA)!=YEA)
		{
			move(2, 0);
			prints("取消强制关闭行动\n");
			pressreturn();
			clear();
			break;
		}
		sprintf(buf, "强制关闭投票 %s", currvote.title);
		security_report(buf,2);
		dele_vote(allnum + 1);
		break;
	default:
		return 0;
	}
	if (deal)
	{
		Show_Votes();
		vote_title();
	}
	return 1;
}

int Show_Votes()
{
	move(3, 0);
	clrtobot();
	printvote(NULL);
	setcontrolfile();
	if (apply_record(controlfile, printvote, sizeof(struct votebal)) == -1)
	{
		prints("错误，没有投票箱开启....");
		pressreturn();
		return 0;
	}
	clrtobot();
	return 0;
}

int b_vote()
{
	int     num_of_vote;
	int     voting;
	char boardwrite[STRLEN];
	setbfile(boardwrite, currboard, "board.write");
	if(dashf(boardwrite)&&!seek_in_file(boardwrite,currentuser.userid))
		return;
	if (!HAS_PERM(PERM_VOTE) || (currentuser.stay < 1800))
		return;
	setcontrolfile();
	num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
	if (num_of_vote == 0)
	{
		move(2, 0);
		clrtobot();
		prints("\n抱歉, 目前并没有任何投票举行。\n");
		pressreturn();
		setvoteflag(currboard, 0);
		return FULLUPDATE;
	}
	setvoteflag(currboard, 1);
	modify_user_mode(VOTING);
	setlistrange(num_of_vote);
	clear();
	voting = choose(NA, 0, vote_title, vote_key, Show_Votes, user_vote);
	clear();
	return FULLUPDATE;
}

int b_results()
{
	return vote_results(currboard);
}
/*
int
m_vote()
{
	char    buf[STRLEN];
	strcpy(buf, currboard);
	strcpy(currboard, DEFAULTBOARD);
	modify_user_mode(ADMIN);
	vote_maintain(DEFAULTBOARD);
	strcpy(currboard, buf);
	return;
}

int
x_vote()
{
	char    buf[STRLEN];
	modify_user_mode(XMENU);
	strcpy(buf, currboard);
	strcpy(currboard, DEFAULTBOARD);
	b_vote();
	strcpy(currboard, buf);
	return;
}

int
x_results()
{
	modify_user_mode(XMENU);
	return vote_results(DEFAULTBOARD);
}

*/

int m_vote()
{
	char buf[STRLEN];
	strcpy(buf, currboard);
	//strcpy(currboard, "NanKaiBBS");
	strcpy(currboard, "FameHall");
	modify_user_mode(ADMIN);
	//vote_maintain("NanKaiBBS");
	vote_maintain("FameHall");
	strcpy(currboard, buf);
	return;
}

int x_vote()
{
	char buf[STRLEN];
	modify_user_mode(XMENU);
	strcpy(buf, currboard);
	strcpy(currboard, "FameHall");
	b_vote();
	strcpy(currboard, buf);
	return;
}

int x_results()
{
	modify_user_mode(XMENU);
	return vote_results("FameHall");
	//return vote_results("NanKaiBBS");
}

int check_x_vote()
{
	char   buf[STRLEN];
	char   fname[STRLEN];
	int    num_of_vote;
	int checkvoted=1;
	struct votebal tmpbal;
	struct ballot uservote;
	struct stat st1,st2;
	strcpy(buf, currboard);
	//strcpy(currboard, "NanKaiBBS");
	strcpy(currboard, "sysop");
	setcontrolfile();
	char voted[STRLEN];
	sprintf(voted,"home/%c/%s/voted",toupper(currentuser.userid[0]),currentuser.userid);
	if(dashf(voted)&&dashf(controlfile))
	{
		stat(voted,&st1);
		stat(controlfile,&st2);
		if(st1.st_mtime>st2.st_mtime)
		{
			strcpy(currboard,buf);
			return 0;
		}
		else
		{
			unlink(voted);
		}
	}
	num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
	//if(num_of_vote) 
		//if(askyn("本站正在全站投票中,是否马上参加?",NA,NA)==YEA) b_vote();
	if(num_of_vote)
	{
		for(;num_of_vote>0;num_of_vote--)
		{
			get_record(controlfile, &tmpbal, sizeof(struct votebal),num_of_vote);
			sprintf(fname, "vote/%s/flag.%d", currboard, tmpbal.opendate);
			if (search_record(fname, &uservote, sizeof(uservote), cmpvuid,
	                  currentuser.userid) <= 0) checkvoted=0;
		}
		if(checkvoted==0)
			if(askyn("本站正在举行全站投票中,您还未投票,是否马上参加?",NA,NA)==YEA) b_vote();
	}
	strcpy(currboard, buf);
	char cmd[STRLEN+20];
	sprintf(cmd,"/bin/touch %s",voted);
	system(cmd);
	return num_of_vote;
}//loveni:检查全站投票

