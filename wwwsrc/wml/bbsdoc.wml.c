//bbsdoc.wml.c
//huangxu@nkbbs.org

//��ʾ�������º��ż�

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"
#include "showboard.h"

//boardmode:
//0 = normal
//1 = G
//2 = M??
//4 = JUNK
//8 = TOP
//16 = CMODE??
//256 = TOPIC**

char dir[256];
char modeinfo[256];
int boardmode;
char board[80];
struct shortfile *xb;
struct fileheader x[40];
int start;

int main()
{
	int total, i, ps, tmp;
	char buf[256];
	int pagerange [] = {-5, -2, -1, 1, 2, 5, 0};
	FILE * fp;
	wml_init();
	wml_head();
	strncpy(board, getparm("board"), 79);
	boardmode = atoi(getparm("mode"));
	xb = getbcache(board);
	if (xb)
	{
		if ((!canenterboard (currentuser.userid, board) || !has_read_perm(&currentuser, board)) && !HAS_PERM(PERM_SYSOP))
		{
			xb = NULL;
		}
	}
	start = atoi(getparm("start"));
	if(loginok)
	{
		if (!strcmp(board, WML_VB_MAIL))
		{
			wmlmode(u_info, RMAIL);
		}
		else
		{
			wmlmode(u_info, READING);
		}
	}
	if (xb || (!strcmp(board, WML_VB_MAIL) && loginok))
	{
		boardModeDir (board, boardmode, dir, modeinfo);
		printf ("<card title=\"%s(%s) -- %s\">", xb ? (xb->title + 10) : "����", modeinfo, BBSNAME);
		if (!xb)
		{
			//��ʾ�������������˰ɡ�
		}
		fp = fopen(dir, "rb");
		if (fp)
		{
			total = file_size(dir) / sizeof(struct fileheader);
			if (start==0 || start > total - WMLS_POST_PER_PAGE)
			{
				start = total - WMLS_POST_PER_PAGE;
			}
			else
			{
				start--;
			}
			if (start < 0)
			{
				start = 0;
			}
			fseek(fp, sizeof(struct fileheader) * start, 0);
			ps = fread(x, sizeof(struct fileheader), WMLS_POST_PER_PAGE, fp);
			fclose(fp);
			brc_init(currentuser.userid, board);
			strcpy(u_info->board, board);
			if (xb)
			{
				printf ("<p>%s/%s</p><p><anchor><go href=\"bbspst.wml?board=%s\" />����</anchor> <anchor><go href=\"bbsbrdchg.wml?board=%s\" />�ղ�</anchor> ", xb->title + 10, xb->filename, xb->filename, xb->filename, xb->filename);
				if (xb->flag & VOTE_FLAG)
				{
					printf ("<anchor><go href=\"bbsvote.wml?board=%s\" />ͶƱ</anchor> ", xb->filename);
				}
				printf ("<anchor><go href=\"boarddetail.wml?board=%s\" />����</anchor></p>", xb->filename);
			}
			else
			{
				printf ("<p><anchor><go href=\"bbspst.wml?board=*mail\" />д��</anchor></p>");
			}
			for (i = 0; i < ps; i++)
			{
				wmlposttitle(x + i, start + i);
			}
			sprintf (buf, "bbsdoc.wml?board=%s&amp;mode=%d&amp;start=", board, boardmode);
			printf ("<p><anchor><go href=\"%s%d\" />&lt;&lt;</anchor> ", buf, 1);
			for (i = 0; *(pagerange + i); i++)
			{
				ps = start + *(pagerange + i) * WMLS_POST_PER_PAGE;
				if (ps < 0 && ps + WMLS_POST_PER_PAGE >= 0)
				{
					ps = 0;
				}
				else if (ps >= total - WMLS_POST_PER_PAGE && ps < total)
				{
					ps = total - WMLS_POST_PER_PAGE;
				}
				if (ps >= 0 && ps <= total - WMLS_POST_PER_PAGE)
				{
					printf ("<anchor><go href=\"%s%d\" />%+d</anchor> ", buf, ps + 1, *(pagerange + i));
				}
				else
				{
					printf ("%+d ", *(pagerange + i));
				}
			}
			printf ("<anchor><go href=\"%s%d\" />&gt;&gt;</anchor></p> ", buf, total - WMLS_POST_PER_PAGE + 1);
			if (xb)
			{
				printf ("<p><anchor><go href=\"bbspst.wml?board=%s\" />����</anchor> <anchor><go href=\"bbsbrdchg.wml?board=%s\" />�ղ�</anchor> ", xb->filename, xb->filename, xb->filename);
				if (xb->flag & VOTE_FLAG)
				{
					printf ("<anchor><go href=\"bbsvote.wml?board=%s\" />ͶƱ</anchor> ", xb->filename);
				}
				printf ("<anchor><go href=\"boarddetail.wml?board=%s\" />����</anchor></p>", xb->filename);
			}
			else
			{
				printf ("<p><anchor><go href=\"bbspst.wml?board=*mail\" />д��</anchor></p>");
			}
		}
		else
		{
			printf ("<p>�޷���������</p>");
		}
	}
	else
	{
		printf ("<card title=\"�����������\">");
		printf ("<p>�����������ڻ�����Ȩ�������������</p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
}

int wmlposttitle(struct fileheader * x, int num)
{
	char * ptr;
	char buf[80];
	printf ("<p>");
	printf ("<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s&amp;mode=%d\" />", board, x->filename, boardmode);
	w_hprintf ("%s ", x->title);
	printf ("</anchor><br />");
	if (*board == '*')
	{
		if (!(x->accessed[0] & FILE_READ))
		{
			printf ("N ");
		}
	}
	else if (boardmode == 8)//�ö�
	{
		if (x->accessed[1] & FILE_STYLE1)
		{
			printf("[����] ");
		}
		else if (x->accessed[1] & FILE_STYLE2)
		{
			printf("[���] ");
		}
		else if (x->accessed[1] & FILE_STYLE3)
		{
			printf("[�Ƽ�] ");
		}
		else if (x->accessed[1] & FILE_STYLE4)
		{
			printf("[����] ");
		}
		else if (x->accessed[1] & FILE_STYLE5)
		{
			printf("[����] ");
		}
		else 
		{
			printf("[��ʾ] ");
		}
	}
	else
	{
		ptr = flag_str2 (x->accessed[0], brc_has_read(x->filename));
		printf ("%s ", ptr);
	}
	strncpy (buf, x->owner, IDLEN + 1);
	for (ptr = buf; *ptr; ptr++)
	{
		if (!(isalnum(*ptr)))
		{
			*ptr = 0;
			break;
		}
	}
	printf ("<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor> ", buf, buf);
	shorttime (buf, atoi(x->filename + 2));
	w_hprintf ("%s #%d ", buf, num + 1);
	printf ("</p>");
	return 0;
}
