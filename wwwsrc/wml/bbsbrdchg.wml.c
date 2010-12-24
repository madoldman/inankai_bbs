//bbsbrdchg.wml
//huangxu@nkbbs.org

//增删收藏夹板面，只有简单的操作

#include "BBSLIB.inc"
#include "WMLLIB.inc"

char board[80];
char mybrd[GOOD_BRC_NUM][80];
char mybrdnew[GOOD_BRC_NUM][80];

int bbsbrdchg_act(char * info)
{
	int i, num;
	int add;
	char board[80];
	strcpy (board, getparm("board"));
	if (!loginok)
	{
		strcpy (info, "匆匆过客无法使用收藏夹");
		return -1;
	}
	if (!has_read_perm(&currentuser, board))
	{
		strcpy (info, "不正确的板面。");
		return -2;
	}
	add = 1;
	num = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
	for (i = 0; i < num; i++)
	{
		if (!strcasecmp(board, mybrd[i]))
		{
			memmove (mybrd[i], mybrd[i + 1], 80 * (num - i - 1));
			printf ("<card title=\"取消订阅 -- %s\">", BBSNAME);
			printf ("<p>板面“%s”取消成功。</p>", board);
			num --;
			add = 0;
			break;
		}
	}
	if (add)
	{
		if (num + 1 > GOOD_BRC_NUM)
		{
			sprintf (info, "收藏夹上限为%d。不能继续收藏板面。", GOOD_BRC_NUM);
			return -3;
		}
		strcpy (mybrd[num], board);
		printf ("<card title=\"订阅板面 -- %s\">", BBSNAME);
		printf ("<p>板面“%s”订阅成功。</p>", board);
		num++;
	}
	SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, num);
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsbrdchg_act(buf))
	{
		printf ("<card title=\"错误 -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回上一页</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
