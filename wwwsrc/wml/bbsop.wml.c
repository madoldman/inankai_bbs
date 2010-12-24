//bbsop.wml.c
//huangxu@nkbbs.org

//�����²���

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"
#include "announce.inc"

//���²���
//bbsop.wml?board=(1)&mode=(2)&file=(3)&act=(4)
//act:
//0 -- ��ʾҳ��
//1 -- F
//2 -- ^C
//16 -- _x
//32 -- d
//256 -- G//�ż���Ч
//512 -- M
//1024 -- Y

int bbsop_act(char * info)
{
	char board[80], file[256];
	int boardmode, act;
	char buf[256], buf2[256], buf3[256];
	int i, j, tmp, pl, status, ismail;
	char * ptr;
	struct fileheader fh, fh2;
	struct shortfile * b;
	struct userec * u;
	struct AnnItem ai;
	FILE * fp;
	strncpy (board, getparm("board"), 79);
	strncpy (file, getparm("file"), 255);
	boardmode = atoi(getparm("mode"));
	act = atoi(getparm("act"));
	ismail = !strcmp(board, WML_VB_MAIL);
	//boardModeDir(board, boardmode, buf, buf2);
	if (!loginok || (!ismail && (!has_read_perm(&currentuser, board) || !HAS_PERM(PERM_POST))))
	{
		strcpy (info, "Ȩ�޲�����");
		return -2;
	}
	tmp = fhfromdir(file, board, boardmode, &fh);
	b = getbcache(board);
	pl = (ismail || has_BM_perm(&currentuser, board)) << 8 | !strcmp(currentuser.userid, fh.owner);
	if (ismail)
	{
		boardmode = 0;
	}
	//256:����1:������
	if (tmp < 0)
	{
		sprintf (info, "����ȷ��������Ϣ��", tmp);
		return -1;
	}
	if (*board == '*' && (act & (16 | 256)))
	{
		strcpy (info, "�������޷�ʹ����ժ�򲻿ɻظ���ǡ�");
		return -1;
	}
	if ((boardmode & (1 | 8 | 16)) && (act & (16 | 256 | 512)))
	{
		strcpy (info, "��ģʽ�²��ܽ��б�ǡ�");
		return -1;
	}
	if (act != ACT_ANN_EMBODY && ((act & ~255) && !(pl & 256)) || ((act & 240) && !(pl & 257)))
	{
		strcpy (info, "Ȩ�޲�����");
		return -2;
	}
	status = -1;//����
	strcpy (info, "��������");
	if (!ismail)
	{
		sprintf (buf, "boards/%s/%s", board, fh.filename);
	}
	else
	{
		sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
		if (!file_exist(buf))
		{
			sprintf (buf, "mail/announce/%s", fh.filename);
		}
	}
	if (!(act != 32) && !file_exist(buf))
	{
		strcpy (info, "�ļ������Ѷ�ʧ��");
		return -5;
	}
	switch (act)
	{
		case 1://F
			strncpy (buf3, getparm("to"), IDLEN + 1);
			snprintf(buf2, 60, "[ת��] %s", fh.title);
			if (post_mail_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1,1, "Wapת��") <= 0)
			{
				strcpy (info, "ת��ʧ�ܡ�");
				return -6;
			}
			break;
		case 2://^C
			strncpy (buf3, getparm("to"), 80);
			if (!has_post_perm(&currentuser, buf3))
			{
				sprintf (info, "��������%s���������¡�", buf3);
				return -7;
			}
			snprintf (buf2, 60, "%s(ת��)", fh.title);
			if (post_article_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1, "Wapת��") <= 0)
			{
				sprintf (info, "ת��ʧ�ܡ�");
				return -8;
			}
			if (!ismail)
			{
				sprintf (buf, "Wap cross-post \"%s\" from %s to %s ", fh.title, board, buf3);
				report (buf);
			}
			break;
		case 16://_x
		case 256://G
		case 512://M
			if (act == 16)
			{
				tmp = FILE_NOREPLY;
			}
			else if (act == 256)
			{
				tmp = FILE_DIGEST;
				fh2 = fh;
				*fh2.filename = 'G';
				if (!(fh.accessed[0] & FILE_DIGEST))//����ԭ��û��G��ǵ����
				{
					sprintf (buf2, "cp boards/%s/%s boards/%s/%s -f", board, fh.filename, board, fh2.filename);
					//����һ��G����
					system (buf2);
					boardModeDir(board, 1, buf2, buf3);
					file_append (buf2, &fh2, sizeof(struct fileheader), 1);
					//д��G����
				}
				else
					//�����Ѿ���G����ˣ��ڴ�ȥ��
				{
					sprintf (buf2, "boards/%s/%s -f", board, fh2.filename);
					unlink(buf2);
					//ɾ��G����
					boardModeDir(board, 1, buf2, buf3);
					seekDel(buf2, &fh2);
					//ͬ��������
				}
			}
			else if (act == 512)
			{
				tmp = FILE_MARKED;
			}
			fh.accessed[0] ^= tmp;
			if (boardModeDir(board, boardmode, buf, buf2) || writeBackDir(buf, &fh))
			{
				printf (info, "���ʧ�ܡ�");
				return -9;
			}
			break;
		case 32://d
		case 1024://Y
			//����ȡ���ö���ɾ����ͨ���º�G���£�ֻ����ͨ���½�����վ
			if (act == 32)
			{
				if (!(boardmode == 0 || boardmode == 1 || boardmode == 8))
				{
					sprintf (info, "�ڴ�ģʽ�²���ɾ�ġ�");
					return -10;
				}
				if (!HAS_PERM(PERM_SYSOP))
				{
					if (strstr(board, "syssecurity") || (!strcmp(board, "Punishment") && !HAS_PERM(PERM_BLEVELS)))
					{
						sprintf (info, "����Ȩ��%s��ɾ�ġ�", board);
						return -11;
					}
				}
				if (!(pl & 256) && (fh.accessed[0] & FILE_MARKED))
				{
					sprintf (info, "������ɾ��������M��ǵ����¡�");
					return -14;
				}
				sprintf (buf3, "\n\n[1;%dm�� ɾ��:��%s %s ��Wap[FROM: %.20s][m\n", 31 + rand()%7, BBSNAME, BBSHOST, fromhost);
				f_append (buf, buf3);
				if (!ismail && boardmode == 0)
				{
					u	=	getuser(fh.owner);
					if(!junkboard(board) && u) 
					{
						if(u->numposts>0)
						u->numposts--;
						save_user_data(u);
					}
				}
				if (boardModeDir(board, boardmode, buf, buf2) || boardModeDir(board, 4, buf2, buf3))
				{
					sprintf (info, "�����������");
					return -12;
				}
			}
			else
			{
				if (boardmode != 4)
				{
					sprintf (info, "ֻ�ָܻ�����վ�е����µ������ϡ�");
					return -13;
				}
				//����϶��ǰ��棬buf������δ�ı�
				fp = fopen (buf, "rt");
				if (fp)
				{
					while (fgets(buf, 255, fp))
					{
						if (*buf >= 0 && *buf < ' ')
						{
							break;
						}
						else if (!strncmp(buf, "��  ��: ", 8))
						{
							strcpy (fh.title, buf + 8);
							break;
						}
					}
				}
				if (boardModeDir(board, 4, buf, buf2) || boardModeDir(board, 0, buf2, buf3))
				{
					sprintf (info, "�����������");
					return -12;
				}
			}
			seekDel (buf, &fh);//��ԭ����DIR��ɾ����¼
			if ((boardmode == 0 || boardmode == 4) && !ismail)
				//�����Ļ���Ҳ����˵Ҫд����һ��DIR�����ļ���Ȼ��Ч
			{
				file_append (buf2, &fh, sizeof(struct fileheader), 1);
				sprintf (buf, "%s %s: %s", act == 32 ? "deleted" : "restored", fh.filename, fh.title);
				report (buf);
			}
			else
			{
				if (boardmode != 8)//�����8�Ļ����ö�����ֻ��ָ�����
				{
					//����Ļ���Ҫɾ���ļ�
					if (ismail)
						//���ң����治�ᱻɾ����
					{
						sprintf (buf, "mail/%c/%s/%s", toupper(*currentuser.userid), currentuser.userid, fh.filename);
					}
					else
					{
						sprintf (buf, "boards/%s/%s", board, fh.filename);
					}
					unlink (buf);
					//��ժ����������վ��ɾ���ļ���
					//g�����ô�죿ȥ��G��ǰɣ�
					if (boardmode == 1)//G��ǵĴ���
					{
						*fh.filename = 'M';
						strcpy (buf, fh.filename);
						if (fhfromdir(buf, board, 0, &fh) >= 0)//�ҵ��Ļ�
						{
							fh.accessed[0] &= ~FILE_DIGEST;
							boardModeDir(board, 0, buf2, buf3);
							writeBackDir(buf2, &fh);
							//������ҵ�˵...�ظ����������ɴΡ�����������
						}
					}
				}
			}
			//WARNING ����ı���boardmode
			if (boardmode == 4)
			{
				boardmode = 0;
			}
			else
			{
				boardmode = 4;
			}
			break;
		case ACT_ANN_EMBODY:
			sethomefile(buf2, currentuser.userid, STR_ANN_SINKROAD);
			fp = fopen(buf2, "rt");
			if (!fp)
			{
				sprintf (info, "˿·�ǿյġ�");
				return -7;
			}
			fgets(buf2, LIM_ANN_PATHLEN, fp);
			fclose (fp);
			if (!*buf2)
			{
				sprintf (info, "˿·�ǿյġ�");
				return -7;
			}
			memset (&ai, 0, sizeof (ai));
			strcpy (ai.path, fh.filename);
			strcpy (ai.owner, currentuser.userid);
			strncpy (ai.title, fh.title, 38);
			ai.flag = FLAG_ANN_FILE;
			sprintf (buf3, "0Announce/%s/%s", buf2, ai.path);
			if (file_exist(buf3))
			{
				sprintf (info, "��Ŀ¼�Ѿ���ͬ���ļ����ڡ�");
				return -4;
			}
			sprintf (buf3, "cp -f %s 0Announce/%s/", buf, buf2);
			system(buf3);
			appendAnnIndex(buf2, &ai);
			break;
		case 0://Ĭ��
			break;
		default:
			strcpy (info, "δָ���Ĵ���");
			return -3;
			break;
	}
	if (act)//�����ɹ�
	{
		if (act == 32)
		{
			sprintf (buf, "bbsdoc.wml?board=%s", board);
		}
		else
		{
			sprintf (buf, "bbscon.wml?board=%s&amp;file=%s&amp;mode=%d", board, file, boardmode);
		}
		printf ("<card title=\"�����ɹ� -- %s\" ontime=\"%s\">", BBSNAME, buf);
		printf ("<timer value=\"30\" />");
		printf ("<p>�Զ���ת�С���</p>");
		printf ("<p><anchor><go href=\"%s\" />��������Զ���ת����ʹ�ô����ӡ�</anchor></p>", buf);
		return 0;
	}
	//��������ʾ�б��ʱ����
	printf ("<card title=\"ѡ����� -- %s\">", BBSNAME);
	if (b)
	{
		printf ("<p>���棺<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=%d\" />%s/%s</anchor><br />", b->filename, boardmode, b->title + 11, b->filename);
	}
	else
	{
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />����</anchor><br />", WML_VB_MAIL);
	}
	w_hsprintf (buf, "%s", fh.title);
	printf ("���⣺<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s&amp;mode=%d\" />%s</anchor><br />", board, file, boardmode, buf);
	strncpy (buf, fh.owner, IDLEN + 1);
	for (ptr = buf; *ptr; ptr++)
	{
		if (!(isalnum(*ptr)))
		{
			*ptr = 0;
			break;
		}
	}
	printf ("%s��<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", b ? "����" : "����", buf, buf);
	printf ("��ǣ�%s</p>", flag_str2 (fh.accessed[0], *board == '*' ? (fh.accessed[0] & FILE_READ) : brc_has_read(fh.filename)));
	sprintf (buf, "bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=", board, boardmode, file);
	printf ("<p><input name=\"inp_to\" /><anchor>ת��<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor>&#32;<anchor>ת��<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor></p>", buf, 1, buf, 2);
	printf ("<p><anchor><go href=\"bbsqry.wml?userid=%s\" />����</anchor>&#32;<anchor><go href=\"bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=%d\" />����</anchor>&#32;", fh.owner, board, boardmode, fh.filename, ACT_ANN_EMBODY);
	if (boardmode == 0 && (!(fh.accessed[0] & FILE_NOREPLY) || has_BM_perm(&currentuser, board)) && has_post_perm(&currentuser, board) && isAllowedIp(fromhost))
	{
		printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=0\" />�ظ�</anchor>&#32;", board, fh.filename);
	}
	printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;userid=%s&amp;act=0\" />����</anchor>&#32;", board, fh.filename, fh.owner);
	printf ("</p>");
	if ((pl & 257) && boardmode == 0)
	{
		printf ("<p><anchor><go href=\"%s%d\" />ɾ��</anchor>&#32;<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=256\" />�༭</anchor></p>", buf, 32, board, file);
	}
	if (pl & 256)
	{
		printf ("<p>");
		if (boardmode == 0)
		{
			if (!ismail)
			{
				printf ("<anchor><go href=\"%s%d\" />����RE</anchor>&#32;<anchor><go href=\"%s%d\" />G���</anchor>&#32;", buf, 16, buf, 256);
			}
			printf ("<anchor><go href=\"%s%d\" />M���</anchor>&#32;", buf, 512);
		}
		else if (boardmode == 4)
		{
			printf ("<anchor><go href=\"%s%d\" />�ָ�</anchor>", buf, 1024);
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
	if (bbsop_act(buf))
	{
		printf ("<card title=\"���� -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
