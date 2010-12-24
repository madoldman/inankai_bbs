//bbsboa.wml.c
//huangxu@nkbbs.org

//�������ӷ��������б�

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

int main()
{
	wml_init();
	wml_head();
	int i, tmp, max, min;
	int z;
	char board[80];
	char buf[256], buf2[256], title[256];
	FILE * fp;
	struct rmd r;
	if (loginok)
	{
		wmlmode(u_info, SELECT);
	}
	z = atoi(getparm("zone"));
	if (!*(getparm("zone")) || z < -1 || z >= SECNUM)
	{
		z = -3;//list mode
	}
	strncpy(board, getparm("board"), 79);
	if (*board)//zoneģʽ
	{
		z = -2;
	}
	if (z == -3)
	{
		printf ("<card title=\"ѡ�������� -- %s\" >", BBSNAME);
		printf ("<p>");
		min = 0;
		if (loginok)
		{
			min -= 2;
		}
		for (i = min; i < SECNUM; i++)
		{
			if (i == -2)
			{
				printf ("&#32;&#32;<anchor><go href=\"bbsboa.wml?zone=-1\" />�ҵ��ղ�</anchor>");
			}
			else if ( i == -1)
			{
				printf ("&#32;&#32;<anchor><go href=\"bbsdoc.wml?board=%s\" />�鿴����</anchor>", WML_VB_MAIL);
			}
			else
			{
				printf ("&#32;&#32;<anchor><go href=\"bbsboa.wml?zone=%d\" />%s</anchor>", i, secname[i][0]);
			}
			if ((i & 1) && (i < SECNUM - 1))
			{
				printf ("<br />");
			}
		}
		printf ("</p>");
		fp = fopen(PATHRMD, "rb");
		if (fp)
		{
			printf ("<p><em>�����Ƽ�</em><br />");
			while (fread (&r, sizeof(struct rmd), 1, fp))
			{
				w_hsprintf(buf, "%s", r.cn);
				printf ("<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s\" />%s</anchor> <anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor>@<anchor><go href=\"bbsdoc.wml?board=%s\" />%s</anchor><br />", r.board, r.en, buf, r.owner, r.owner, r.board, r.board);
			}
			printf ("</p>");
			fclose (fp);
		}
		fp = fopen("etc/posts/day", "rt");
		if (fp)
		{
			printf ("<p><em>ʮ�����Ż���</em><br />");
			fgets(buf, 255, fp);
			fgets(buf, 255, fp);//�������п�ͷ
			for(i = 1; i <= 10; i++) 
			{
				if (!fgets(buf, 255, fp))
				{
					break;
				}
				sscanf (buf + 45, "%s", board);
				if (!fgets(buf, 255, fp))//��˵����ʾIDû���ð�������Ҳ�ϳ���
				{
					break;
				}
				strsncpy (title, buf + 27, 60);
				rtrims(title);
				printf ("<anchor><go href=\"bbstopic.wml?board=%s&amp;topic=%s\" />", board, urlencode2(title));
				w_hsprintf(buf2, "%s", title);
				printf ("%s</anchor> <anchor><go href=\"bbsdoc.wml?board=%s\" />%s��</anchor> <br />", buf2, board, board);
			}
			fclose (fp);
			printf ("</p>");
		}
		printf ("<p><input name=\"inp_goto\" /><anchor><go href=\"bbssel.wml\" method=\"post\"><postfield name=\"goto\" value=\"$(inp_goto)\" /></go>����ȥ</anchor></p>");
		wml_bar(0);
		printf ("</card>");
	}
	else//����
	{
		struct shortfile data[MAXBOARD], *x;
		int total, br;
		printf ("<card title=\"%s -- %s\">", (z >= 0 ? secname[z][0] : (z == -1 ? "�ҵ��ղ�" : board)), BBSNAME);
		if (z == -1)
		{
			char mybrd[GOOD_BRC_NUM][80];
			int mybrdnum = 0;
			mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
			for(i=0; i<mybrdnum; i++) 
			{
				if(!(x=getbcache(mybrd[i])))
				{
					continue;
				}
				if(x->filename[0] <= 32 || x->filename[0] > 'z')
				{
					continue;
				}
				if(!has_read_perm(&currentuser, x->filename))
				{
					continue;
				}
				if(x->flag & ZONE_FLAG)
				{
					continue;
				}
				data[total++] = *x;
			}
		}
		else
		{
			for(i = 0; i < MAXBOARD; i++) 
			{
				x = &(shm_bcache->bcache[i]);
				if(x->filename[0] <= 32 || x->filename[0] > 'z')
				{
					continue;
				}
				if(!has_read_perm(&currentuser, x->filename))
				{
					continue;
				}
				if(z >= 0 && !strchr(seccode[z], x->title[0]))
				{
					continue;
				}
				if (z == -2)
					//����ĳ���ӷ���
				{
					if(!(x->flag & CHILDBOARD_FLAG) && strcasecmp(x->owner, board))
					{
						continue;
					}
				}
				else
					//����ĳ������
				{
					if(x->flag & CHILDBOARD_FLAG)
					{
						continue;
					}
				}
				if((x->flag & ZONE_FLAG) && !count_zone(BOARDS, x->filename))
				{
					continue;
				}
				//��Լ��Դ��
				memcpy(&data[total], x, sizeof(struct shortfile));
				total++;
			}
			qsort(data, total, sizeof(struct shortfile), cmpboard);
			//printf ("<p><anchor><go href=\"userindex.wml?zone=%d\" />��������</anchor></p>", z);
		}
		for (i = 0; i < total; i++)
		{
			printf ("<p>");
			br = board_read(data[i].filename);
			printf ("<anchor><go href=\"%s.wml?board=%s\" />", (data[i].flag & ZONE_FLAG) ? "bbsboa" : "bbsdoc", data[i].filename);
			if (!br)
			{
				printf ("<em>&#42;");
			}
			w_hprintf ("%s/%s", data[i].title + 10, data[i].filename);
			if (!br)
			{
				printf ("</em>");
			}
			printf ("</anchor></p>");
		}
		wml_bar(0);
		printf ("</card>");
	}
	wml_tail();
	return 0;
}
