//bbstopic.wml.c
//huangxu@nkbbs.org

//同主题阅读

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

//bbstopic.wml?board=(1)&mode=(2)&topic=(3)&curr=(4)&start=(5)
//curr与start均可用于定位，有一个就好

int bbstopic_act(char * info)
{
	char board[80], topic[256], curr[256];
	int boardmode, start;
	int total, cnt, i, tmp, ismail, slen;
	struct fileheader fh[WMLP_TPOST_PER_PAGE], fhs[WMLP_TPOST_PER_PAGE], fh1[WMLP_TPOST_PER_PAGE];
	struct shortfile * b;
	char buf[256], buf2[256];
	char * ptr;
	FILE * fp;
	if (loginok)
	{
		wmlmode (u_info, READING);
	}
	strncpy (board, getparm("board"), 79);
	strncpy (topic, getparm("topic"), 255);
	strncpy (curr, getparm("curr"), 255);
	boardmode = atoi(getparm ("mode"));
	start = atoi(getparm("start"));
	ismail = !strcmp(board, WML_VB_MAIL);
	boardModeDir(board, boardmode, buf, buf2);
	b = getbcache(board);
	if (!*topic)
	{
		strcpy (info, "缺少主题。");
		return -1;
	}
	if (!loginok || (!ismail && (!has_read_perm(&currentuser, board) )))
	{
		strcpy (info, "权限不够。");
		return -2;
	}
	fp = fopen (buf, "rb");
	if (!fp)
	{
		sprintf (info, "无法打开索引。");
		return -1;
	}
	printf ("<card title=\"主题树 -- %s\">", BBSNAME);
	if (ismail)
	{
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />信箱</anchor></p>", WML_VB_MAIL);
	}
	else
	{
		printf ("<p>板面：<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=%d\" />%s/%s</anchor></p>", b->filename, boardmode, b->title + 11, b->filename);
	}
	w_hsprintf (buf, "%s", topic);
	printf ("<p>主题：%s</p>", buf);
	cnt = 0;
	slen = 0;
	start --;
	memset (fhs, 0, sizeof(fhs));
	while (tmp = fread(fh, sizeof(struct fileheader), WMLP_TPOST_PER_PAGE, fp))
	{
		for (i = 0; i < tmp; i++)
		{
			ptr = (fh + i)->title;
			if (!strncasecmp(ptr, "Re: ", 3))
			{
				ptr += 4;
			}
			if (!strncmp(topic, ptr, 56))
			{
				cnt++;
				if (start < 0 || start / WMLP_TPOST_PER_PAGE <= cnt / WMLP_TPOST_PER_PAGE)
				{
					*(fhs + slen) = *(fh + i);
					slen = slen % WMLP_TPOST_PER_PAGE + 1;
					//循环使用好吧。
				}
				if (curr && !strcmp((fh + i)->filename, curr))
				{
					start = (cnt - 1) / WMLP_TPOST_PER_PAGE * WMLP_TPOST_PER_PAGE;
				}
				if (cnt == WMLP_TPOST_PER_PAGE)
				{
					memcpy (fh1, fhs, sizeof(fh1));
				}
			}
		}
	}
	if (start < 0)
	{
		if (cnt > WMLP_TPOST_PER_PAGE)
		{
			memcpy (fhs, fh1, sizeof(fh1));
			slen = WMLP_TPOST_PER_PAGE;
		}
		start = 0;
	}
	printf ("<p>同主题%d篇，以下显示%d至%d。</p>", cnt, start + 1, start + slen);
	for (i = 0; i < slen; i++)
	{
		ptr = ismail ? (((fhs + i)->accessed[0] & FILE_READ) ? "" : "N") : flag_str2 ((fhs + i)->accessed[0], brc_has_read((fhs + i)->filename));
		printf ("<p>%s %s ", strncasecmp((fhs + i)->title, "Re: ", 3) ? "T" : "R", ptr);
		strncpy (buf, (fhs + i)->owner, IDLEN + 1);
		for (ptr = buf; *ptr; ptr++)
		{
			if (!(isalnum(*ptr)))
			{
				*ptr = 0;
				break;
			}
		}
		printf ("<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor> ", buf, buf);
		printf ("<anchor><go href=\"bbscon.wml?board=%s&amp;mode=%d&amp;file=%s\" />", board, boardmode, (fhs + i)->filename);
		datetime (buf, atoi((fhs + i)->filename + 2));
		w_hprintf ("%s", buf);
		printf ("</anchor></p>");
	}
	if (cnt > WMLP_TPOST_PER_PAGE)
	{
		printf ("<p>");
		strcpy (buf, urlencode2(topic));
		for (i = 0; i <= cnt / WMLP_TPOST_PER_PAGE; i++)
		{
			if (i == 0 || i == cnt / WMLP_TPOST_PER_PAGE || abs(i - (start / WMLP_TPOST_PER_PAGE)) < 3)
			{
				if (i == start / WMLP_TPOST_PER_PAGE)
				{
					printf ("%d ", i + 1);
				}
				else
				{
					printf ("<anchor><go href=\"bbstopic.wml?board=%s&amp;mode=%d&amp;topic=%s&amp;start=%d\" />%d</anchor> ", board, boardmode, buf, (i * WMLP_TPOST_PER_PAGE) + 1, i + 1);
				}
			}
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
	if (bbstopic_act(buf))
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
