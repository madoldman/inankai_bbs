//bbsvote.wml.c
//同样，此文件完全照bbsvote.c修改而来
//huangxu@nkbbs.org

//投票

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "../../bbssrc/include/vote.h"
//是不是该整理一下头了
static char *vote_type[] = { "是非", "单选", "复选", "数字", "问答" };
static char *vote_yn[] = {"赞成", "反对", "弃权"};
void setvfile(char *buf, char *bname, char *filename)
{
	sprintf(buf, "vote/%s/%s", bname, filename);
}

int votesug_inp(struct ballot * v)
{
	int i;
	printf ("<p>请输入您的建议：");
	for (i = 0; i < 3; i++)
	{
		printf ("<br /><input name=\"inp_sug%d\" maxlength=\"78\" value=\"", i);
		w_hprintf ("%s", v->msg[i]);
		printf ("\" />");
	}
	printf ("</p>");
	return 0;
}

int votesug_pst()
{
	int i;
	char buf[256];
	for (i = 0; i < 3; i++)
	{
		sprintf (buf, "sug%d", i);
		wmlPostfield(buf);
	}
	return 0;
}

int bbsvote_act(char * info)
{
	FILE *fp;
	struct votebal currvote, ent;
	char buf[STRLEN], buf1[512];
	char buf2[1024];
	struct ballot uservote;
	//	struct votelog log;
	int aborted = NA, pos;
	int i, chkd;
	unsigned int j, multiroll = 0;
	char board[80];
	char posturl[256];
	char controlfile[STRLEN];
	char *date, *tmp1, *tmp2;
	char flagname[STRLEN];
	char logname[STRLEN];
	int voted_flag;		//用户是否投过该项票
	int num_voted;		//这个是有多少人投过票
	int num_of_vote;	//这个是开启的投票数
	int votenum;		//这个是用户选择进行第几个投票
	int votevalue = 0;
	int procvote;
	time_t closedate;
	struct stat st;
	int now_t=time(0);
	if (!loginok) 
	{
		strcpy(info, "匆匆过客不能投票。");
		return -1;
	}
	strsncpy(board, getparm("B"), 32);
	if (!*board)
	{
		strsncpy(board, getparm("board"), 32);
	}
	votenum = atoi(getparm("votenum"));
	procvote = atoi(getparm("procvote"));
	modify_mode(u_info, VOTING);
	if(!has_post_perm(&currentuser, board) || !HAS_PERM(PERM_VOTE))//去死吧倒霉的全站投票
	{
		strcpy(info, "您无权在本板内投票。");
		return -1;
	}
	sprintf(controlfile, "vote/%s/%s", board, "control");
	num_of_vote = (stat(controlfile, &st) == -1) ? 0 : st.st_size / sizeof (struct votebal);
	if (!num_of_vote)
	{
		strcpy(info, "抱歉, 目前并没有任何投票举行。");
		return -2;
	}
	fp = fopen(controlfile, "r");
	if (!fp)//大概不会发生
	{
		strcpy(info, "系统错误。");
		return -256;
	}
	//从if中提出来的
	if (!votenum) //这是投票列表
	{
		printf ("<card title=\"投票箱 -- %s\">", BBSNAME);
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />%s板</anchor>投票</p>", board, board);
		for (i = 1; i <= num_of_vote; i++) 
		{
			fread(&ent, sizeof (struct votebal), 1, fp);
			sprintf(flagname, "vote/%s/flag.%d", board, (int) ent.opendate);
			num_voted = (stat(flagname, &st) ==	-1) ? 0 : st.st_size / sizeof (struct ballot);
			closedate = ent.opendate + ent.maxdays * 86400;
			//注意，这里的date成了结束时间
			w_hsprintf(buf2, "%s", ent.title);//输出标题
			printf("<p><anchor><go href=\"bbsvote.wml?board=%s&amp;votenum=%d\" />#%d %s<br />", board, i, i, buf2);
			//发起投票者被华丽地无视了！
			datestr(buf2, closedate);
			//类别也被华丽地无视了！
			printf ("结束：%s 参与：%d</anchor></p>", buf2, num_voted);
		}
		fclose(fp);
		return 0;
	}
	else 
	{
		if (votenum > num_of_vote)
		{
			strcpy(info, "参数错误。");
			return -3;
		}
		fseek(fp, sizeof (struct votebal) * (votenum - 1), 0);
		fread(&currvote, sizeof (struct votebal), 1, fp);
		fclose(fp);
		sprintf(flagname, "vote/%s/flag.%d", board, (int) currvote.opendate);
		num_voted = (stat(flagname, &st) == -1) ? 0 : st.st_size / sizeof (struct ballot);
		pos = 0;
		fp = fopen(flagname, "r");
		voted_flag = NA;
		if (fp) 
		{
			for (i = 1; i <= num_voted; i++) 
			{
				fread(&uservote, sizeof (struct ballot), 1, fp);
				if (!strcasecmp(uservote.uid, currentuser.userid)) 
				{
					voted_flag = YEA;
					pos = i;
					break;
				}
			}
			fclose(fp);
		}
		if (!voted_flag)
		{
			memset(&uservote, 0, sizeof (uservote));
		}
		if (procvote == 0) 
			//-------投票权限判断
			//没仔细看-_-b
		{
			if (currentuser.firstlogin > currvote.opendate)	
			{
				strcpy(info, "对不起, 本投票在您帐号申请之前开启，您不能投票。");
				return -3;
			}
			else if (!HAS_PERM(currvote.level & ~(LISTMASK | VOTEMASK)))
			{
				strcpy(info, "对不起，您目前尚无权在本票箱投票。");
				return -4;
			}
			else if(currvote.level & LISTMASK )
			{
				char listfilename[STRLEN];
				setvfile(listfilename, board, "vote.list");
				if(!dashf(listfilename)) 
				{
					strcpy(info, "对不起，本票箱需要设定好投票名册方可进行投票。");
					return -5;
				}
				else if(!seek_in_file(listfilename,currentuser.userid))
				{
					strcpy(info, "对不起, 投票名册上找不到您的大名。");
					return -6;
				}
			}
			else if(currvote.level & VOTEMASK )
			{
				if(currentuser.numlogins < currvote.x_logins || currentuser.numposts < currvote.x_posts ||currentuser.stay < currvote.x_stay * 3600
					|| currentuser.firstlogin > currvote.opendate - currvote.x_live * 86400)
				{
					strcpy(info, "对不起，您目前尚不够资格在本票箱投票。");
					return -7;
				}
			}
			closedate =	currvote.opendate + currvote.maxdays * 86400;
			printf ("<card title=\"投票箱 -- %s\">", BBSNAME);
			w_hsprintf (buf2, "%s", currvote.title);
			printf("<p>投票主题： %s<br />", buf2);
			printf("投票类型： %s<br />", vote_type[currvote.type - 1]);
			printf("发起投票： <anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", currvote.userid, currvote.userid);
			printf("所在板面： <anchor><go href=\"bbsdoc.wml?board=%s\" />%s</anchor><br />", board, board);
			datetime(buf2, closedate);
			printf("结束时间： %s<br />", buf2);
			if (currvote.type != VOTE_ASKING)
			{
				printf("最多投票： %d<br />", currvote.maxtkt);
				printf("投票说明：<br />");
				sprintf(buf, "vote/%s/desc.%d", board, (int) currvote.opendate);
				fp = fopen(buf, "r");
				if (fp)
				{
					while (1) 
					{
						if (fgets(buf1, sizeof (buf1), fp) == 0)
						{
							break;
						}
						w_hprintf("%s", buf1);
						printf ("<br />");
					}
					fclose(fp);
				}
			}
			printf ("</p>");
			//multiroll这种东西没用啦
			sprintf (posturl, "bbsvote.wml?board=%s&amp;votenum=%d", board, votenum);
			switch (currvote.type) 
			{
				case VOTE_SINGLE:
					//单选
					j =	uservote.voted + (uservote.voted << currvote.totalitems);
					chkd = 0;
					//这是当前选中的
					printf ("<p>选项列表：<br />");
					for (i = 0; i < currvote.totalitems; i++) 
					{
						w_hprintf ("选项%d %s", i + 1, currvote.items[i]);
						printf ("<br />");
						if (j & 1)
						{
							chkd = i + 1;
						}
						j >>= 1;
					}
					printf ("请选择：<select name=\"inp_vote\" value=\"%d\">", chkd);
					printf ("<option value=\"0\">未选择</option>");
					for (i = 0; i < currvote.totalitems; i++)
					{
						printf ("<option value=\"%d\">%d</option>", i + 1, i + 1);
					}
					printf ("</select></p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"2\" />", posturl);
					wmlPostfield("vote");
					votesug_pst();
					printf ("</go>投票</anchor></p>");
					break;
				case VOTE_MULTI:
					//复选
					j = uservote.voted + (uservote.voted << currvote.totalitems);
					printf ("<p>选项列表<em>（最多投%d票）</em>：<br />", currvote.maxtkt);
					for (i = 0; i < currvote.totalitems; i++) 
					{
						printf ("<select name=\"inp_votemulti%d\" value=\"%d\"><option value=\"0\">不选择</option><option value=\"1\">已选择</option></select> ", i + 1, (j & 1));
						w_hprintf ("%s", currvote.items[i]);
						printf ("<br />");
						j >>= 1;
					}
					printf ("</p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"3\" />", posturl);
					for (i = 0; i < currvote.totalitems; i++)
					{
						sprintf (buf2, "votemulti%d", i + 1);
						wmlPostfield(buf2);
					}
					votesug_pst();
					printf ("</go>投票</anchor></p>");
					break;
				case VOTE_YN:
					//是非 ... 不是跟单选一样的么？
					j =	uservote.voted + (uservote.voted << currvote.totalitems);
					chkd = 0;
					//这是当前选中的
					printf ("<p>选项列表：<br />");
					for (i = 0; i < currvote.totalitems; i++) 
					{
						if (j & 1)
						{
							chkd = i + 1;
							break;
						}
						j >>= 1;
					}
					printf ("请选择：<select name=\"inp_vote\" value=\"%d\">", chkd);
					printf ("<option value=\"0\">未选择</option>");
					for (i = 0; i < currvote.totalitems; i++)
					{
						printf ("<option value=\"%d\">", i + 1);
						w_hprintf ("%s", currvote.items[i]);
						printf ("</option>");
					}
					printf ("</select></p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"1\" />", posturl);
					//这里不同，单选procvote=2
					wmlPostfield("vote");
					votesug_pst();
					printf ("</go>投票</anchor></p>");
					break;
				case VOTE_VALUE:
					//数值
					printf ("<p>请输入一个值：");
					printf ("<input name=\"inp_votevalue\" value=\"%d\" /></p>", uservote.voted);
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"4\" />", posturl);
					wmlPostfield("votevalue");
					votesug_pst();
					printf ("</go>投票</anchor></p>");
					break;
				case VOTE_ASKING:
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"5\" />", posturl);
					votesug_pst();
					printf ("</go>投票</anchor></p>");
					break;
				default:
					printf ("<p>没有这种类型的投票啊。</p>");
					return 0;
					//:(
				//<-switch
			}
		}
		else
		{
			if (procvote != currvote.type)
			{
				strcpy(info, "投票错误。");
				return -1;
			}
			switch (procvote) 
			{
				case 2:	//VOTE_SINGLE
					votevalue = 1;
					votevalue <<= atoi(getparm("vote")) - 1;
					if (atoi(getparm("vote")) > currvote.totalitems + 1)
					{
						strcpy(info, "无效选项。");
						return -1;
					}
					aborted = (votevalue == uservote.voted);
					break;
				case 3:	//VOTE_MULTI
					votevalue = 0;
					j = 0;
					for (i = currvote.totalitems - 1; i >= 0; i--) 
					{
						votevalue <<= 1;
						sprintf(buf, "votemulti%d", i + 1);
						votevalue |= !!atoi(getparm(buf));
						j += !!atoi(getparm(buf));
					}
					aborted = (votevalue == uservote.voted);
					if (j > currvote.maxtkt) 
					{
						sprintf(info, "本投票最多只能选择%d个选项。", currvote.maxtkt);
						return -1;
					}
					break;
				case 1:	//VOTE_YN
					votevalue = 1;
					j = atoi(getparm("vote")) - 1;
					if (j >	currvote.totalitems || j < 0)
					{
						strcpy(info, "无效选项。");
						return -1;
					}
					votevalue <<= j;
					aborted = (votevalue == uservote.voted);
					break;
				case 4:	//VOTE_VALUE
					aborted = ((votevalue = atoi(getparm("votevalue"))) == uservote.voted);
					if (votevalue < 0 || votevalue > currvote.maxtkt) 
					{
						sprintf(info, "应当介于0到%d之间（含）。", currvote.maxtkt);
						return -1;
					}
					break;
				case 5: //VOTE_ASKING
					aborted = NA;
					break;
				//<-switch
			}
			printf ("<card title=\"投票箱 -- %s\">", BBSNAME);
			if (aborted == YEA) 
			{
				printf ("<p>未修改该投票的选项。</p>");
				return 0;
			}
			else
			{
				fp = fopen(flagname, "r+");
				if (fp == 0)
				{
					fp = fopen(flagname, "w+");
				}
				if (fp)
				{
					flock(fileno(fp), LOCK_EX);
					if (pos > 0)
					{
						fseek(fp, (pos - 1) * sizeof (struct ballot), SEEK_SET);
					}
					else
					{
						fseek(fp, 0, SEEK_END);
					}
					strcpy(uservote.uid, currentuser.userid);
					uservote.voted = votevalue;
					for (i = 0; i < 3; i++)
					{
						sprintf (buf2, "sug%d", i);
						strncpy (uservote.msg[i], getparm(buf2), 78);
						removeInvisibleChars(uservote.msg[i], ' ');
					}
					fwrite(&uservote, sizeof (struct ballot), 1, fp);
					flock(fileno(fp), LOCK_UN);
					fclose(fp);
					printf("<p>已经帮您投入票箱中。</p>");
					sprintf(buf, "%s %s %s", currentuser.userid, currentuser.lasthost, Ctime(now_t));
					fs_append(BBSHOME"/vote.log", buf);
				}
				else
				{
					printf ("<p>系统发生了一点小小的问题-_-b</p>");
				}
			}
		}
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsvote_act(buf))
	{
		printf ("<card title=\"错误 -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}


