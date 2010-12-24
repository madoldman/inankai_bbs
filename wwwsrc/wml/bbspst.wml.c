//bbspst.wml.c
//huangxu@nkbbs.org

//�������½����༭������
//�������£��ż�

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

//bbspst.wml?board=(1)&tite=(2)&userid=(3)&file=(4)&act=(5)&boardmode=(6)
//�ʼ���board=*mail&title=%s&userid=%s&file=%s
//���ģ�board=%s&title=%s&file=%s

//act = 0: �½�
//act = 1: �ύ
//act = 256: �༭
//act = 257: ����

char board[80];
char title[256];
char userid[256];
char file[256];
struct shortfile * b;
int act;
int boardmode;

int bbspst_act(char * info)
{
	char buf[1024], buf2[1024];
	char * p, * cont;
	int tmp, i;
	int ismail, isbm, qmd;
	int nore, outgo;
	FILE * fp;
	struct fileheader fh;
	memset (&fh, 0, sizeof(fh));
	if (!loginok)
	{
		sprintf (info, "�Ҵҹ�����Ȩʹ�ø�ҳ����ʹ�����ļ��������%s��ע��ID��", BBSNAME);
		return -1;
	}
	act = atoi(getparm("act"));// ^ 65536;//�Ҿ���Ĭ��wml2.x��
	strncpy (board, getparm("board"), 79);
	strncpy (title, getparm("title"), 79);
	strncpy (userid, getparm("userid"), 255);
	strncpy (file, getparm("file"), 255);
	nore = atoi(getparm("nore"));
	outgo = atoi(getparm("outgo"));
	boardmode = atoi(getparm("boardmode"));
	b = getbcache(board);
	ismail = !strcmp(board, WML_VB_MAIL) || *userid;
	wmlmode(u_info, ismail ? SMAIL : POSTING);
	isbm = ismail || has_BM_perm(&currentuser, board);
	if (act == 0 || act == 1)
	{
		if (!HAS_PERM(PERM_POST))
		{
			sprintf (info, "����δ��÷���Ȩ�ޡ�");
			return -1;
		}
		if (!ismail && !isAllowedIp(fromhost))
		{
			sprintf (info, "�Բ�������Ҫ���й���½����������������ܷ��ġ���ʹ�����ļ�������ʱ�վ��ִ�д˲�����");
			return -5;
		}
		if (!ismail && !has_post_perm(&currentuser, board))
		{
			sprintf (info, "����Ȩ�ڴ˰��淢�����¡�");
			//TODO: ǿ��ģ�巢��
			return -1;
		}
		if (!ismail && boardmode && !(act & 256))
		{
			sprintf (info, "����ģʽ�²��ܷ������¡�");
			return -6;
		}
	}
	//boardModeDir(board, boardmode, buf, buf2);
	tmp = fhfromdir(file, board, boardmode, &fh);
	if ((act & 256) && tmp < 0)
	{
		sprintf (info, "�����²����ڡ�");
		return -2;
	}
	if (act & 256)
	{
		if (!ismail && !isbm && strcmp(currentuser.userid, fh.owner))
		{
			sprintf (info, "����Ȩ�༭�����¡�");
			return -3;
		}
	}
	if (!*title && (act & 255) && !(act & 256))//�ձ���
	{
		if (ismail)
		{
			strcpy (title, "û����");
		}
		else
		{
			sprintf (info, "���±����б��⡣");
			return -4;
		}
	}
	//if (!ismail && check_post_limit(board))
	//WARNING: ��ˮ��������ֱ����������ģ��Ƚ�����-_-b
	//��������Ҳû�����ơ�
	//ps:����ǿ�ҷ��Թ�ˮ����
	//{
	//	return -6;
	//}
	if (!(act & 256))
	{
		if (!isbm && (fh.accessed[0] & FILE_NOREPLY || b->flag & NOREPLY_FLAG))
		{
			sprintf (info, "�����º��в���RE���ԣ������ܻظ������¡�");
			return -4;
		}
	}
	if ((act & 255) == 0)//�༭ģʽ
	{
		printf ("<card title=\"%s %s -- %s\">", !ismail ? board : "д��", (act & 256) ? "�༭" : "�½�", BBSNAME);
		if (!(act & 65536))
		{
			printf ("<form action=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">", ismail ? WML_VB_MAIL : board, act | 1);
		}
		if (ismail)
		{
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />����</anchor></p>", WML_VB_MAIL);
		}
		else
		{
			printf ("<p>���棺<anchor><go href=\"bbsdoc.wml?board=%s\" />", board);
			w_hprintf ("%s/%s", b->title + 11, b->filename);
			printf ("</anchor></p>");
		}
		strcpy (buf2, title);
		if (!*buf2)
		{
			p = fh.title;
			if (*p)
			{
				if (!strncasecmp(p, "Re: ", 3))
				{
					p += 4;
				}
				sprintf (buf2, "%s%s", (act & 256) ? "" : "Re: ", p);
			}
		}
		if (!(act & 256))
		{
			w_hsprintf (buf, "%s", buf2);
			if (act & 65536)
			{
				printf ("<p>���⣺<input type=\"text\" name=\"inp_title\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>���⣺<textarea name=\"title\" row=\"1\" col=\"18\">%s</textarea></p>", buf);
			}
		}
		if (ismail)
		{
			w_hsprintf (buf, "%s", userid);
			if (act & 65536)
			{
				printf ("<p>�͸���<input type=\"text\" name=\"inp_userid\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>�͸���<textarea name=\"userid\">%s</textarea></p>", buf);
			}
		}
		if (!(act & 256))
		{
			printf ("<p>ǩ������<select title=\"ǩ����\" multiple=\"false\" name=\"%sqmd\" value=\"0\">", (act & 65536) ? "inp_" : "");
			printf ("<option value=\"0\" title=\"��\">��</option>");
			for (i = 1; i < 6; i++)
			{
				printf ("<option value=\"%d\" title=\"%d\">%d</option>", i, i, i);
			}
			printf ("</select></p>");
		}
		if (!(act & 65536))
		{
			printf ("<p>");
			if (!ismail)
			{
				printf ("<input type=\"checkbox\" name=\"nore\" value=\"1\" />����RE ");
			}
			if (b && (b->flag & OUT_FLAG))
			{
				printf ("<input type=\"checkbox\" name=\"outgo\" />ת�� ");
			}
			printf ("</p>");
			printf ("<textarea name=\"cont\" col=\"20\" rows=\"5\">");
		}
		else
		{
			if (act & 256)
			{
				printf ("<p>ǿ�ҽ��鲻Ҫ��WML1.xģʽ�±༭���¡�</p>");
			}
			printf ("<p><input type=\"text\" name=\"inp_cont\" value=\"");
		}
		if (strcmp(board, WML_VB_MAIL))//���ﲻ����ismail
		{
			sprintf (buf, "boards/%s/%s", board, fh.filename);
		}
		else
		{
			sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
		}
		if (!file_isfile(buf))
		{
			fp = NULL;
		}
		else
		{
			fp = fopen(buf, "rt");//WARNING�������fp�����ǿգ�
		}
		skipHeader(fp);
		if (!(act & 256))//�½�/�ظ�
		{
			if (fp && (!(act & 65536)))
			{
				printf ("\n\n");
				if (fh.owner)
				{
					w_hprintf ("�� �� %s �Ĵ������ᵽ: ��", fh.owner);
					printf ("\n");
				}
				for (i = 0; i < 6; i++)
				{
					if (!readFileLine(buf, 256, fp))
					{
						break;
					}
					else
					{
						w_hprintf (": %s", buf);
						printf ("\n");
					}
				}
			}
		}
		else//�༭
		{
			while (readFileLine(buf, 256, fp))
			{
				w_hprintf ("%s", buf);
				if (!(act & 65536))
				{
					printf ("\n");
				}
			}
		}
		if (fp)
		{
			fclose(fp);
		}
		if (!(act & 65536))
		{
			printf ("</textarea>");
			printf ("<p><input type=\"submit\" value=\"�ύ\" />&#32;&#32;<anchor><prev />����</anchor></p></form>");
		}
		else
		{
			printf ("\" /></p>");
			printf ("<p><anchor>�ύ<go href=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">\
<postfield name=\"title\" value=\"$(inp_title)\"/>\
<postfield name=\"userid\" value=\"$(inp_userid)\"/>\
<postfield name=\"cont\" value=\"$(inp_cont)\"/>\
<postfield name=\"qmd\" value=\"$(inp_qmd)\"/>\
</go></anchor>&#32;&#32;<anchor><prev />����</anchor></p>", ismail ? WML_VB_MAIL : board, act | 1);
		}
		printf ("<p><anchor><go href=\"bbspst.wml?board=%s&amp;userid=%s&amp;file=%s&amp;act=%d&amp;boardmode=%d\" />�л���WML%d.xģʽ</anchor></p>", board, userid, file, act ^ 65536, boardmode, (act & 65536) ? 2 : 1);
	}
	else//�ύģʽ
	{
		if (ismail)
		{
			nore = 0;
		}
		if (act & 256)//�޸�
		{
			printf ("<card title=\"���� -- %s\">", BBSNAME);
			if (!ismail)//�������ismail��
			{
				sprintf (buf, "boards/%s/%s", board, fh.filename);
			}
			else
			{
				sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
			}
			fp = fopen(buf, "rt+");
			if (fp)
			{
				skipHeader(fp);
				ftruncate (fileno(fp), ftell(fp));
				fprintf (fp, "%s", getparm("cont"));
				fprintf (fp, "\n[1;%dm�� �޸�:�� %s %s ��Wap[FROM: %.20s][m\n", 31+rand()%7, BBSNAME, BBSHOST, fromhost);
				fclose (fp);
			}
			else
			{
				printf ("<p>�޷����ļ���</p>");
			}
		}
		else//���Ļ���
		{
			qmd = atoi(getparm("qmd"));
			cont = getparm("cont");//ע����ʱ����getparm
			if (!ismail && !(b->flag2&NOFILTER_FLAG) && (bad_words(title) || bad_words(cont) ))
			{
				//printf ("�������¿��ܰ���������ʾ�����ݣ��Ѿ��ύ��ˡ��뷵�ز������޸ġ�");
				sprintf (buf2, "TEMP-%s", board);
				strcpy (board, "TEMP");
			}
			else
			{
				strcpy (buf2, board);
			}
			sprintf (buf, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
			f_append (buf, cont);//��ֹgetparm���
			if (ismail)
			{
				tmp = post_mail_d(userid, title, buf, currentuser.userid, currentuser.username, fromhost, qmd - 1, 1, "Wap");
			}
			else
			{
				if (nore)
				{
					nore = FILE_NOREPLY;
				}
				tmp = post_article2_d (buf2, title, buf, currentuser.userid, currentuser.username, fromhost, qmd - 1, nore, 0, outgo, "Wap");
				if(!junkboard(board)) 
				{
					currentuser.numposts++;
					save_user_data(&currentuser);
				}
			}
			unlink (buf);
			if (tmp <= 0)
			{
				sprintf (info, "�����͵�%sʧ�ܡ�", ismail ? "�ż�" : "����");
				return -7;
			}
			if (!ismail)
			{
				sprintf(buf2, "M.%d.A", tmp);
				brc_init(currentuser.userid, board);
				brc_add_read(buf2);
				brc_update(currentuser.userid, board);
				sprintf(buf, "posted Wap '%s' on '%s'", title, board);
			}
			else
			{
				sprintf(buf, "mailed Wap %s", userid);
			}
			report (buf);
			printf ("<card title=\"�ɹ� -- %s\" ontimer=\"bbsdoc.wml?board=%s\">", BBSNAME, board);
			printf ("<timer value=\"30\" />");
			if (!strcmp (board, "TEMP"))
			{
				printf ("<p><em>�����͵����ݺ������дʣ���Ҫ��˺���ܳ����ڰ����ϡ�</em></p>");
			}
			printf ("<p>�����͵�%s�ѳɹ��ͳ���</p>", ismail ? "�ż�" : "����");
			printf ("<p>�Զ���ת�С���</p>");
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />��������Զ���ת����ʹ�ô����ӡ�</anchor></p>", board);
		}
	}
	return 0;
}

int main()
{
	char buf[1024];
	wml_init();
	wml_head();
	if (bbspst_act(buf))
	{
		printf ("<card title=\"����\" >");
		printf ("<p>%s</p>", buf);
		printf ("<p><anchor><prev />����</anchor></p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
