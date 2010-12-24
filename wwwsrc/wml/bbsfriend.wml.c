//bbsfriend.wml.c
//huangxu@nkbbs.org

#include "BBSLIB.inc"
#include "WMLLIB.inc"

//bbsfriend.wml?userid=(1)&act=(2)&start=(3)
//查看、编辑好友。说明就算了。

int bbsfriend_act(char * info)
{
	int act, start;
	char userid[IDLEN + 1];
	char buf[256];
	int i, j, tmp;
	struct userec * x;
	FILE * fp;
	if (!loginok)
	{
		sprintf (info, "匆匆过客不能使用此功能。");
		return -1;
	}
	wmlmode (u_info, FRIEND);
	strncpy (userid, getparm("userid"), IDLEN);
	act = atoi(getparm("act"));
	start = atoi(getparm("start")) - 1;
	loadfriend(currentuser.userid);
	if (act)
	{
		tmp = -1;
		for (i = 0; i < friendnum; i++)
		{
			if (!strcasecmp(userid, fff[i].id))
			{
				tmp = i;
				break;
			}
		}
		if (act == 1)//增加好友
		{
			if (tmp >= 0)
			{
				sprintf (info, "%s已经在你的ID之中了。", userid);
				return -2;
			}
			if (friendnum + 1 > MAXFRIENDS)
			{
				printf (info, "您的好友数已经达到上限%d，不能继续添加好友。", MAXFRIENDS);
				return -3;
			}
			x = getuser(userid);
			if (!x)
			{
				sprintf (info, "该用户不存在。");
				return -4;
			}
			memset (fff + friendnum, 0, sizeof(struct override));
			strcpy (fff[friendnum].id, x->userid);
			tmp = friendnum;
			friendnum++;
		}
		else if (act == 2)//删除好友
		{
			if (tmp < 0)
			{
				sprintf (info, "您未添加过此好友。");
				return -2;
			}
			memmove (fff + i, fff + i + 1, friendnum - i - 1);
			friendnum--;
			memset (fff + friendnum, 0, sizeof(struct override));
		}
		sprintf(buf, "home/%c/%s/friends", toupper(currentuser.userid[0]), currentuser.userid);
		fp = fopen(buf, "w");
		fwrite(fff, sizeof(struct override), friendnum, fp);
		fclose(fp);
		tmp = tmp / WMLS_FRIEND_PER_PAGE * WMLS_FRIEND_PER_PAGE;
		printf ("<card title=\"好友操作成功 -- %s\" ontimer=\"bbsfriend.wml?start=%d\">", BBSNAME, tmp + 1);
		printf ("<timer value=\"50\" />");
		printf ("<p>已成功%s好友<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor>。</p>", act == 1 ? "添加" : "删除", userid, userid);
		printf ("<p><anchor><go href=\"bbsfriend.wml?start=%d\" />如果不能自动跳转，请使用此链接。</anchor></p>", tmp + 1);
		return 0;
	}
	if (start < 0 || start >= friendnum)
	{
		start = 0;
	}
	printf ("<card title=\"好友名单 -- %s\">", BBSNAME);
	tmp = start + WMLS_FRIEND_PER_PAGE;
	if (tmp > friendnum)
	{
		tmp = friendnum;
	}
	for (i = start; i < tmp; i++)
	{
		printf ("<p>#%d <a href=\"bbsqry.wml?userid=%s\">%s</a> ", i + 1, fff[i].id, fff[i].id);
		w_hprintf ("%s", fff[i].exp);
		printf ("</p>");
	}
	printf ("<p>ID：<input name=\"inp_userid\" /> <anchor><go href=\"bbsqry.wml\" method=\"get\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>查询用户</anchor> <anchor><go href=\"bbsfriend.wml?act=1\" method=\"post\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>加为好友</anchor> <anchor><go href=\"bbsfriend.wml?act=2\" method=\"post\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>删除好友</anchor></p>");
	if (friendnum > WMLS_FRIEND_PER_PAGE)
	{
		printf ("<p>");
		for (i = 0; i <= friendnum / WMLS_FRIEND_PER_PAGE; i++)
		{
			if (i == 0 || i == friendnum / WMLS_FRIEND_PER_PAGE || abs(i - (start / WMLS_FRIEND_PER_PAGE)) < 3)
			{
				if (i == start / WMLS_FRIEND_PER_PAGE)
				{
					printf ("%d ", i + 1);
				}
				else
				{
					printf ("<a href=\"bbsfriend.wml?start=%d\">%d</a> ", (i * WMLS_FRIEND_PER_PAGE) + 1, i + 1);
				}
			}
		}
		printf ("</p>");
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsfriend_act(buf))
	{
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
