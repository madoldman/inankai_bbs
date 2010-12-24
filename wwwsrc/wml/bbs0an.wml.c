//bbs0an.wml.c
//huangxu@nkbbs.org

//精华区列表

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "announce.inc"

//bbs0an.wml?path=(1)&start=(2)

int bbs0an_act(char * info)
{
	struct AnnItem par[LIM_ANN_PATHDIR], curr[WMLS_POST_PER_PAGE];
	char path[LIM_ANN_PATHLEN + 1], buf[(LIM_ANN_PATHLEN + 1) * 2], buf2[(LIM_ANN_PATHLEN + 1) * 2];
	int start;
	int i, t, tmp, ac;
	char * p;
	start = atoi(getparm("start")) - 1;
	if (loginok)
	{
		wmlmode (u_info, DIGEST);
	}
	if (start < 0 || start >= LIM_ANN_PER_DIR)
	{
		start = 0;
	}
	strncpy (path, getparm("path"), LIM_ANN_PATHLEN);
	t = path2Ann(path, par, buf, NULL, NULL, NULL);
	if (t < 0)
	{
		strcpy (info, buf);
		return t;
	}
	strcpy (path, buf);
	if ((par + t - 1)->flag & FLAG_ANN_FILE)
	{
		sprintf (buf, "bbscon.wml?board=%s&amp;file=%s", WML_VB_ANN, path);
		printf ("<card title=\"浏览精华 -- %s\" ontimer=\"%s\" >", BBSNAME, buf);
		printf ("<timer value=\"30\" />");
		printf ("<p><anchor><go href=\"%s\" />您以目录的形式请求了一个文件。正在跳转中……</anchor></p>", buf);
		return 0;
	}
	ac = annCounter (path);
	printf ("<card title=\"浏览精华 -- %s\" >", BBSNAME);
	printf ("<p>");
	tmp = 0;
	if (t > 3)
	{
		printf ("……/");
		tmp = t - 3;
	}
	//strcpy (buf2, "/");
	for (i = 0; i < t; i++)
	{
		strcat (buf2, (par + i)->path);
		if (i >= tmp)
		{
			w_hsprintf (buf, "%s", (par + i)->title);
			printf ("<anchor><go href=\"bbs0an.wml?path=%s\" />%s</anchor>/", buf2, buf);
		}
		if (*(buf2 + strlen(buf2) - 1) != '/')
		{
			strcat (buf2, "/");
		}
	}
	printf ("</p>");
	if ((par + t - 1)->perm & ~PERM_ANN_READ)
	{
		printf ("<p>");
		if ((par + t - 1)->perm & PERM_ANN_ADD)
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />发文</anchor> ", path, ACT_ANN_EDIT);
		}
		if ((par + t - 1)->perm & (~PERM_ANN_READ & ~PERM_ANN_ADD))
		{
			printf ("<anchor><go href=\"bbsano.wml?path=%s&amp;act=%d\" />丝路</anchor> <anchor><go href=\"bbsano.wml?path=%s\" />...</anchor> ", path, ACT_ANN_SINKROAD, path);
		}
		printf ("</p>");
	}
	t = item2Ann (path, (par + t - 1)->perm, curr, start, WMLS_POST_PER_PAGE);
	tmp = (t > start + WMLS_POST_PER_PAGE) ? WMLS_POST_PER_PAGE : (t - start);
	for (i = 0; i < tmp; i++)
	{
		w_hsprintf (buf, "%s", (curr + i)->title);
		if ((curr + i)->flag & FLAG_ANN_FILE)
		{
			sprintf (buf2, "bbscon.wml?board=%s&amp;file=%s%s", WML_VB_ANN, path, (curr + i)->path);
		}
		else
		{
			sprintf (buf2, "bbs0an.wml?path=%s%s", path, (curr + i)->path);
		}
		printf ("<p>");
		printf ("#%d <anchor><go href=\"%s\" />%s</anchor>", i + start + 1, buf2, buf);
		/*printf ("<br />#%d %s ", i + start + 1, aiModeStr((curr + i)->flag));
		annBMStr ((curr + i)->owner, buf);
		printf ("%s", buf);
		if ((curr + i)->mtime)
		{
			datestr(buf, (curr + i)->mtime);
			w_hprintf (" %s", buf);
		}*/
		//以上注释掉了作者和时间的显示
		printf ("</p>");
	}
	if (t > WMLS_POST_PER_PAGE)
	{
		printf ("<p>");
		sprintf (buf, "bbs0an.wml?path=%s&amp;start=", path);
		for (i = 0; i <= t / WMLS_POST_PER_PAGE; i++)
		{
			if (i == 0 || i == t / WMLS_POST_PER_PAGE || abs(i - (start / WMLS_POST_PER_PAGE)) < 3)
			{
				if (i == start / WMLS_POST_PER_PAGE)
				{
					printf ("%d ", i + 1);
				}
				else
				{
					printf ("<anchor><go href=\"%s%d\" />%d</anchor> ", buf, (i * WMLS_POST_PER_PAGE) + 1, i + 1);
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
	if (bbs0an_act(buf))
	{
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

