//bbscon.wml.c
//huangxu@nkbbs.org

//显示文章内容
//包括文章、信件、精华内容等几乎一切需要显示的东西

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

struct nextfile//512B
{
	char title[256];
	char sfn[256];
};

int nffromdir(const char * dir, const char * fn, struct nextfile * nf, const char * board, int boardmode)
//此函数明显太强大了
{
	FILE * fp;
	struct fileheader fh[WML_TOPIC_RANGE * 3];
	int i, j, posstart, max, cnt, posend, tmp;
	char buf[80], *ptr, cnf, buf2[80], *p, from[IDLEN + 1];
	struct nextfile * pnf;
	fp = fopen(dir, strncmp(dir, "mail", 4) ? "rb" : "rb+");
	if (!fp)
	{
		return -4;
	}
	*buf = 0;
	posstart = 0;
	posend = 0;
	cnt = 0;
	for (;;)
	{
		posend += fread (fh + posend, sizeof (struct fileheader), WML_TOPIC_RANGE * 3 - posend, fp);
		max = WML_TOPIC_RANGE * 2;
		if (posend < max)
		{
			max = posend;
		}
		for (i = posstart; i < max; i++, cnt++)
		{
			if (!strcmp((fh + i)->filename, fn))
			{
				if (!strncmp(dir, "mail", 4) && !((fh + i)->accessed[0] & FILE_READ))
					//更新read表
				{
					(fh + i)->accessed[0] |= FILE_READ;
					tmp = ftell(fp);
					fseek (fp, sizeof(struct fileheader) * cnt, 0);
					fwrite (fh + i, sizeof(struct fileheader), 1, fp);
					fseek (fp, tmp, 0);
				}
				ptr = (fh + i)->title;
				if (!strncasecmp(ptr, "Re:", 3))
				{
					ptr+=4;
				}
				strncpy (buf, ptr, 61);
				memset (nf, 0, sizeof (struct nextfile) * 6);
				strcpy (nf->title, "&lt;");
				strcpy ((nf + 1)->title, "&lt;T");
				strcpy ((nf + 2)->title, "T&gt;");
				strcpy ((nf + 3)->title, "&gt;");
				strcpy ((nf + 4)->title, "退");
				sprintf ((nf + 4)->sfn, "*bbsdoc.wml?board=%s&amp;mode=%d&amp;start=%d", board, boardmode, (cnt / WMLS_POST_PER_PAGE) * WMLS_POST_PER_PAGE + 1);
				cnf = 5;
				strncpy (from, (fh + i)->owner, IDLEN);
				for (ptr = from; *ptr; ptr++)
				{
					if (!(isalnum(*ptr)))
					{
						*ptr = 0;
						break;
					}
				}
				if (boardmode == 0)
				{
					strcpy ((nf + cnf)->title, "T");
					sprintf ((nf + cnf)->sfn, "*bbstopic.wml?board=%s&amp;boardmode=%d&amp;topic=%s&amp;curr=%s", board, boardmode, urlencode2(buf), fn);
					cnf++;
				}
				if (!strncmp(dir, "mail", 4))//邮件
				{
					strcpy ((nf + cnf)->title, "Re");
					sprintf ((nf + cnf)->sfn, "*bbspst.wml?board=%s&amp;file=%s&amp;act=0&amp;userid=%s", board, (fh + i)->filename, from);
					cnf++;
				}
				else if ((boardmode == 0 && (!((fh + i)->accessed[0] & FILE_NOREPLY) || has_BM_perm(&currentuser, board)) && has_post_perm(&currentuser, board) && isAllowedIp(fromhost)))
				{
					strcpy ((nf + cnf)->title, "Re");
					sprintf ((nf + cnf)->sfn, "*bbspst.wml?board=%s&amp;file=%s&amp;act=0", board, (fh + i)->filename);
					cnf++;
				}
				strcpy ((nf + cnf)->title, "作者");
				sprintf ((nf + cnf)->sfn, "*bbsqry.wml?userid=%s", from);
#ifdef MUST_EXISTING_USER
				if (strncmp(dir, "mail", 4) && !postReadable(fh + i))
				{
					return -8;
				}
#endif

				cnf++;
				strcpy ((nf + cnf)->title, "...");
				sprintf ((nf + cnf)->sfn, "*bbsop.wml?board=%s&amp;file=%s&amp;mode=%d", board, (fh + i)->filename, boardmode);
				cnf++;
				if (i > 0)
				{
					strcpy (nf->sfn, (fh + i - 1)->filename);
					for (j = i - 1; j >= 0; j--)
					{
						ptr = (fh + j)->title;
						if (!strncasecmp(ptr, "Re:", 3))
						{
							ptr+=4;
						}
						if (!strncmp(buf, ptr, 56))
						{
							strcpy ((nf + 1)->sfn, (fh + j)->filename);
							break;
						}
					}
				}
				if (i < max - 1)
				{
					strcpy ((nf + 3)->sfn, (fh + i + 1)->filename);
					for (j = i + 1; j < posend; j++)
					{
						ptr = (fh + j)->title;
						if (!strncasecmp(ptr, "Re:", 3))
						{
							ptr+=4;
						}
						if (!strncmp(buf, ptr, 56))
						{
							strcpy ((nf + 2)->sfn, (fh + j)->filename);
							break;
						}
					}
				}
				fclose(fp);
				return cnf;
			}
		}
		if (posend < WML_TOPIC_RANGE * 2)
		{
			break;
		}
		memmove (fh, fh + WML_TOPIC_RANGE, sizeof (struct fileheader) * (posend - WML_TOPIC_RANGE));
		posend -= WML_TOPIC_RANGE;
		posstart = WML_TOPIC_RANGE;
	}
	fclose (fp);
	return -5;//not found
}

int getpath(const char * board, int boardmode, const char * fn, char * buf, struct nextfile * nf)
//>=0表示成功，nf的个数
{
	struct shortfile * x;
	char buf1[1024], buf2[1024], buf3[1024];
	FILE * fp;
	int i;
	char *p;
	struct nextfile * pnf;
	struct fileheader fh;
	if (!*board)
	{
		return -1;//error
	}
	if (strstr(fn, ".."))
	{
		return -2;//error
	}
	if (!strcasecmp(board, WML_VB_MAIL))
	{
		//邮箱
		sprintf (buf2, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
		sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fn);
		if (!file_exist(buf))
		{
			sprintf (buf, "mail/announce/%s", fn);
		}
		return nffromdir(buf2, fn, nf, board, boardmode);
	}
	else if (!strcasecmp(board, WML_VB_ANN))
	{
		if (strcasestr (fn, ".Names") || strcasestr (fn, ".Search"))
		{
			return -3;
		}
		strcpy (buf1, fn);
		p = strrchr (buf1, '/');
		if (p)
		{
			*p = 0;
		}
		sprintf (buf2, "0Announce%s/.Names", buf1);
		pnf = nf;
		strcpy (pnf->title, "向上");
		sprintf (pnf->sfn, "%s/", buf1);
		strncpy (buf1, pnf->sfn, 1023);
		sprintf (pnf->sfn, "*bbs0an.wml?path=%s", buf1);
		sprintf (buf, "0Announce%s", fn);
		if (loginok)
		{
			pnf++;
			strcpy (pnf->title, "...");
			sprintf (pnf->sfn, "*bbsano.wml?path=%s", fn);
		}
		//精华区，检查.Names等标记
		return pnf - nf + 1;
	}
	else if (!strcasecmp(board, WML_VB_SMD))
		//说明档……
	{
		sprintf (buf, "home/%c/%s/plans", toupper(*fn), fn);
		strcpy (nf->title, "用户");
		sprintf (nf->sfn, "*bbsqry.wml?userid=%s", fn);
		return 1;
	}
	else if (!strcasecmp(board, WML_VB_DENIED))
	//封人名单
	{
		if (!has_read_perm(&currentuser, fn))
		{
			return -1;
		}
		//话说，如果版面不存在，就不会有可读的情况了
		sprintf (buf, "boards/%s/deny_users", fn);
		strcpy (nf->title, "回到本板");
		sprintf (nf->sfn, "bbsdoc.wml?board=%s", fn);
		return 1;
	}
	else if (!strcasecmp(board, WML_VB_HEXIE))
		//敏感词
	{
		sprintf (buf, "etc/.badwords");
		return 0;
	}
	x = getbcache(board);
	if (x)
	{
		if ((!canenterboard (currentuser.userid, board) || !has_read_perm(&currentuser, board)) && !HAS_PERM(PERM_SYSOP))
		{
			x = NULL;
		}
	}
	if (!x)
	{
		return -2;
	}
	boardModeDir(board, boardmode, buf2, buf);//DIR放在buf2里边，buf里的东西暂时没用
	i = nffromdir(buf2, fn, nf, board, boardmode);
	sprintf (buf, "boards/%s/%s", x->filename, fn);
	return i;
}

int wmlcon(const char * board, int boardmode, const char * fn, int pagestart)
{
	char buf[1024];
	struct nextfile nf[10];
	int i, nfc, cnt, s, e, start;
	FILE * fp;
	cnt = 0;
	start = (pagestart - 1) * WMLS_LINE_PER_PAGE;
	if (start < 0)
	{
		start = 0;
	}
	nfc = getpath(board, boardmode, fn, buf, nf);
	if (nfc < 0)
	{
		return -1;
	}
	if (file_isfile(buf))
	{
		fp = fopen(buf, "rt");
	}
	else
	{
		fp = NULL;
	}
	if (!fp)
	{
		return -2;
	}
	if (loginok)
	{
		if (isalnum(*board))
		{
			brc_init(currentuser.userid, board);
			brc_add_read(fn);
			brc_update(currentuser.userid, board);
		}
		else if (!strcmp(board, WML_VB_MAIL))
		{
			;
			//在上面处理
		}
	}
	printf ("<card title=\"阅读文章 -- %s\">", BBSNAME);
	printf ("<p><br />");
	for (i = 0; i < start; i++, cnt++)
	{
		if(!fgets(buf, 256, fp))
		{
			break;
		}
	}
	for (i = 0; i < WMLS_LINE_PER_PAGE; i++, cnt++)
	{
		if (!fgets(buf, 256, fp))
		{
			break;
		}
		w_hprintf("%s", buf);
		printf ("<br />");
	}
	while (!feof(fp))
	{
		fgets(buf, 256, fp);
		cnt++;
	}
	fclose (fp);
	printf ("</p>");
	s = pagestart - 3;
	e = s + 4;
	if (s < 0)
	{
		s = 0;
	}
	if (e > cnt / WMLS_LINE_PER_PAGE)
	{
		e = cnt / WMLS_LINE_PER_PAGE;
	}
	if (e)
	{
		printf ("<p>");
		sprintf (buf, "bbscon.wml?board=%s&amp;mode=%d&amp;file=%s&amp;start=", board, boardmode, fn);
		if (s > 0)
		{
			printf ("<anchor><go href=\"%s%d\" />1</anchor>... ", buf, 1);
		}
		for (i = s; i <= e; i++)
		{
			printf ("<anchor><go href=\"%s%d\" />%d</anchor> ", buf, i + 1, i + 1);
		}
		if (e < cnt / WMLS_LINE_PER_PAGE)
		{
			printf ("...<anchor><go href=\"%s%d\" />%d</anchor>", buf, cnt / WMLS_LINE_PER_PAGE + 1, cnt / WMLS_LINE_PER_PAGE + 1);
		}
		printf ("</p>");
	}
	printf ("<p>");
	for (i = 0; i < nfc; i++)
	{
		if (*(nf + i)->sfn)
		{
			if (*(nf + i)->sfn == '*')
			{
				printf ("<anchor><go href=\"%s\" />%s</anchor> ", (nf + i)->sfn + 1, (nf + i)->title);
			}
			else
			{
				printf ("<anchor><go href=\"bbscon.wml?board=%s&amp;mode=%d&amp;file=%s\" />%s</anchor> ", board, boardmode, (nf + i)->sfn, (nf + i)->title);
			}
		}
		else
		{
			printf ("%s ", (nf + i)->title);
		}
	}
	printf ("</p>");
	return 0;
}

int main()
{
	char dir[256];
	char modeinfo[256];
	int boardmode;
	char board[80];
	char fn[256];
	int start, i;
	wml_init();
	wml_head();
	strncpy (board, getparm("board"), 79);
	strncpy (fn, getparm("file"), 255);
	boardmode = atoi(getparm("mode"));
	start = atoi(getparm("start"));
	if (i = wmlcon(board, boardmode, fn, start))
	{
		printf ("<card title=\"错误 -- %s\"><p>参数错误。</p>", BBSNAME);
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
