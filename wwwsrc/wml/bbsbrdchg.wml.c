//bbsbrdchg.wml
//huangxu@nkbbs.org

//��ɾ�ղؼа��棬ֻ�м򵥵Ĳ���

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
		strcpy (info, "�Ҵҹ����޷�ʹ���ղؼ�");
		return -1;
	}
	if (!has_read_perm(&currentuser, board))
	{
		strcpy (info, "����ȷ�İ��档");
		return -2;
	}
	add = 1;
	num = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
	for (i = 0; i < num; i++)
	{
		if (!strcasecmp(board, mybrd[i]))
		{
			memmove (mybrd[i], mybrd[i + 1], 80 * (num - i - 1));
			printf ("<card title=\"ȡ������ -- %s\">", BBSNAME);
			printf ("<p>���桰%s��ȡ���ɹ���</p>", board);
			num --;
			add = 0;
			break;
		}
	}
	if (add)
	{
		if (num + 1 > GOOD_BRC_NUM)
		{
			sprintf (info, "�ղؼ�����Ϊ%d�����ܼ����ղذ��档", GOOD_BRC_NUM);
			return -3;
		}
		strcpy (mybrd[num], board);
		printf ("<card title=\"���İ��� -- %s\">", BBSNAME);
		printf ("<p>���桰%s�����ĳɹ���</p>", board);
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
		printf ("<card title=\"���� -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />������һҳ</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
