//boarddetail.wml.c
//huangxu@nkbbs.org

//��ʾ������ϸ��Ϣ��

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
			case 1://�������δ��
				if (loginok)
				{
					brc_init(currentuser.userid, board);
					brc_clear();
					brc_update(currentuser.userid, board);
					strcpy (opinfo, "�ѱ�Ǳ���Ϊ�Ѷ���");
				}
				break;
		}
		printf ("<card title=\"%s/%s\">", x->title + 11, x->filename);
		if (*opinfo)
		{
			printf ("<p>%s</p>", opinfo);
		}
		printf ("<p>���棺<anchor><go href=\"bbsdoc.wml?board=%s\" />%s/%s</anchor></p>", x->filename, x->title + 11, x->filename);
		wmlUseridStr(buf, x->BM);
		printf ("<p>������%s %s %s</p>", *buf ? buf : "��", (x->flag & OUT_FLAG) ? "ת��" : "", junkboard(board) ? "����������" : "��������");
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=0\" />��ͨ</anchor> ", board);
		printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=1\" />��ժ</anchor> ", board);
		printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=8\" />�ö�</anchor> ", board);
		if (has_BM_perm(&currentuser, x->filename) || HAS_PERM(PERM_FORCEPAGE))
		{
			printf ("<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=4\" />����</anchor>", board);
		}
		printf ("</p>");
		if (HAS_PERM(PERM_POST))
		{
			if (x->flag & VOTE_FLAG)
			{
				printf ("<p><anchor><go href=\"bbsvote.wml?board=%s\" />ͶƱ</anchor></p>", x->filename);
			}
			printf ("<p><anchor><go href=\"bbspst.wml?board=%s\" />����</anchor> <anchor><go href=\"bbspattern.wml?board=%s\" />ģ��</anchor> <anchor><go href=\"bbs0an.wml?path=%s\" />����</anchor> <anchor><go href=\"boarddetail.wml?board=%s&amp;act=1\" />���δ��</anchor> </p>", x->filename, x->filename, anno_path_of(x->filename), board);
		}
		printf ("<p><anchor><go href=\"bbscon.wml?board=%s&amp;file=%s\" />�������</anchor> <anchor><go href=\"bbscon.wml?board=%s\" />���д�</anchor></p>", WML_VB_DENIED, board, WML_VB_HEXIE);
		return 0;
	}
	else
	{
		strcpy (info, "�����������ڻ�����Ȩ�������������");
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
		printf ("<card title=\"���� -- %s\">", BBSNAME);
		printf ("<p>%s</p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
