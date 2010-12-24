//boarddetail.wml.c
//huangxu@nkbbs.org

//显示版面详细信息等

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

char dir[256];
char modeinfo[256];
int boardmode;
char board[80];
struct shortfile *x;

int boarddetail_act(char * info)
{
	int act;
	char buf[1024];
	char opinfo[256];
	*opinfo = 0;
	strncpy (board, getparm("board"), 79);
	act = atoi(getparm("act"));
	if (loginok)
	{
		wmlmode (u_info, READNEW);
	}
	x = getbcache(board);
	if (x)
	{
		if ((!canenterboard (currentuser.userid, board) || !has_read_perm(&currentuser, board)) && !HAS_PERM(PERM_SYSOP))
		{
			x = NULL;
		}
	}
	if (x)
	{
		switch (act)
		{
			case 1://清除板面未读
				if (loginok)
				{
					brc_init(currentuser.userid, board);
					brc_clear();
					brc_update(currentuser.userid, board);
					strcpy (opinfo, "已标记本板为已读。");
				}
				break;
		}
		printf ("<card title=\"%s/%s\">", x->title + 11, x->filename);
		if (*opinfo)
		{
			printf ("<p>%s</p>", opinfo);
		}
		printf ("<p>板面：<anchor><go href=\"bbsdoc.wml?board=%s\" />%s/%s</anchor></p>", x->filename, x->title + 11, x->filename);
		wmlUseridStr(buf, x->BM);
		printf ("<p>板主：%s %s %s</p>", *buf ? buf : "无", (x->flag & OUT_FLAG) ? "转信" : "", junkboard(board) ? "不计文章数" : "计文章数");
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=0\" />普通</anchor> ", board);
		printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=1\" />文摘</anchor> ", board);
		printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=8\" />置顶</anchor> ", board);
		if (has_BM_perm(&currentuser, x->filename) || HAS_PERM(PERM_FORCEPAGE))
		{
			printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=4\" />回收</anchor>", board);
		}
		printf ("</p>");
		if (HAS_PERM(PERM_POST))
		{
			if (x->flag & VOTE_FLAG)
			{
				printf ("<p><anchor><go href=\"bbsvote.wml?board=%s\" />投票</anchor></p>", x->filename);
			}
			printf ("<p><anchor><go href=\"bbspst.wml?board=%s\" />发文</anchor> <anchor><go href=\"bbspattern.wml?board=%s\" />模板</anchor> <anchor><go href=\"bbs0an.wml?path=%s\" />精华</anchor> <anchor><go href=\"boarddetail.wml?board=%s&amp;act=1\" />清除未读</anchor> </p>", x->filename, x->filename, anno_path_of(x->filename), board);
		}
		printf ("<p><anchor><go href=\"bbscon.wml?board=%s&amp;file=%s\" />封禁名单</anchor> <anchor><go href=\"bbscon.wml?board=%s\" />敏感词</anchor></p>", WML_VB_DENIED, board, WML_VB_HEXIE);
		return 0;
	}
	else
	{
		strcpy (info, "讨论区不存在或您无权进入此讨论区。");
		return -1;
	}
}

int main()
{
	char buf[1024];
	wml_init();
	wml_head();
	if (boarddetail_act(buf))
	{
		printf ("<card title=\"错误 -- %s\">", BBSNAME);
		printf ("<p>%s</p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
