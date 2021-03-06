//bbspst.wml.c
//huangxu@nkbbs.org

//操作：新建，编辑，保存
//对象：文章，信件

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

//bbspst.wml?board=(1)&tite=(2)&userid=(3)&file=(4)&act=(5)&boardmode=(6)
//邮件：board=*mail&title=%s&userid=%s&file=%s
//发文：board=%s&title=%s&file=%s

//act = 0: 新建
//act = 1: 提交
//act = 256: 编辑
//act = 257: 保存

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
		sprintf (info, "匆匆过客无权使用该页！请使用您的计算机访问%s以注册ID。", BBSNAME);
		return -1;
	}
	act = atoi(getparm("act"));// ^ 65536;//我决定默认wml2.x。
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
			sprintf (info, "您尚未获得发文权限。");
			return -1;
		}
		if (!ismail && !isAllowedIp(fromhost))
		{
			sprintf (info, "对不起，您需要绑定中国大陆地区教育网邮箱才能发文。请使用您的计算机访问本站以执行此操作。");
			return -5;
		}
		if (!ismail && !has_post_perm(&currentuser, board))
		{
			sprintf (info, "您无权在此板面发表文章。");
			//TODO: 强制模板发文
			return -1;
		}
		if (!ismail && boardmode && !(act & 256))
		{
			sprintf (info, "正常模式下才能发表文章。");
			return -6;
		}
	}
	//boardModeDir(board, boardmode, buf, buf2);
	tmp = fhfromdir(file, board, boardmode, &fh);
	if ((act & 256) && tmp < 0)
	{
		sprintf (info, "该文章不存在。");
		return -2;
	}
	if (act & 256)
	{
		if (!ismail && !isbm && strcmp(currentuser.userid, fh.owner))
		{
			sprintf (info, "您无权编辑该文章。");
			return -3;
		}
	}
	if (!*title && (act & 255) && !(act & 256))//空标题
	{
		if (ismail)
		{
			strcpy (title, "没主题");
		}
		else
		{
			sprintf (info, "文章必须有标题。");
			return -4;
		}
	}
	//if (!ismail && check_post_limit(board))
	//WARNING: 灌水机部分是直接输出东西的，比较郁闷-_-b
	//反正现在也没有限制。
	//ps:本人强烈反对灌水机。
	//{
	//	return -6;
	//}
	if (!(act & 256))
	{
		if (!isbm && (fh.accessed[0] & FILE_NOREPLY || b->flag & NOREPLY_FLAG))
		{
			sprintf (info, "该文章含有不可RE属性，您不能回复该文章。");
			return -4;
		}
	}
	if ((act & 255) == 0)//编辑模式
	{
		printf ("<card title=\"%s %s -- %s\">", !ismail ? board : "写信", (act & 256) ? "编辑" : "新建", BBSNAME);
		if (!(act & 65536))
		{
			printf ("<form action=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">", ismail ? WML_VB_MAIL : board, act | 1);
		}
		if (ismail)
		{
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />信箱</anchor></p>", WML_VB_MAIL);
		}
		else
		{
			printf ("<p>板面：<anchor><go href=\"bbsdoc.wml?board=%s\" />", board);
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
				printf ("<p>标题：<input type=\"text\" name=\"inp_title\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>标题：<textarea name=\"title\" row=\"1\" col=\"18\">%s</textarea></p>", buf);
			}
		}
		if (ismail)
		{
			w_hsprintf (buf, "%s", userid);
			if (act & 65536)
			{
				printf ("<p>送给：<input type=\"text\" name=\"inp_userid\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>送给：<textarea name=\"userid\">%s</textarea></p>", buf);
			}
		}
		if (!(act & 256))
		{
			printf ("<p>签名档：<select title=\"签名档\" multiple=\"false\" name=\"%sqmd\" value=\"0\">", (act & 65536) ? "inp_" : "");
			printf ("<option value=\"0\" title=\"无\">无</option>");
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
				printf ("<input type=\"checkbox\" name=\"nore\" value=\"1\" />不可RE ");
			}
			if (b && (b->flag & OUT_FLAG))
			{
				printf ("<input type=\"checkbox\" name=\"outgo\" />转信 ");
			}
			printf ("</p>");
			printf ("<textarea name=\"cont\" col=\"20\" rows=\"5\">");
		}
		else
		{
			if (act & 256)
			{
				printf ("<p>强烈建议不要在WML1.x模式下编辑文章。</p>");
			}
			printf ("<p><input type=\"text\" name=\"inp_cont\" value=\"");
		}
		if (strcmp(board, WML_VB_MAIL))//这里不能用ismail
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
			fp = fopen(buf, "rt");//WARNING：这里的fp可能是空！
		}
		skipHeader(fp);
		if (!(act & 256))//新建/回复
		{
			if (fp && (!(act & 65536)))
			{
				printf ("\n\n");
				if (fh.owner)
				{
					w_hprintf ("【 在 %s 的大作中提到: 】", fh.owner);
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
		else//编辑
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
			printf ("<p><input type=\"submit\" value=\"提交\" />&#32;&#32;<anchor><prev />返回</anchor></p></form>");
		}
		else
		{
			printf ("\" /></p>");
			printf ("<p><anchor>提交<go href=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">\
<postfield name=\"title\" value=\"$(inp_title)\"/>\
<postfield name=\"userid\" value=\"$(inp_userid)\"/>\
<postfield name=\"cont\" value=\"$(inp_cont)\"/>\
<postfield name=\"qmd\" value=\"$(inp_qmd)\"/>\
</go></anchor>&#32;&#32;<anchor><prev />返回</anchor></p>", ismail ? WML_VB_MAIL : board, act | 1);
		}
		printf ("<p><anchor><go href=\"bbspst.wml?board=%s&amp;userid=%s&amp;file=%s&amp;act=%d&amp;boardmode=%d\" />切换到WML%d.x模式</anchor></p>", board, userid, file, act ^ 65536, boardmode, (act & 65536) ? 2 : 1);
	}
	else//提交模式
	{
		if (ismail)
		{
			nore = 0;
		}
		if (act & 256)//修改
		{
			printf ("<card title=\"保存 -- %s\">", BBSNAME);
			if (!ismail)//这里就是ismail了
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
				fprintf (fp, "\n[1;%dm※ 修改:· %s %s ·Wap[FROM: %.20s][m\n", 31+rand()%7, BBSNAME, BBSHOST, fromhost);
				fclose (fp);
			}
			else
			{
				printf ("<p>无法打开文件。</p>");
			}
		}
		else//发文或信
		{
			qmd = atoi(getparm("qmd"));
			cont = getparm("cont");//注意暂时不能getparm
			if (!ismail && !(b->flag2&NOFILTER_FLAG) && (bad_words(title) || bad_words(cont) ))
			{
				//printf ("您的文章可能包含不便显示的内容，已经提交审核。请返回并进行修改。");
				sprintf (buf2, "TEMP-%s", board);
				strcpy (board, "TEMP");
			}
			else
			{
				strcpy (buf2, board);
			}
			sprintf (buf, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
			f_append (buf, cont);//禁止getparm解除
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
				sprintf (info, "您发送的%s失败。", ismail ? "信件" : "文章");
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
			printf ("<card title=\"成功 -- %s\" ontimer=\"bbsdoc.wml?board=%s\">", BBSNAME, board);
			printf ("<timer value=\"30\" />");
			if (!strcmp (board, "TEMP"))
			{
				printf ("<p><em>您发送的内容含有敏感词，需要审核后才能出现在板面上。</em></p>");
			}
			printf ("<p>您发送的%s已成功送出。</p>", ismail ? "信件" : "文章");
			printf ("<p>自动跳转中……</p>");
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />如果不能自动跳转，请使用此链接。</anchor></p>", board);
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
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
		printf ("<p><anchor><prev />返回</anchor></p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
