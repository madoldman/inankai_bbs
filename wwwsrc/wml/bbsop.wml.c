//bbsop.wml.c
//huangxu@nkbbs.org

//简单文章操作

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"
#include "announce.inc"

//文章操作
//bbsop.wml?board=(1)&mode=(2)&file=(3)&act=(4)
//act:
//0 -- 显示页面
//1 -- F
//2 -- ^C
//16 -- _x
//32 -- d
//256 -- G//信件无效
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
		strcpy (info, "权限不够。");
		return -2;
	}
	tmp = fhfromdir(file, board, boardmode, &fh);
	b = getbcache(board);
	pl = (ismail || has_BM_perm(&currentuser, board)) << 8 | !strcmp(currentuser.userid, fh.owner);
	if (ismail)
	{
		boardmode = 0;
	}
	//256:斑竹，1:所有者
	if (tmp < 0)
	{
		sprintf (info, "不正确的文章信息。", tmp);
		return -1;
	}
	if (*board == '*' && (act & (16 | 256)))
	{
		strcpy (info, "信箱内无法使用文摘或不可回复标记。");
		return -1;
	}
	if ((boardmode & (1 | 8 | 16)) && (act & (16 | 256 | 512)))
	{
		strcpy (info, "此模式下不能进行标记。");
		return -1;
	}
	if (act != ACT_ANN_EMBODY && ((act & ~255) && !(pl & 256)) || ((act & 240) && !(pl & 257)))
	{
		strcpy (info, "权限不够。");
		return -2;
	}
	status = -1;//错误
	strcpy (info, "参数错误");
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
		strcpy (info, "文件内容已丢失。");
		return -5;
	}
	switch (act)
	{
		case 1://F
			strncpy (buf3, getparm("to"), IDLEN + 1);
			snprintf(buf2, 60, "[转寄] %s", fh.title);
			if (post_mail_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1,1, "Wap转寄") <= 0)
			{
				strcpy (info, "转寄失败。");
				return -6;
			}
			break;
		case 2://^C
			strncpy (buf3, getparm("to"), 80);
			if (!has_post_perm(&currentuser, buf3))
			{
				sprintf (info, "您不能在%s板张贴文章。", buf3);
				return -7;
			}
			snprintf (buf2, 60, "%s(转载)", fh.title);
			if (post_article_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1, "Wap转载") <= 0)
			{
				sprintf (info, "转载失败。");
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
				if (!(fh.accessed[0] & FILE_DIGEST))//这是原来没有G标记的情况
				{
					sprintf (buf2, "cp boards/%s/%s boards/%s/%s -f", board, fh.filename, board, fh2.filename);
					//创建一个G副本
					system (buf2);
					boardModeDir(board, 1, buf2, buf3);
					file_append (buf2, &fh2, sizeof(struct fileheader), 1);
					//写入G索引
				}
				else
					//这里已经有G标记了，在此去掉
				{
					sprintf (buf2, "boards/%s/%s -f", board, fh2.filename);
					unlink(buf2);
					//删除G副本
					boardModeDir(board, 1, buf2, buf3);
					seekDel(buf2, &fh2);
					//同样，清理
				}
			}
			else if (act == 512)
			{
				tmp = FILE_MARKED;
			}
			fh.accessed[0] ^= tmp;
			if (boardModeDir(board, boardmode, buf, buf2) || writeBackDir(buf, &fh))
			{
				printf (info, "标记失败。");
				return -9;
			}
			break;
		case 32://d
		case 1024://Y
			//可以取消置顶、删除普通文章和G文章，只有普通文章进回收站
			if (act == 32)
			{
				if (!(boardmode == 0 || boardmode == 1 || boardmode == 8))
				{
					sprintf (info, "在此模式下不能删文。");
					return -10;
				}
				if (!HAS_PERM(PERM_SYSOP))
				{
					if (strstr(board, "syssecurity") || (!strcmp(board, "Punishment") && !HAS_PERM(PERM_BLEVELS)))
					{
						sprintf (info, "您无权在%s板删文。", board);
						return -11;
					}
				}
				if (!(pl & 256) && (fh.accessed[0] & FILE_MARKED))
				{
					sprintf (info, "您不能删除被板主M标记的文章。");
					return -14;
				}
				sprintf (buf3, "\n\n[1;%dm※ 删除:·%s %s ·Wap[FROM: %.20s][m\n", 31 + rand()%7, BBSNAME, BBSHOST, fromhost);
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
					sprintf (info, "板面参数错误。");
					return -12;
				}
			}
			else
			{
				if (boardmode != 4)
				{
					sprintf (info, "只能恢复回收站中的文章到板面上。");
					return -13;
				}
				//这里肯定是板面，buf变量尚未改变
				fp = fopen (buf, "rt");
				if (fp)
				{
					while (fgets(buf, 255, fp))
					{
						if (*buf >= 0 && *buf < ' ')
						{
							break;
						}
						else if (!strncmp(buf, "标  题: ", 8))
						{
							strcpy (fh.title, buf + 8);
							break;
						}
					}
				}
				if (boardModeDir(board, 4, buf, buf2) || boardModeDir(board, 0, buf2, buf3))
				{
					sprintf (info, "板面参数错误。");
					return -12;
				}
			}
			seekDel (buf, &fh);//从原来的DIR中删掉记录
			if ((boardmode == 0 || boardmode == 4) && !ismail)
				//这样的话，也就是说要写入另一个DIR，即文件仍然有效
			{
				file_append (buf2, &fh, sizeof(struct fileheader), 1);
				sprintf (buf, "%s %s: %s", act == 32 ? "deleted" : "restored", fh.filename, fh.title);
				report (buf);
			}
			else
			{
				if (boardmode != 8)//如果是8的话，置顶文章只是指针而已
				{
					//否则的话，要删掉文件
					if (ismail)
						//而且，公告不会被删掉的
					{
						sprintf (buf, "mail/%c/%s/%s", toupper(*currentuser.userid), currentuser.userid, fh.filename);
					}
					else
					{
						sprintf (buf, "boards/%s/%s", board, fh.filename);
					}
					unlink (buf);
					//文摘区不进回收站，删掉文件。
					//g标记怎么办？去掉G标记吧？
					if (boardmode == 1)//G标记的处理
					{
						*fh.filename = 'M';
						strcpy (buf, fh.filename);
						if (fhfromdir(buf, board, 0, &fh) >= 0)//找到的话
						{
							fh.accessed[0] &= ~FILE_DIGEST;
							boardModeDir(board, 0, buf2, buf3);
							writeBackDir(buf2, &fh);
							//这里好乱的说...重复遍历了若干次。姑且这样。
						}
					}
				}
			}
			//WARNING 这里改变了boardmode
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
				sprintf (info, "丝路是空的。");
				return -7;
			}
			fgets(buf2, LIM_ANN_PATHLEN, fp);
			fclose (fp);
			if (!*buf2)
			{
				sprintf (info, "丝路是空的。");
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
				sprintf (info, "该目录已经有同名文件存在。");
				return -4;
			}
			sprintf (buf3, "cp -f %s 0Announce/%s/", buf, buf2);
			system(buf3);
			appendAnnIndex(buf2, &ai);
			break;
		case 0://默认
			break;
		default:
			strcpy (info, "未指定的错误");
			return -3;
			break;
	}
	if (act)//操作成功
	{
		if (act == 32)
		{
			sprintf (buf, "bbsdoc.wml?board=%s", board);
		}
		else
		{
			sprintf (buf, "bbscon.wml?board=%s&amp;file=%s&amp;mode=%d", board, file, boardmode);
		}
		printf ("<card title=\"操作成功 -- %s\" ontime=\"%s\">", BBSNAME, buf);
		printf ("<timer value=\"30\" />");
		printf ("<p>自动跳转中……</p>");
		printf ("<p><anchor><go href=\"%s\" />如果不能自动跳转，请使用此链接。</anchor></p>", buf);
		return 0;
	}
	//这里是显示列表的时候了
	printf ("<card title=\"选择操作 -- %s\">", BBSNAME);
	if (b)
	{
		printf ("<p>板面：<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=%d\" />%s/%s</anchor><br />", b->filename, boardmode, b->title + 11, b->filename);
	}
	else
	{
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />信箱</anchor><br />", WML_VB_MAIL);
	}
	w_hsprintf (buf, "%s", fh.title);
	printf ("标题：<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s&amp;mode=%d\" />%s</anchor><br />", board, file, boardmode, buf);
	strncpy (buf, fh.owner, IDLEN + 1);
	for (ptr = buf; *ptr; ptr++)
	{
		if (!(isalnum(*ptr)))
		{
			*ptr = 0;
			break;
		}
	}
	printf ("%s：<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", b ? "作者" : "来自", buf, buf);
	printf ("标记：%s</p>", flag_str2 (fh.accessed[0], *board == '*' ? (fh.accessed[0] & FILE_READ) : brc_has_read(fh.filename)));
	sprintf (buf, "bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=", board, boardmode, file);
	printf ("<p><input name=\"inp_to\" /><anchor>转寄<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor>&#32;<anchor>转载<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor></p>", buf, 1, buf, 2);
	printf ("<p><anchor><go href=\"bbsqry.wml?userid=%s\" />作者</anchor>&#32;<anchor><go href=\"bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=%d\" />精华</anchor>&#32;", fh.owner, board, boardmode, fh.filename, ACT_ANN_EMBODY);
	if (boardmode == 0 && (!(fh.accessed[0] & FILE_NOREPLY) || has_BM_perm(&currentuser, board)) && has_post_perm(&currentuser, board) && isAllowedIp(fromhost))
	{
		printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=0\" />回复</anchor>&#32;", board, fh.filename);
	}
	printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;userid=%s&amp;act=0\" />回信</anchor>&#32;", board, fh.filename, fh.owner);
	printf ("</p>");
	if ((pl & 257) && boardmode == 0)
	{
		printf ("<p><anchor><go href=\"%s%d\" />删除</anchor>&#32;<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=256\" />编辑</anchor></p>", buf, 32, board, file);
	}
	if (pl & 256)
	{
		printf ("<p>");
		if (boardmode == 0)
		{
			if (!ismail)
			{
				printf ("<anchor><go href=\"%s%d\" />不可RE</anchor>&#32;<anchor><go href=\"%s%d\" />G标记</anchor>&#32;", buf, 16, buf, 256);
			}
			printf ("<anchor><go href=\"%s%d\" />M标记</anchor>&#32;", buf, 512);
		}
		else if (boardmode == 4)
		{
			printf ("<anchor><go href=\"%s%d\" />恢复</anchor>", buf, 1024);
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
		printf ("<card title=\"错误 -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
