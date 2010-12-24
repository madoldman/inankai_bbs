//bbsano.wml.c
//huangxu@nkbbs.org

//����������
//������announce.inc

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "announce.inc"

//bbsano.wml?path=(1)&act=(2)&title=(3)&cont=(4)&to=(5)
//act:

int bbsano_act(char * info)
{
	char path[LIM_ANN_PATHLEN + 1];
	char buf[4096], buf2[4096], buf3[4096];
	char opinfo[1024];
	char * p;
	struct AnnItem par[LIM_ANN_PATHDIR], *pai, *paa, ai;
	struct AnnItem curr[LIM_ANN_PER_DIR];//512KB����...
	int i, j, t, act, clen;
	int perm, flag, iperm;
	struct shortfile * b;
	struct userec * u;
	FILE * fp;
	*opinfo = 0;
	if (loginok)
	{
		wmlmode (u_info, DIGEST);
	}
	strncpy (path, getparm("path"), LIM_ANN_PATHLEN);
	act = atoi(getparm("act"));
	if (!loginok || !HAS_PERM(PERM_POST))
	{
		strcpy (info, "�Ҵҹ�����Ȩʹ�ñ�ҳ�档");
		return -1;
	}
	clen = LIM_ANN_PER_DIR;
	t = path2Ann(path, par, buf, curr, &clen, &iperm);
	//����ǿ�������˱���Ŀ¼���������ݡ�
	if (t < 0)
	{
		sprintf (info, "Ŀ¼�����ڻ�Ȩ�޲�����%s", buf);
		return -1;
	}
	pai = par + t - 1;
	perm = pai->perm;
	flag = pai->flag;
	strcpy (path, buf);
	if ((act & (ACT_ANN_CP | ACT_ANN_FORWARD)) && !(flag & FLAG_ANN_FILE))
	{
		sprintf (info, "ֻ���ļ�֧�ִ˲�����");
		return -2;
	}
	if ((act & (ACT_ANN_SINKROAD | ACT_ANN_TI | ACT_ANN_PASTE | ACT_ANN_PASTEL | ACT_ANN_MKDIR | ACT_ANN_SETBM )) && !(flag & FLAG_ANN_DIR))
	{
		sprintf (info, "ֻ��Ŀ¼֧�ִ˲�����");
		return -2;
	}
	if ((act & (ACT_ANN_RENAME | ACT_ANN_TO | ACT_ANN_DELETE | ACT_ANN_COPY)) && t < 2)
	{
		sprintf (info, "��Ŀ¼��ֹ���������");
		return -2;
	}
	if (act & ACT_ANN_EDIT)
	{
		if (flag & FLAG_ANN_DIR)//�ڱ�Ŀ¼�з��ģ��½�
		{
			if (!((iperm | perm) & PERM_ANN_ADD))
			{
				strcpy (info, "�������ڴ�Ŀ¼�з��ġ�");
				return -1;
			}
			fp = NULL;
		}
		else if (flag & FLAG_ANN_FILE)//�༭����
		{
			if (!(perm & PERM_ANN_EDIT))
			{
				strcpy (info, "�����ܸĶ���Ŀ¼�е����¡�");
				return -1;
			}
			sprintf (buf, "0Announce/%s", path);
			fp = fopen(buf, "rt");
		}
		else
		{
			strcpy (info, "δ֪����");
			return -3;
		}
		printf ("<card title=\"�༭���� -- %s\">", BBSNAME);
		if (flag & FLAG_ANN_DIR)//Ŀ¼
		{
			w_hsprintf (buf, "%s", pai->title);
			printf ("<p>Ŀ¼��<anchor><go href=\"bbs0an.wml?path=%s\" />%s</anchor></p>", path, buf);
		}
		else//�༭
		{
			w_hsprintf (buf, "%s", (pai - 1)->title);
			strcpy (buf2, path);
			p = (buf2 + strlen(buf2)) - 1;
			if (*p == '/')
			{
				*p = 0;
				p--;
			}
			p = strrchr(buf2, '/');
			if (p)//һ����
			{
				*p = 0;
			}
			printf ("<p>�༭��<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s\" />%s</anchor></p>", WML_VB_ANN, path, buf);
		}
		if (act & ACT_ANN_WML2)
		{
			printf ("<form action=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\">", path, ACT_ANN_SAVE);
		}
		if (flag & FLAG_ANN_DIR)
		{
			printf ("<p>���⣺<input type=\"text\" name=\"%stitle\" /></p>", act & ACT_ANN_WML2 ? "" : "inp_");
		}
		else//��Ȼ��fileģʽ
		{
			printf ("<p><em>��WAP�±༭���¿��ܻ������Ϣ��ʧ��</em></p>");
		}
		if (act & ACT_ANN_WML2)
		{
			printf ("<textarea name=\"cont\" col=\"20\" row=\"5\">");
			if (flag & FLAG_ANN_FILE)//Ҫ��ʾԭ�е�����
			{
				sprintf (buf, "0Announce/%s", path);
				fp = fopen (buf, "rt");
				if (fp)
				{
					while (fgets(buf, 256, fp))
					{
						w_hprintf ("%s", buf);
						printf ("<br />");
					}
					fclose (fp);
				}
			}
			printf ("</textarea>");
			printf ("<p><input type=\"submit\" value=\"�ύ\" /></p>");
			printf ("</form>");
		}
		else
		{
			printf ("<p><input name=\"inp_cont\" /></p>");
			//�ü�ª����
			printf ("<p><anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\">", path, ACT_ANN_SAVE);
			if (flag & FLAG_ANN_DIR)
			{
				printf ("<postfield name=\"title\" value=\"$(inp_title)\" />");
			}
			printf ("<postfield name=\"cont\" value=\"$(inp_cont)\" /></go>�ύ</anchor></p>");
		}
		printf ("<p><anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />�л���WML%d.xģʽ</anchor></p>", path, act ^ ACT_ANN_WML2, (act & ACT_ANN_WML2) ? 1 : 2);
		return 0;
		//�༭����������
	}
	else if (act & ACT_ANN_CP)
	{
		strncpy (buf, getparm("obj"), 80);
		b = getbcache(buf);
		if (!b)
		{
			strcpy (info, "������İ��治��ȷ��");
			return -4;
		}
		sprintf (buf, "Cross-posted '%s' from 0Announce to %s", (par + t - 1)->title, b->filename);
		report (buf);
		sprintf (buf, "0Announce/%s", path);
		post_article_d(b->filename, (par + t - 1)->title, buf, currentuser.userid, currentuser.username, fromhost, -1, "Wap��ת�Ծ�����");
		sprintf (opinfo, "�Ѿ�����ת���� %s �塣", b->filename);
	}
	else if (act & ACT_ANN_FORWARD)
	{
		strncpy (buf, getparm("obj"), 80);
		u = getuser(buf);
		if (!u)
		{
			strcpy (info, "��������û�����ȷ��");
			return -4;
		}
		sprintf (buf, "mailed to %s", u->userid);
		report (buf);
		sprintf (buf, "0Announce/%s", path);
		post_mail_d(u->userid, (par + t - 1)->title, buf, currentuser.userid, currentuser.username, fromhost, -1, 1, "Wap��ת�Ծ�����");
		sprintf (opinfo, "�Ѿ�����ת�� %s��", u->userid);
	}
	else if (act & ACT_ANN_SINKROAD)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_SINKROAD);
		fp = fopen(buf, "wt");
		fprintf (fp, "%s", path);
		fclose (fp);
		strcpy (opinfo, "����˿·�ɹ���"); 
	}
	else if (act & ACT_ANN_RENAME)
	{
		if (!(perm & PERM_ANN_EDIT))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		strncpy (buf, getparm("obj"), 16);
		if (!*buf)
		{
			sprintf (info, "Ŀ���ַ�������Ϊ�ա�");
			return -2;
		}
		for (p = buf; *p; p++)
		{
			if (!isalnum(*p))
			{
				sprintf (info, "������ֻ�ܰ�����ĸ�ͣ������֡�");
				return -5;
			}
		}
		parentDir (path, buf3);
		sprintf (buf2, "0Announce/%s/%s", buf3, buf);
		if (file_exist(buf2))
		{
			sprintf (info, "����ͬ���ļ����ڡ�");
			return -6;
		}
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, pai->perm, curr, 0, LIM_ANN_PER_DIR);//ע�⣡�������˱����Ȩ�ޣ������û������޸��Լ������ļ������֣�==
		}
		paa = annSeekFileI (pai->path, curr, clen);
		if (!paa)
		{
			sprintf (info, "δ֪����");
			return -256;
		}
		sprintf (buf2, "mv 0Announce/%s/%s 0Announce/%s/%s -f", buf3, pai->path, buf3, buf);
		system(buf2);
		strcpy (paa->path, buf);
		createAnnIndex(buf3, pai - 1, curr, clen);
		strcpy (opinfo, "�������ɹ���");
		sprintf (path, "%s/%s", buf3, buf);
		strcpy (pai->path, buf);
	}
	else if (act & ACT_ANN_ORDER)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		j = atoi(getparm("obj"));
		parentDir (path, buf3);
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, pai->perm, curr, 0, LIM_ANN_PER_DIR);
		}
		j--;//�û�������1��ʼ
		if (j < 0 || j >= clen)
		{
			sprintf (info, "�µ�λ��Ӧ����1��%d�ķ�Χ�ڡ�", clen);
			return -6;
		}
		paa = annSeekFileI (pai->path, curr, clen);
		if (!paa)
		{
			sprintf (info, "δ֪����");
			return -256;
		}
		i = paa - curr;
		//��ʵ��ʱ*pai���Ǳ��ļ���һ�����������Բ��ÿ��Ǳ��������
		if (i > j)//����ǰ
		{
			memmove (curr + j + 1, curr + j, sizeof(struct AnnItem) * (i - j));
		}
		else if (j < i)//j > i��ǰ���
		{
			memmove (curr + i, curr + i + 1, sizeof(struct AnnItem) * (j - i));
		}
		*(curr + j) = *pai;//��Path2Ann�õ��ĸ����ָ�
		createAnnIndex(buf3, pai - 1, curr, clen);
		strcpy (opinfo, "�ƶ������ɹ���");
	}
	if (act & (ACT_ANN_TI | ACT_ANN_TO))
	{
		if (!(perm & PERM_ANN_LINK))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		strncpy (buf, getparm("obj"), 38);
		for (p = buf; *p >= ' ' || *p < 0; p++);
		*p = 0;
		if (!*buf)
		{
			sprintf (info, "���ⲻ��Ϊ�ա�");
			return -2;
		}
		if (act & ACT_ANN_TO)//�����Ŀ¼
		{
			parentDir(path, buf3);
			if (pai->flag & FLAG_ANN_DIR)
			{
				clen = item2Ann (buf3, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
			}
			paa = annSeekFileI(pai->path, curr, clen);
			if (paa)//����һ����
			{
				strncpy (paa->title, buf, 38);
				*pai = *paa;
				createAnnIndex (buf3, pai - 1, curr, clen);
			}
		}
		if (act & ACT_ANN_TI)//���ڲ��ļ�����ǰ��һ���жϹ��ˣ���϶��Ǹ�Ŀ¼
		{
			clen = item2Ann (path, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
			strncpy (pai->title, buf, 38);
			createAnnIndex (path, pai, curr, clen);
		}
		sprintf (opinfo, "�ı�������ɹ���");
	}
	else if (act & ACT_ANN_SAVE)
	{
		if (flag & FLAG_ANN_FILE)//���ã����Ǿ�����
		{
			if (!(perm & PERM_ANN_EDIT))
			{
				sprintf (info, "����Ȩ�޸����¡�");
				return -1;
			}
			strcpy (buf, path);
		}
		else if (flag & FLAG_ANN_DIR)//�½���...
		{
			if (!((perm | iperm) & PERM_ANN_ADD))
			{
				sprintf (info, "����Ȩ�ڴ˷��ġ�");
				return -1;
			}
			if (t >= LIM_ANN_PER_DIR)
			{
				sprintf (info, "��Ŀ¼�������Ѿ��ﵽ���ޡ�");
				return -2;
			}
			parentDir (path, buf3);
			memset (&ai, 0, sizeof(ai));
			genAnnFn('A', ai.path);
			strncpy (ai.title, getparm("title"), 38);
			strcpy (ai.owner, currentuser.userid);
			//��ͻ��=_-���������pow(36, 8), ���10��12�η������Һ��Ժ��ˡ�
			ai.flag = FLAG_ANN_FILE;
			formalizeAI (&ai, 1);
			if (!*ai.title)
			{
				strcpy (info, "���ⲻ��Ϊ�գ�");
			}
			appendAnnIndex(path, & ai);
			sprintf (buf, "%s/%s", path, ai.path);
		}
		else//�����ܰ�?
		{
			sprintf (info, "δ֪����");
			return -256;
		}
		sprintf (buf2, "0Announce/%s", buf);
		fp = fopen (buf2, "wt");
		if (!fp)
		{
			sprintf (info, "�޷����ļ���");
			return -5;
		}
		fprintf (fp, "%s", getparm("cont"));
		fclose (fp);
		strcpy (opinfo, "�������³ɹ���");
	}
	else if (act & ACT_ANN_MKDIR)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "�����������ｨ��Ŀ¼��");
			return -1;
		}
		if (t >= LIM_ANN_PER_DIR)
		{
			sprintf (info, "��Ŀ¼�������Ѿ��ﵽ���ޡ�");
			return -2;
		}
		memset (&ai, 0, sizeof(ai));
		strncpy (ai.title, getparm("obj"), 38);
		genAnnFn ('D', ai.path);
		ai.flag = FLAG_ANN_DIR;
		formalizeAI (&ai, 1);
		if (!*ai.title)
		{
			sprintf (info, "Ŀ¼�����б��⡣");
			return -2;
		}
		appendAnnIndex(path, &ai);
		sprintf (buf, "mkdir -p 0Announce/%s/%s", path, ai.path);
		system (buf);
		sprintf (buf, "0Announce/%s/%s", path, ai.path);
		createAnnIndex(buf, &ai, NULL, 0);
		strcpy (opinfo, "����Ŀ¼�ɹ���");
	}
	else if (act & ACT_ANN_DELETE)
	{
		if (!(flag & FLAG_ANN_ERROR) && !*(getparm("obj")))
		{
			sprintf (info, "Ϊ��ֹ���������ɾ������ʱ����������������������ݡ�");
			return -8;
		}
		if (!(perm & PERM_ANN_EDIT))
		{
			sprintf (info, "����Ȩɾ�����ļ���");
			return -1;
		}
		parentDir(path, buf3);
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
		}
		paa = annSeekFileI(pai->path, curr, clen);
		if (paa)//����һ����
		{
			memmove (paa, paa + 1, sizeof(struct AnnItem) * (curr + clen - paa - 1));
			clen --;
			createAnnIndex (buf3, pai - 1, curr, clen);
		}
		if (!(flag & FLAG_ANN_ERROR) && *pai->path)
		{
			sprintf (buf, "rm -Rf 0Announce/%s", path);
			system (buf);
		}
		sprintf (buf, "bbs0an.wml?path=%s", buf3);
		printf ("<card title=\"�ļ���ɾ�� -- %s\" ontimer=\"%s\">", BBSNAME, buf);
		printf ("<p><anchor><go href=\"%s\" />����ת���ϼ�Ŀ¼��</anchor></p>", buf);
		return 0;
		//����Ŀ¼��ɾ�������ͱ���ų���
	}
	else if (act & ACT_ANN_COPY)
	{
		if (!(perm & PERM_ANN_LINK))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_CLIPBOARD);
		fp = fopen(buf, "wt");
		fprintf (fp, "%s", path);
		fclose (fp);
		strcpy (opinfo, "�ѽ��ļ���·�������浽�����塣"); 
	}
	else if (act & (ACT_ANN_PASTE | ACT_ANN_PASTEL))
		//ճ���ȸ��ƷѾ�����=_=
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_CLIPBOARD);
		fp = fopen(buf, "rt");
		if (!fp)
		{
			sprintf (info, "�������ǿյġ�");
			return -7;
		}
		fgets(buf, LIM_ANN_PATHLEN, fp);
		fclose (fp);
		if (!*buf)
		{
			sprintf (info, "�������ǿյġ�");
			return -7;
		}
		if (strstr(path, buf) == path)
		{
			sprintf (info, "��Ŀ¼���ܱ����Ƶ�����Ŀ¼�С�");
			return -8;
		}
		strcpy (buf2, buf);
		p = buf2 + strlen(buf2) - 1;
		if (*p == '/')
		{
			*p = 0;
		}
		for (; p >= buf2 && *p != '/'; p--);
		if (p >= buf2)
		{
			*p = 0;
			p++;
		}
		memset(&ai, 0, sizeof(ai));
		if (annSeekFiles (buf2, PERM_ANN_FULL, &p, 1, &ai) < 1)
		{
			return -9;
		}
		if (act & ACT_ANN_PASTE)
		{
			sprintf (buf2, "0Announce/%s/%s", path, ai.path);
			if (file_exist(buf2))
			{
				sprintf (info, "ͬ���ļ��Ѵ��ڡ�");
				return -9;
			}
		}
		if (act & ACT_ANN_PASTEL)
		{
			genAnnFn('L', ai.path);
			sprintf (buf2, "ln -s %s/0Announce/%s 0Announce/%s/%s", BBSHOME, buf, path, ai.path);
		}
		else
		{
			sprintf (buf2, "cp -R 0Announce/%s 0Announce/%s/", buf, path);
		}
		system (buf2);
		appendAnnIndex(path, &ai);
		strcpy (opinfo, "ճ���ɹ���");
	}
	else if (act & ACT_ANN_SETBM)
	{
		if (!(perm & PERM_ANN_SET))
		{
			sprintf (info, "Ȩ�޲�����");
			return -1;
		}
		strncpy (buf, getparm("obj"), 30);
		if (!*buf)
		{
			memset (pai->owner, 0, sizeof(pai->owner));
		}
		else
		{
			snprintf (pai->owner, sizeof(pai->owner) - 1, "%s%s)", STR_ANN_BM, buf);
		}
		parentDir(path, buf3);
		clen = item2Ann (buf3, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
		paa = annSeekFileI(pai->path, curr, clen);
		if (paa)//����һ����
		{
			*paa = *pai;
			createAnnIndex (buf3, pai - 1, curr, clen);
		}
		sprintf (opinfo, "���ù����߳ɹ���");
	}
	if (act & ACT_ANN_VIEWSR)
	{
		sethomefile(buf, currentuser.userid, STR_ANN_SINKROAD);
		fp = fopen(buf, "rt");
		if (!fp)
		{
			sprintf (info, "˿·�ǿյġ�");
			return -7;
		}
		fgets(buf, LIM_ANN_PATHLEN, fp);
		fclose (fp);
		if (!*buf)
		{
			sprintf (info, "˿·�ǿյġ�");
			return -7;
		}
		strcpy (path, buf);
		t = path2Ann(path, par, buf, NULL, NULL, NULL);
		//�Ҳ���Ҫ֪��̫��
		if (t < 0)
		{
			sprintf (info, "Ŀ¼�����ڻ�Ȩ�޲�����%s", buf);
			return -1;
		}
		printf ("<card title=\"�鿴˿· -- %s\">", BBSNAME);
		printf ("<p>");
		*buf2 = 0;
		for (i = 0; i < t; i++)
		{
			strcat (buf2, (par + i)->path);
			w_hsprintf (buf, "%s", (par + i)->title);
			printf ("<anchor><go href=\"bbs0an.wml?path=%s\" />%s</anchor>/", buf2, buf);
			if (*(buf2 + strlen(buf2) - 1) != '/')
			{
				strcat (buf2, "/");
			}
		}
		printf ("</p>");
		return 0;
	}
	//if (!(act & ~ACT_ANN_WML2))
	{
		printf ("<card title=\"���������� -- %s\">", BBSNAME);
		if (*opinfo)
		{
			printf ("<p>%s</p>", opinfo);
		}
		w_hsprintf (buf, "%s", pai->title);
		if (pai->flag & FLAG_ANN_DIR)
		{
			sprintf (buf2, "bbs0an.wml?path=");
		}
		else
		{
			sprintf (buf2, "bbscon.wml?board=%s&amp;file=", WML_VB_ANN);
		}
		printf ("<p>���⣺<anchor><go href=\"%s%s\" />%s</anchor><br />", buf2, path, buf);
		datestr (buf, pai->mtime);
		paa = annSeekFileI(pai->path, curr, clen);
		if (!paa)
		{
			paa = curr;
		}
		printf ("���ԣ�%s <br />���£�%s<br />", aiModeStr(pai->flag), buf);
		annBMStr (pai->owner, buf);
		//strcpy (buf, pai->owner);
		printf ("%s%s<br />", (flag & FLAG_ANN_FILE) ? "���ߣ�" : "", buf);
		printf ("�ļ���%s</p>", pai->path);
		printf ("<p>");
		printf ("���룺<input name=\"inp_obj\" /><br />");
		printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />�鿴˿·</anchor> ", path, ACT_ANN_VIEWSR);
		if (pai->flag & FLAG_ANN_FILE)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>ת��</anchor> <anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" />ת��</go></anchor><br />", path, ACT_ANN_CP, path, ACT_ANN_FORWARD);
		}
		if (iperm & PERM_ANN_EDIT)
		{
			if (pai->flag & FLAG_ANN_DIR)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>��Ŀ¼</anchor> ", path, ACT_ANN_MKDIR);
			}
			if (perm & PERM_ANN_EDIT)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>���ļ���</anchor> ", path, ACT_ANN_RENAME);
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>����˳��</anchor> ", path, ACT_ANN_ORDER);
			}
			if (perm & PERM_ANN_LINK)
			{
				if (pai->flag & FLAG_ANN_FILE)
				{
					printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>�ı���</anchor> ", path, ACT_ANN_TO);
				}
				else if (pai->flag & FLAG_ANN_DIR)
				{
					printf ("<select title=\"�ı���\" name=\"inp_act\" multi=\"false\"><option value=\"%d\">����</option><option value=\"%d\">�ڲ�</option><option value=\"%d\">�ⲿ</option></select> <anchor><go href=\"bbsano.wml?path=%s\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /><postfield name=\"act\" value=\"$(inp_act)\" /></go>�ı���</anchor> ", ACT_ANN_TO | ACT_ANN_TI, ACT_ANN_TI, ACT_ANN_TO, path);
				}
			}
			if (pai->perm & PERM_ANN_SET)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>����������</anchor> ", path, ACT_ANN_SETBM);
			}
			printf ("<br />");
		}
		if ((pai->flag & FLAG_ANN_FILE) && (pai->perm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />�༭</anchor> ", path, ACT_ANN_EDIT);
		}
		else if ((pai->flag & FLAG_ANN_DIR) && ((perm | iperm) & PERM_ANN_ADD))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />����</anchor> ", path, ACT_ANN_EDIT);
		}
		if ((pai->flag & FLAG_ANN_DIR) && (iperm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />˿·</anchor> ", path, ACT_ANN_SINKROAD);
		}
		if (perm & PERM_ANN_LINK)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />����</anchor> ", path, ACT_ANN_COPY);
		}
		if ((pai->flag & FLAG_ANN_DIR) && (iperm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />ճ��</anchor> ", path, ACT_ANN_PASTE);
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />ճ������</anchor> ", path, ACT_ANN_PASTEL);
		}
		if (perm & PERM_ANN_EDIT)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>ɾ��</anchor> ", path, ACT_ANN_DELETE);
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
	if (bbsano_act(buf))
	{
		printf ("<card title=\"����\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

