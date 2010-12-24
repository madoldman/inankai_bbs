//bbsano.wml.c
//huangxu@nkbbs.org

//精华区操作
//动作看announce.inc

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
	struct AnnItem curr[LIM_ANN_PER_DIR];//512KB而已...
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
		strcpy (info, "匆匆过客无权使用本页面。");
		return -1;
	}
	clen = LIM_ANN_PER_DIR;
	t = path2Ann(path, par, buf, curr, &clen, &iperm);
	//这里强制载入了本层目录的所有内容。
	if (t < 0)
	{
		sprintf (info, "目录不存在或权限不够。%s", buf);
		return -1;
	}
	pai = par + t - 1;
	perm = pai->perm;
	flag = pai->flag;
	strcpy (path, buf);
	if ((act & (ACT_ANN_CP | ACT_ANN_FORWARD)) && !(flag & FLAG_ANN_FILE))
	{
		sprintf (info, "只有文件支持此操作。");
		return -2;
	}
	if ((act & (ACT_ANN_SINKROAD | ACT_ANN_TI | ACT_ANN_PASTE | ACT_ANN_PASTEL | ACT_ANN_MKDIR | ACT_ANN_SETBM )) && !(flag & FLAG_ANN_DIR))
	{
		sprintf (info, "只有目录支持此操作。");
		return -2;
	}
	if ((act & (ACT_ANN_RENAME | ACT_ANN_TO | ACT_ANN_DELETE | ACT_ANN_COPY)) && t < 2)
	{
		sprintf (info, "根目录禁止此类操作。");
		return -2;
	}
	if (act & ACT_ANN_EDIT)
	{
		if (flag & FLAG_ANN_DIR)//在本目录中发文，新建
		{
			if (!((iperm | perm) & PERM_ANN_ADD))
			{
				strcpy (info, "您不能在此目录中发文。");
				return -1;
			}
			fp = NULL;
		}
		else if (flag & FLAG_ANN_FILE)//编辑文章
		{
			if (!(perm & PERM_ANN_EDIT))
			{
				strcpy (info, "您不能改动本目录中的文章。");
				return -1;
			}
			sprintf (buf, "0Announce/%s", path);
			fp = fopen(buf, "rt");
		}
		else
		{
			strcpy (info, "未知错误。");
			return -3;
		}
		printf ("<card title=\"编辑文章 -- %s\">", BBSNAME);
		if (flag & FLAG_ANN_DIR)//目录
		{
			w_hsprintf (buf, "%s", pai->title);
			printf ("<p>目录：<anchor><go href=\"bbs0an.wml?path=%s\" />%s</anchor></p>", path, buf);
		}
		else//编辑
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
			if (p)//一定的
			{
				*p = 0;
			}
			printf ("<p>编辑：<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s\" />%s</anchor></p>", WML_VB_ANN, path, buf);
		}
		if (act & ACT_ANN_WML2)
		{
			printf ("<form action=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\">", path, ACT_ANN_SAVE);
		}
		if (flag & FLAG_ANN_DIR)
		{
			printf ("<p>标题：<input type=\"text\" name=\"%stitle\" /></p>", act & ACT_ANN_WML2 ? "" : "inp_");
		}
		else//必然是file模式
		{
			printf ("<p><em>在WAP下编辑文章可能会造成信息丢失。</em></p>");
		}
		if (act & ACT_ANN_WML2)
		{
			printf ("<textarea name=\"cont\" col=\"20\" row=\"5\">");
			if (flag & FLAG_ANN_FILE)//要显示原有的内容
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
			printf ("<p><input type=\"submit\" value=\"提交\" /></p>");
			printf ("</form>");
		}
		else
		{
			printf ("<p><input name=\"inp_cont\" /></p>");
			//好简陋啊。
			printf ("<p><anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\">", path, ACT_ANN_SAVE);
			if (flag & FLAG_ANN_DIR)
			{
				printf ("<postfield name=\"title\" value=\"$(inp_title)\" />");
			}
			printf ("<postfield name=\"cont\" value=\"$(inp_cont)\" /></go>提交</anchor></p>");
		}
		printf ("<p><anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />切换到WML%d.x模式</anchor></p>", path, act ^ ACT_ANN_WML2, (act & ACT_ANN_WML2) ? 1 : 2);
		return 0;
		//编辑动作结束。
	}
	else if (act & ACT_ANN_CP)
	{
		strncpy (buf, getparm("obj"), 80);
		b = getbcache(buf);
		if (!b)
		{
			strcpy (info, "您输入的板面不正确。");
			return -4;
		}
		sprintf (buf, "Cross-posted '%s' from 0Announce to %s", (par + t - 1)->title, b->filename);
		report (buf);
		sprintf (buf, "0Announce/%s", path);
		post_article_d(b->filename, (par + t - 1)->title, buf, currentuser.userid, currentuser.username, fromhost, -1, "Wap·转自精华区");
		sprintf (opinfo, "已经帮您转贴至 %s 板。", b->filename);
	}
	else if (act & ACT_ANN_FORWARD)
	{
		strncpy (buf, getparm("obj"), 80);
		u = getuser(buf);
		if (!u)
		{
			strcpy (info, "您输入的用户不正确。");
			return -4;
		}
		sprintf (buf, "mailed to %s", u->userid);
		report (buf);
		sprintf (buf, "0Announce/%s", path);
		post_mail_d(u->userid, (par + t - 1)->title, buf, currentuser.userid, currentuser.username, fromhost, -1, 1, "Wap·转自精华区");
		sprintf (opinfo, "已经帮您转给 %s。", u->userid);
	}
	else if (act & ACT_ANN_SINKROAD)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_SINKROAD);
		fp = fopen(buf, "wt");
		fprintf (fp, "%s", path);
		fclose (fp);
		strcpy (opinfo, "设置丝路成功。"); 
	}
	else if (act & ACT_ANN_RENAME)
	{
		if (!(perm & PERM_ANN_EDIT))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		strncpy (buf, getparm("obj"), 16);
		if (!*buf)
		{
			sprintf (info, "目标字符串不能为空。");
			return -2;
		}
		for (p = buf; *p; p++)
		{
			if (!isalnum(*p))
			{
				sprintf (info, "重命名只能包含字母和（或）数字。");
				return -5;
			}
		}
		parentDir (path, buf3);
		sprintf (buf2, "0Announce/%s/%s", buf3, buf);
		if (file_exist(buf2))
		{
			sprintf (info, "已有同名文件存在。");
			return -6;
		}
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, pai->perm, curr, 0, LIM_ANN_PER_DIR);//注意！这里用了本层的权限，所以用户可以修改自己个人文集的名字，==
		}
		paa = annSeekFileI (pai->path, curr, clen);
		if (!paa)
		{
			sprintf (info, "未知错误。");
			return -256;
		}
		sprintf (buf2, "mv 0Announce/%s/%s 0Announce/%s/%s -f", buf3, pai->path, buf3, buf);
		system(buf2);
		strcpy (paa->path, buf);
		createAnnIndex(buf3, pai - 1, curr, clen);
		strcpy (opinfo, "重命名成功。");
		sprintf (path, "%s/%s", buf3, buf);
		strcpy (pai->path, buf);
	}
	else if (act & ACT_ANN_ORDER)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		j = atoi(getparm("obj"));
		parentDir (path, buf3);
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, pai->perm, curr, 0, LIM_ANN_PER_DIR);
		}
		j--;//用户所引从1开始
		if (j < 0 || j >= clen)
		{
			sprintf (info, "新的位置应该在1到%d的范围内。", clen);
			return -6;
		}
		paa = annSeekFileI (pai->path, curr, clen);
		if (!paa)
		{
			sprintf (info, "未知错误。");
			return -256;
		}
		i = paa - curr;
		//其实这时*pai就是本文件的一个副本，所以不用考虑保存的问题
		if (i > j)//后向前
		{
			memmove (curr + j + 1, curr + j, sizeof(struct AnnItem) * (i - j));
		}
		else if (j < i)//j > i，前向后
		{
			memmove (curr + i, curr + i + 1, sizeof(struct AnnItem) * (j - i));
		}
		*(curr + j) = *pai;//用Path2Ann得到的副本恢复
		createAnnIndex(buf3, pai - 1, curr, clen);
		strcpy (opinfo, "移动操作成功。");
	}
	if (act & (ACT_ANN_TI | ACT_ANN_TO))
	{
		if (!(perm & PERM_ANN_LINK))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		strncpy (buf, getparm("obj"), 38);
		for (p = buf; *p >= ' ' || *p < 0; p++);
		*p = 0;
		if (!*buf)
		{
			sprintf (info, "标题不能为空。");
			return -2;
		}
		if (act & ACT_ANN_TO)//改外层目录
		{
			parentDir(path, buf3);
			if (pai->flag & FLAG_ANN_DIR)
			{
				clen = item2Ann (buf3, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
			}
			paa = annSeekFileI(pai->path, curr, clen);
			if (paa)//这是一定的
			{
				strncpy (paa->title, buf, 38);
				*pai = *paa;
				createAnnIndex (buf3, pai - 1, curr, clen);
			}
		}
		if (act & ACT_ANN_TI)//改内层文件名，前面一定判断过了，这肯定是个目录
		{
			clen = item2Ann (path, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
			strncpy (pai->title, buf, 38);
			createAnnIndex (path, pai, curr, clen);
		}
		sprintf (opinfo, "改标题操作成功。");
	}
	else if (act & ACT_ANN_SAVE)
	{
		if (flag & FLAG_ANN_FILE)//还好，覆盖就是了
		{
			if (!(perm & PERM_ANN_EDIT))
			{
				sprintf (info, "您无权修改文章。");
				return -1;
			}
			strcpy (buf, path);
		}
		else if (flag & FLAG_ANN_DIR)//新建啊...
		{
			if (!((perm | iperm) & PERM_ANN_ADD))
			{
				sprintf (info, "您无权在此发文。");
				return -1;
			}
			if (t >= LIM_ANN_PER_DIR)
			{
				sprintf (info, "本目录中子项已经达到上限。");
				return -2;
			}
			parentDir (path, buf3);
			memset (&ai, 0, sizeof(ai));
			genAnnFn('A', ai.path);
			strncpy (ai.title, getparm("title"), 38);
			strcpy (ai.owner, currentuser.userid);
			//冲突？=_-这个概率是pow(36, 8), 大概10的12次方……我忽略好了。
			ai.flag = FLAG_ANN_FILE;
			formalizeAI (&ai, 1);
			if (!*ai.title)
			{
				strcpy (info, "标题不能为空！");
			}
			appendAnnIndex(path, & ai);
			sprintf (buf, "%s/%s", path, ai.path);
		}
		else//不可能吧?
		{
			sprintf (info, "未知错误。");
			return -256;
		}
		sprintf (buf2, "0Announce/%s", buf);
		fp = fopen (buf2, "wt");
		if (!fp)
		{
			sprintf (info, "无法打开文件。");
			return -5;
		}
		fprintf (fp, "%s", getparm("cont"));
		fclose (fp);
		strcpy (opinfo, "保存文章成功。");
	}
	else if (act & ACT_ANN_MKDIR)
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "您不能在这里建立目录。");
			return -1;
		}
		if (t >= LIM_ANN_PER_DIR)
		{
			sprintf (info, "本目录中子项已经达到上限。");
			return -2;
		}
		memset (&ai, 0, sizeof(ai));
		strncpy (ai.title, getparm("obj"), 38);
		genAnnFn ('D', ai.path);
		ai.flag = FLAG_ANN_DIR;
		formalizeAI (&ai, 1);
		if (!*ai.title)
		{
			sprintf (info, "目录必须有标题。");
			return -2;
		}
		appendAnnIndex(path, &ai);
		sprintf (buf, "mkdir -p 0Announce/%s/%s", path, ai.path);
		system (buf);
		sprintf (buf, "0Announce/%s/%s", path, ai.path);
		createAnnIndex(buf, &ai, NULL, 0);
		strcpy (opinfo, "建立目录成功。");
	}
	else if (act & ACT_ANN_DELETE)
	{
		if (!(flag & FLAG_ANN_ERROR) && !*(getparm("obj")))
		{
			sprintf (info, "为防止误操作，在删除内容时请在输入框中输入任意内容。");
			return -8;
		}
		if (!(perm & PERM_ANN_EDIT))
		{
			sprintf (info, "您无权删除此文件。");
			return -1;
		}
		parentDir(path, buf3);
		if (pai->flag & FLAG_ANN_DIR)
		{
			clen = item2Ann (buf3, PERM_ANN_FULL, curr, 0, LIM_ANN_PER_DIR);
		}
		paa = annSeekFileI(pai->path, curr, clen);
		if (paa)//这是一定的
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
		printf ("<card title=\"文件已删除 -- %s\" ontimer=\"%s\">", BBSNAME, buf);
		printf ("<p><anchor><go href=\"%s\" />将跳转到上级目录。</anchor></p>", buf);
		return 0;
		//这里目录都删除啦，就别等着出错啊
	}
	else if (act & ACT_ANN_COPY)
	{
		if (!(perm & PERM_ANN_LINK))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_CLIPBOARD);
		fp = fopen(buf, "wt");
		fprintf (fp, "%s", path);
		fclose (fp);
		strcpy (opinfo, "已将文件（路径）保存到剪贴板。"); 
	}
	else if (act & (ACT_ANN_PASTE | ACT_ANN_PASTEL))
		//粘贴比复制费尽多了=_=
	{
		if (!(iperm & PERM_ANN_EDIT))
		{
			sprintf (info, "权限不够。");
			return -1;
		}
		sethomefile(buf, currentuser.userid, STR_ANN_CLIPBOARD);
		fp = fopen(buf, "rt");
		if (!fp)
		{
			sprintf (info, "剪贴板是空的。");
			return -7;
		}
		fgets(buf, LIM_ANN_PATHLEN, fp);
		fclose (fp);
		if (!*buf)
		{
			sprintf (info, "剪贴板是空的。");
			return -7;
		}
		if (strstr(path, buf) == path)
		{
			sprintf (info, "父目录不能被复制到其子目录中。");
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
				sprintf (info, "同名文件已存在。");
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
		strcpy (opinfo, "粘贴成功。");
	}
	else if (act & ACT_ANN_SETBM)
	{
		if (!(perm & PERM_ANN_SET))
		{
			sprintf (info, "权限不够。");
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
		if (paa)//这是一定的
		{
			*paa = *pai;
			createAnnIndex (buf3, pai - 1, curr, clen);
		}
		sprintf (opinfo, "设置管理者成功。");
	}
	if (act & ACT_ANN_VIEWSR)
	{
		sethomefile(buf, currentuser.userid, STR_ANN_SINKROAD);
		fp = fopen(buf, "rt");
		if (!fp)
		{
			sprintf (info, "丝路是空的。");
			return -7;
		}
		fgets(buf, LIM_ANN_PATHLEN, fp);
		fclose (fp);
		if (!*buf)
		{
			sprintf (info, "丝路是空的。");
			return -7;
		}
		strcpy (path, buf);
		t = path2Ann(path, par, buf, NULL, NULL, NULL);
		//我不需要知道太多
		if (t < 0)
		{
			sprintf (info, "目录不存在或权限不够。%s", buf);
			return -1;
		}
		printf ("<card title=\"查看丝路 -- %s\">", BBSNAME);
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
		printf ("<card title=\"精华区操作 -- %s\">", BBSNAME);
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
		printf ("<p>标题：<anchor><go href=\"%s%s\" />%s</anchor><br />", buf2, path, buf);
		datestr (buf, pai->mtime);
		paa = annSeekFileI(pai->path, curr, clen);
		if (!paa)
		{
			paa = curr;
		}
		printf ("属性：%s <br />更新：%s<br />", aiModeStr(pai->flag), buf);
		annBMStr (pai->owner, buf);
		//strcpy (buf, pai->owner);
		printf ("%s%s<br />", (flag & FLAG_ANN_FILE) ? "作者：" : "", buf);
		printf ("文件：%s</p>", pai->path);
		printf ("<p>");
		printf ("输入：<input name=\"inp_obj\" /><br />");
		printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />查看丝路</anchor> ", path, ACT_ANN_VIEWSR);
		if (pai->flag & FLAG_ANN_FILE)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>转载</anchor> <anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" />转寄</go></anchor><br />", path, ACT_ANN_CP, path, ACT_ANN_FORWARD);
		}
		if (iperm & PERM_ANN_EDIT)
		{
			if (pai->flag & FLAG_ANN_DIR)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>建目录</anchor> ", path, ACT_ANN_MKDIR);
			}
			if (perm & PERM_ANN_EDIT)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>改文件名</anchor> ", path, ACT_ANN_RENAME);
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>调整顺序</anchor> ", path, ACT_ANN_ORDER);
			}
			if (perm & PERM_ANN_LINK)
			{
				if (pai->flag & FLAG_ANN_FILE)
				{
					printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>改标题</anchor> ", path, ACT_ANN_TO);
				}
				else if (pai->flag & FLAG_ANN_DIR)
				{
					printf ("<select title=\"改标题\" name=\"inp_act\" multi=\"false\"><option value=\"%d\">内外</option><option value=\"%d\">内部</option><option value=\"%d\">外部</option></select> <anchor><go href=\"bbsano.wml?path=%s\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /><postfield name=\"act\" value=\"$(inp_act)\" /></go>改标题</anchor> ", ACT_ANN_TO | ACT_ANN_TI, ACT_ANN_TI, ACT_ANN_TO, path);
				}
			}
			if (pai->perm & PERM_ANN_SET)
			{
				printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>设置所有者</anchor> ", path, ACT_ANN_SETBM);
			}
			printf ("<br />");
		}
		if ((pai->flag & FLAG_ANN_FILE) && (pai->perm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />编辑</anchor> ", path, ACT_ANN_EDIT);
		}
		else if ((pai->flag & FLAG_ANN_DIR) && ((perm | iperm) & PERM_ANN_ADD))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />发文</anchor> ", path, ACT_ANN_EDIT);
		}
		if ((pai->flag & FLAG_ANN_DIR) && (iperm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />丝路</anchor> ", path, ACT_ANN_SINKROAD);
		}
		if (perm & PERM_ANN_LINK)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />复制</anchor> ", path, ACT_ANN_COPY);
		}
		if ((pai->flag & FLAG_ANN_DIR) && (iperm & PERM_ANN_EDIT))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />粘贴</anchor> ", path, ACT_ANN_PASTE);
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />粘贴链接</anchor> ", path, ACT_ANN_PASTEL);
		}
		if (perm & PERM_ANN_EDIT)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" method=\"post\"><postfield name=\"obj\" value=\"$(inp_obj)\" /></go>删除</anchor> ", path, ACT_ANN_DELETE);
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
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

