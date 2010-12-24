//bbsusr.wml.c
//huangxu@nkbbs.org

//注意：翻页还有问题。尽量用那个框找人吧。

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "user.h"

//bbsusr.wml?start=(1)&filter=(2)&mode=(3)

int idmatch(const char * id, const char * cond)
{
	if (!strncasecmp(cond, id, strlen(cond)))
		//for further .... -_- why calculate strlen every time?
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int showuser(struct user_info * u, int onlyfriend, const char * filter, int num)
{
	int fristat;
	const char FRIENDTEXT[4][5] = {"-", "&lt;", "&gt;", "o"};
	if (u->active == 0 || u->pid == 0 || u->invisible && !HAS_PERM(PERM_SEECLOAK) && strcmp(u->userid,currentuser.userid) && !canseeme(u->userid))
	{
		return 0;
	}
	if (!idmatch(u->userid, filter))
	{
		return 0;
	}
	fristat = (!!isfriend(u->userid) << 1) | !!hisfriend(u);
	if (onlyfriend && !fristat)
	{
		return 0;
	}
	printf ("<p>%s <anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor> %s %s%s</p>", FRIENDTEXT[fristat], u->userid, u->userid, u->from, u->mode >= 20000 ? "@" : "", ModeType(u->mode >= 20000 ? u->mode - 20000 : u->mode));
	return 1;
}

int bbsusr_act(char * info)
{
	int start, mode;
	char filter[IDLEN + 2];;
	struct userec * x;
	struct user_info * u;
	int i, j, step, end;
	char buf[256];
	char * p;
	start = (atoi(getparm("start")) + MAXACTIVE) % MAXACTIVE;//这里不需要-1了
	mode = atoi(getparm("mode"));
	//方向&&好友状态
	//bit0: 0 前进 1 后退
	//bit1: 1 好友 0 全部
	strncpy (filter, getparm("filter"), IDLEN + 1);
	for (p = filter; *p; p++)
	{
		if (!isalnum(*p))
		{
			*p = 0;
			break;
		}
	}
	if (loginok)
	{
		wmlmode (u_info, LUSERS);
	}
	printf ("<card title=\"在线用户 -- %s\">", BBSNAME);
	step = (mode & 1) ? -1 : 1;
	j = 0;
	end = (start - step + MAXACTIVE) % MAXACTIVE;
	for (i = start ; j < WMLS_FRIEND_PER_PAGE && i != end; i = (i + step) % MAXACTIVE)
	{
		j += showuser(&(shm_utmp->uinfo[i]), mode & 2, filter, i);
		if (i == MAXACTIVE - 1)
		{
			printf ("<p>已达到用户列表结尾处。</p>");
			//break;
		}
	}
	if (j >= WMLS_FRIEND_PER_PAGE)
	{
		printf ("<p><anchor><go href=\"bbsusr.wml?start=%d&amp;mode=%d&amp;filter=%s\" />上一页</anchor> <anchor><go href=\"bbsusr.wml?start=%d&amp;mode=%d&amp;filter=%s\" />下一页</anchor></p>", start - 1, mode | 1, filter, i + 1, mode & ~1, filter);
	}
	printf ("<p><anchor><go href=\"bbsusr.wml?mode=%d\" />%s</anchor> ", mode ^ 2, (mode & 2) ? "显示全部" : "显示好友");
	if (loginok)
	{
		printf ("<anchor><go href=\"bbsfriend.wml\" />好友名册</anchor> ");
	}
	printf ("<input name=\"inp_filter\" maxlength=\"%d\" /> <anchor><go href=\"bbsusr.wml?mode=%d\" method=\"post\"><postfield name=\"filter\" value=\"$(inp_filter)\" /></go>开头过滤</anchor> <anchor><go href=\"bbsqry.wml\" method=\"post\"><postfield name=\"userid\" value=\"$(inp_filter)\" /></go>查询用户</anchor>", IDLEN, mode & ~1);
	printf ("</p>");
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsusr_act(buf))
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

