//announce.inc
//BBSLIB.inc required
//huangxu@nkbbs.org

#include "digest.h"

#define STR_ANN_INDEX ".Names"
#define STR_ANN_SNAME "Name="
#define STR_ANN_FNAME "Path="
#define STR_ANN_NUMB "Numb="
#define STR_ANN_TITLE "# Title="
#define STR_ANN_HIDE "<HIDE>"
#define STR_ANN_GUESTBOOK "<GUESTBOOK>"
#define STR_ANN_BM "(BM: "
#define STR_ANN_SINKROAD ".announcepath"
#define STR_ANN_CLIPBOARD ".copypaste"
#define STR_ANN_COUNTER "counter.person"
#define LIM_ANN_PATHLEN 255
#define LIM_ANN_PATHDIR 64
#define LIM_ANN_PER_DIR 1024

#define PERM_ANN_READ 1
#define PERM_ANN_ADD 2
#define PERM_ANN_HIDE 4
#define PERM_ANN_DELETE 8
#define PERM_ANN_EDIT 16
#define PERM_ANN_LINK 32
#define PERM_ANN_SET 64

#define FLAG_ANN_FILE 1
#define FLAG_ANN_DIR 2
#define FLAG_ANN_LINK 4
#define FLAG_ANN_ERROR 8

#define ACT_ANN_VIEW 0
#define ACT_ANN_CP 1
#define ACT_ANN_FORWARD 2
#define ACT_ANN_EDIT 4
#define ACT_ANN_SINKROAD 8
#define ACT_ANN_RENAME 16
#define ACT_ANN_ORDER 32
#define ACT_ANN_TI 64
#define ACT_ANN_TO 128
#define ACT_ANN_SAVE 256
#define ACT_ANN_MKDIR 512
#define ACT_ANN_DELETE 1024
#define ACT_ANN_COPY 2048
#define ACT_ANN_PASTE 4096
#define ACT_ANN_PASTEL 8192
#define ACT_ANN_SETBM 16384
#define ACT_ANN_EMBODY 32768
#define ACT_ANN_WML2 65536
#define ACT_ANN_VIEWSR 131072


#define PERM_ANN_FULL (PERM_ANN_READ | PERM_ANN_ADD | PERM_ANN_HIDE | PERM_ANN_DELETE | PERM_ANN_EDIT | PERM_ANN_LINK | PERM_ANN_SET)

struct AnnItem//128B
{
	char title[40];
	char owner[36];
	char path[40];
	time_t mtime;
	int flag, perm;
};

char * aiModeStr(int flag)
{
	static char AIMS [][8] = {"文章", "目录", "链接", "错误"};
	if (flag & FLAG_ANN_ERROR)
	{
		return AIMS[3];
	}
	else if (flag & FLAG_ANN_LINK)
	{
		return AIMS[2];
	}
	else if (flag & FLAG_ANN_DIR)
	{
		return AIMS[1];
	}
	else
	{
		return AIMS[0];
	}
}

int fileFlag(char * path, struct AnnItem * ai)
{
	int f;
	struct stat st;
	if ((*ai->path <= ' ' && *ai->path >= 0) || stat(path, &st))
	{
		f = FLAG_ANN_ERROR;
		ai->mtime = 0;
	}
	else
	{
		ai->mtime = st.st_mtime;
		f = (S_ISREG(st.st_mode) ? FLAG_ANN_FILE : 0) | (S_ISDIR(st.st_mode) ? FLAG_ANN_DIR : 0) | (S_ISLNK(st.st_mode) ? FLAG_ANN_LINK : 0);
	}
	ai->flag |= f;
	return f;
}

int annbm(char * bmstr)
{
	char * p;
	if (strncmp(bmstr, STR_ANN_BM, strlen(STR_ANN_BM)))
	{
		return 0;
	}
	//未做sysops和bms的判断。
	p = strstr(bmstr, currentuser.userid);
	if (!p)
	{
		return 0;
	}
	if (!(isalnum(*(p - 1))) && !(isalnum(*(p + strlen(p)))))
	{
		return 1;
	}
	return 0;
}

int formalizeAI(struct AnnItem * ai, int len)
{
	int i;
	char * p;
	for (i = 0; i < len; i++)
	{
		//if ((ai + i)->flag && FLAG_ANN_ERROR)//错误，清空路经
		{
		//	*(ai + i)->path = 0;
		}
		//else
		{
			for (p = (ai + i)->path; *p; p++)
			{
				if (!isalnum(*p) && *p != '.' && *p != '_')
				{
					*p = 0;
					break;
				}
			}
		}
		if (((ai + i)->flag & FLAG_ANN_FILE) && *(ai + i)->owner && *(ai + i)->owner != ' ')
		{
			memmove ((ai + i)->owner + 1, (ai + i)->owner, sizeof((ai + i)->owner) - 1);
			*(ai + i)->owner = ' ';
		}
		rtrims((ai + i)->owner);
		for (p = (ai + i)->title; *p; p++)
		{
			if (*p >= 0 && *p < ' ')
			{
				*p = 0;
				break;
			}
		}
		rtrims((ai + i)->title);
	}
	return i;
}

struct AnnItem * annSeekFileI (char * file, struct AnnItem * ai, int len)
{
	struct AnnItem * a;
	for (a = ai; a < ai + len; a++)
	{
		if (!strcmp(file, a->path))
		{
			return a;
		}
	}
	return NULL;
}

int annSeekFiles (char * path, int perm, char ** files, int len, struct AnnItem * ai)
{
	int i, cnt;
	char buf[2048], buf2[2048];
	char * p;
	FILE * fp;
	struct AnnItem aic;
	sprintf (buf, "0Announce%s/%s", path, STR_ANN_INDEX);
	memset (ai, 0, sizeof(struct AnnItem) * len);
	fp = fopen(buf, "rt");
	if (!fp)
	{
		return 0;
	}
	cnt = 0;
	while (fgets(buf, 256, fp))
	{
		if (*buf == '#')
		{
			continue;
		}
		else if (!strncmp(buf, STR_ANN_SNAME, strlen(STR_ANN_SNAME)))
		{
			memset (&aic, 0, sizeof(aic));
			if ((perm & PERM_ANN_HIDE) || strncmp(buf + 5, STR_ANN_HIDE, strlen(STR_ANN_HIDE)))
			{
				strncpy(aic.title, buf + 5, 37);
				strncpy(aic.owner, buf + 43, 36);
			}
		}
		else if (!strncmp(buf, STR_ANN_FNAME, strlen(STR_ANN_FNAME)))
		{
			if (*(aic.title) && strcmp(buf + strlen(STR_ANN_FNAME), STR_ANN_INDEX))
			{
				strncpy(aic.path, buf + 7, 40);
				for (p = aic.path; isalnum(*p) || *p == '.' || *p == '_'; p++);
				*p = 0;
				for (i = 0; i < len; i++)
				{
					if (!strcmp(*(files + i), aic.path))
					{
						sprintf (buf2, "0Announce%s/%s", path, aic.path);
						fileFlag (buf2, &aic);
						aic.perm = perm;
						*(ai + i) = aic;
						cnt++;
						break;
					}
				}
			}
		}
	}
	formalizeAI(ai, cnt);
	return cnt;
}

int annBMStr(char * owner, char * buf)
{
	int l;
	char * p;
	*buf = 0;
	if (!*owner)
	{
		return 0;
	}
	if (p = (strstr(owner, STR_ANN_BM)))
	{
		strcpy (buf, STR_ANN_BM);
		p += strlen(STR_ANN_BM);
	}
	else
	{
		p = owner;
	}
	wmlUseridStr(buf + strlen(buf), p);
	if (p != owner)
	{
		strcat (buf, ")");
	}
	return strlen(buf);
}

int genAnnFn(char head, char * buf)
{
	int i, t;
	unsigned int x;
	unsigned char c;
	memset (buf, 0, sizeof(buf));
	if (!isalnum(head) && head != '.')
	{
		head = 'W';
	}
	*buf = head;
	x = time(NULL);
	for (i = 6; i > 0; i--)
	{
		c = x % 36;
		x /= 36;
		c += (c >= 10 ? 55 : 48);
		*(buf + i) = (char)(c);
	}
	for (i = 7; i < 9; i++)
	{
		c = rand() % 36;
		c += (c >= 10 ? 55 : 48);
		*(buf + i) = (char)(c);
	}
	*(buf + 9) = 0;
	return 9;
}

int path2Ann(char * path, struct AnnItem * ai, char * info, struct AnnItem * aicurr, int * aic, int * indirperm)
//将路径变成ai
//路经，装载路经的ai，返回信息，装载当前目录文件的AI，他的尺寸（读写）
//aicurr只有在非0且path指向文件时才有效。
{
	char buf[1024], buf2[1024];
	char bufpath[LIM_ANN_PATHLEN + 1 + 16];
	char fullpath[LIM_ANN_PATHLEN + 1 + 16];
	char *p, *pbuf;
	struct AnnItem aibuf, *aip;
	int i, tmp;
	int currmode, primode;
	int currperm;
	int pathlen;
	FILE * fp;
	currmode = 0;
	primode = 0;
	currperm = PERM_ANN_READ;
	snprintf (bufpath, sizeof(bufpath), "0Announce/%s", path);
	pbuf = bufpath;
	*fullpath = 0;
	pathlen = 0;
	//strcpy (fullpath, "0Announce/");
	if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_ANNOUNCE))
	{
		currperm = PERM_ANN_FULL;
	}
	fp = NULL;
	for (i = 0; i < LIM_ANN_PATHDIR; i++)
	{
		if (!*pbuf)
		{
			break;//整体分析完毕
		}
		for (p = pbuf; isalnum(*p) || *p == '.' || *p == '_'; p++);
		*p = 0;
		strcpy (buf, pbuf);
		pbuf = p + 1;
		if ((!*buf || !strcmp(buf, ".") || !strcmp(buf, "..")))
		{
			i--;
			continue;
		}
		strcat (fullpath, buf);
		strcat (fullpath, "/");
		if (i == 1)
		{
			if (!strcmp(buf, "PersonalCorpus"))
			{
				primode = 2;//文集
			}
			else if (!strcmp(buf, "groups"))
			{
				primode = 1;//讨论区
				if (HAS_PERM(PERM_OBOARDS))
				{
					currperm |= PERM_ANN_FULL;
				}
			}
			else
			{
				primode = 3;//其他
			}
		}
		else if (i == 3)
		{
			if (primode == 1)
			{
				if (!has_read_perm(&currentuser, buf))
				{
					sprintf (info, "权限不够。");
					return -2;
				}
			}
			else if (primode == 2)
			{
				if (strcmp(currentuser.userid, buf))
				{
					currperm &= ~PERM_ANN_HIDE;
				}
			}
		}
		sprintf (buf2, "%s%s", fullpath, STR_ANN_INDEX);
		fp = fopen(buf2, "rt");//文件打开两次也无所谓的……
		if (!fp)
		//这时候，可能因为是一个文件，也可能因为index丢失
		//但是无论如何都不会是根目录
		{
			if (((ai + pathlen - 1)->flag & FLAG_ANN_DIR) && dirouterTitle(fullpath, ai + pathlen))
			//这层是目录的话，并且成功找到上层索引
			{
				(ai + pathlen)->perm = currperm;
				//注意，该函数取回的PERM无效。
				//另外，由于这是一种错误，所以不处理太多权限啦。
				pathlen++;
			}
			else//文件的话..
			{
				*(fullpath + strlen(fullpath) - 1) = 0;
				for (p = fullpath + strlen(fullpath); p >= fullpath && *p != '/'; p--);//不可能没有/的，最后一个就是/
				*p = 0;
				p++;
				if (aicurr && aic)
				{
					*aic = item2Ann (fullpath + 9, currperm, aicurr, 0, *aic);
					aip = annSeekFileI (p, aicurr, *aic);
					if (!aip)
					{
						sprintf (info, "找不到指定的目录。");
						return -1;
					}
					else
					{
						*(ai + pathlen) = *aip;
						pathlen++;
					}
				}
				else//只要找到目录信息就好
				{
					if (annSeekFiles (fullpath + 9, currperm, &p, 1, &aibuf) <= 0)
					{
						sprintf (info, "找不到指定的目录。");
						return -1;
					}
					else
					{
						*(ai + pathlen) = aibuf;
						pathlen++;
					}
				}
				*(--p) = '/';//恢复=_=
				strcpy (info, fullpath + 9);
				return pathlen;
			}
		}
		else
		{
			while (fgets(buf2, sizeof(buf2) - 1, fp))
			//这里可能读到buf2不够长，于是乎会出现奇怪的错误！
			{
				if (!strncmp(buf2, STR_ANN_TITLE, strlen(STR_ANN_TITLE)))
				{
					strncpy((ai + pathlen)->title, buf2 + strlen(STR_ANN_TITLE), 37);
					p = buf2 + strlen(STR_ANN_TITLE) + 38;
					if (buf2 + strlen(buf2) > p)
					{
						strncpy((ai + pathlen)->owner, p, 36);
					}
					else
					{
						*(ai + pathlen)->owner = 0;
					}
					strncpy((ai + pathlen)->path, buf, 40);
					//文集HIDE不是哄人么？-_b
					if (i > 3)
					{
						if (!(currperm & PERM_ANN_HIDE) && !strncmp((ai + pathlen)->title, STR_ANN_HIDE, strlen(STR_ANN_HIDE)))
						{
							strcpy (info, "您无权查看此目录。");
							return -3;
						}
					}
					tmp = annbm((ai + pathlen)->owner);
					if (tmp)
					{
						currperm |= (PERM_ANN_FULL & ~PERM_ANN_SET & ~PERM_ANN_LINK);
					}
					(ai + pathlen)->perm = currperm;
					fileFlag (fullpath, ai + pathlen);
					if (tmp)
					{
						currperm |= PERM_ANN_FULL;// & ~PERM_ANN_SET;
					}
					if (!strncmp((ai + pathlen)->title, STR_ANN_GUESTBOOK, strlen(STR_ANN_GUESTBOOK)))
					{
						(ai + pathlen)->perm |= PERM_ANN_ADD;
					}
					if (pathlen == 0)
					{
						strcpy ((ai + pathlen)->path, "/");
					}
					formalizeAI(ai + pathlen, 1);
					pathlen ++;
					if (indirperm)
					{
						*indirperm = currperm;
					}
					break;
				}
			}
			fclose (fp);
		}
	}
	strcpy (info, fullpath + 9);
	return pathlen;
}

int item2Ann (char * path, int perm, struct AnnItem * ai, int start, int length)
//返回的是总计记录个数，而不是写入的个数
{
	char buf[1024], buf2[1024];
	int i, skipthis;
	FILE * fp;
	char * p;
	sprintf (buf, "0Announce/%s/%s", path, STR_ANN_INDEX);
	fp = fopen(buf, "rt");
	if (!fp)
	{
		return -1;
	}
	i = 0;
	skipthis = 0;
	memset(ai, 0, sizeof(struct AnnItem) * length);
	while (fgets(buf, sizeof(buf) - 1, fp))
	{
		if (*buf == '#')
		{
			continue;
		}
		else if (!strncmp(buf, STR_ANN_SNAME, strlen(STR_ANN_SNAME)))
		{
			if ((perm & PERM_ANN_HIDE) || strncmp(buf + 5, STR_ANN_HIDE, strlen(STR_ANN_HIDE)))
			{
				if (i >= start && i < start + length)
				{
					strncpy((ai + i - start)->title, buf + 5, 37);
					if (strlen(buf) > 43)
					{
						strncpy((ai + i - start)->owner, buf + 43, 36);
					}
					else
					{
						*(ai + i - start)->owner = 0;
					}
					(ai + i - start)->perm = perm;
				}
			}
			else
			{
				skipthis = 1;
			}
		}
		else if (!strncmp(buf, STR_ANN_FNAME, strlen(STR_ANN_FNAME)))
		{
			if (!skipthis)
			{
				if (i >= start && i < start + length)
				{
					strncpy((ai + i - start)->path, buf + 7, 40);
					for (p = (ai + i - start)->path; isalnum(*p) || *p == '.' || *p == '_'; p++);
					*p = 0;
					sprintf (buf2, "0Announce/%s/%s", path, (ai + i - start)->path);
					fileFlag (buf2, (ai + i - start));
				}
				i++;
			}
		}
	}
	fclose (fp);
	formalizeAI(ai, i);
	return i;
}

int parentDir(char * path, char * buf)
{
	char * p;
	strcpy (buf, path);
	p = buf + strlen(buf) - 1;
	if (p <= buf)
	{
		return -1;
	}
	//空串？=_-
	if (*p == '/')
	{
		*p = 0;
	}
	for (; p >= buf && *p != '/'; p--);
	if (p >= buf)
	{
		*p = 0;
		p++;
	}
	else
	{
		return -1;
	}
	return p - buf;
}

int dirouterTitle(char * path, struct AnnItem * ai)
{
	char buf[1024], buf2[1024];
	char * p;
	int r;
	r = parentDir(path, buf);
	//返回值是长度的说！
	if (r <= 0)
	{
		return 0;
	}
	p = path + r + 1;//这就是文件名了……
	strcpy (buf2, p);
	p = strchr(buf2, '/');
	if (p)
	{
		*p = 0;
	}
	p = buf2;
	r = annSeekFiles (buf, PERM_ANN_FULL, &p, 1, ai);
	if (r > 0)
	//注意！这里的perm没用的
	{
		ai->perm = PERM_ANN_READ;
	}
	return 1;
}

int dirouterAnn(char * path, int perm, struct AnnItem * ai, int start, int length)
{
	char buf[1024];
	if (parentDir(path, buf) <= 0)
	{
		return 0;
	}
	return item2Ann(buf, perm, ai, start, length); 
}

int pri_title_n_owner(struct AnnItem * ai, char * buf)//内部函数--_--
{
	return sprintf (buf, "%-38s%s", ai->title, ai->owner);
}

int createAnnIndex(char * path, struct AnnItem * thisdir, struct AnnItem * childitems, int childcnt)
//写一个.Names索引。其中path是不包含.Names的目录。
//大于等于零表示成功
{
	FILE * fp;
	int i;
	char buf[1024];
	formalizeAI (thisdir, 1);
	formalizeAI (childitems, childcnt);
	sprintf (buf, "0Announce/%s/%s", path, STR_ANN_INDEX);
	fp = fopen(buf, "wt");//直接覆盖死了=_=
	if (!fp)
	{
		return -1;
	}
	flock (fileno(fp), LOCK_EX);//加锁。
	//写头
	fprintf (fp, "#\n");
	pri_title_n_owner (thisdir, buf);
	fprintf (fp, "%s%s\n", STR_ANN_TITLE, buf);
	fprintf (fp, "#\n");
	//写body
	for (i = 0; i < childcnt; i++)
	{
		pri_title_n_owner (childitems + i, buf);
		fprintf (fp, "%s%s\n", STR_ANN_SNAME, buf);
		fprintf (fp, "%s~/%s\n", STR_ANN_FNAME, (childitems + i)->path);
		fprintf (fp, "%s%d\n", STR_ANN_NUMB, i + 1);
		fprintf (fp, "#\n");
	}
	flock (fileno(fp), LOCK_UN);
	fclose (fp);
	return 0;
}

int appendAnnIndex(char * path, struct AnnItem * ai)
{
	FILE * fp;
	char buf[1024];
	struct AnnItem a;
	sprintf (buf, "0Announce/%s/%s", path, STR_ANN_INDEX);
	fp = fopen(buf, "at");
	if (!fp)
	//无法打开文件的情况。可能是空目录？
	{
		fp = fopen(buf, "wt");//尝试新建
		if (!fp)
		{
			return -1;
		}
	}
	if (ftell(fp) == 0)//文件位于开始位置...
	{
		if (dirouterTitle(path, &a) <= 0)
		{
			///不知道叫什么-_-!
			return -1;
		}
		//新建那个头
		fprintf (fp, "#\n");
		fprintf (fp, "%s", STR_ANN_TITLE);
		pri_title_n_owner (&a, buf);
		fprintf (fp, "%s\n", buf);
		fprintf (fp, "#\n");
	}
	flock (fileno(fp), LOCK_EX);
	formalizeAI(ai, 1);
	pri_title_n_owner (ai, buf);
	fprintf (fp, "%s%s\n", STR_ANN_SNAME, buf);
	fprintf (fp, "%s~/%s\n", STR_ANN_FNAME, ai->path);
	fprintf (fp, "%s\n", STR_ANN_NUMB);
	fprintf (fp, "#\n");
	flock (fileno(fp), LOCK_UN);
	fclose (fp);
	return 0;
}

int annCounter(char * path)
{
	char buf[1024];
	FILE * fp;
	int i;
	sprintf (buf, "0Announce/%s/%s", path, STR_ANN_COUNTER);
	fp = fopen(path, "rt+");
	//这种破文件没必要锁定吧？
	if (!fp)
	{
		fp = fopen (path, "wt");
		if (!fp)
		{
			return -1;
		}
		i = 0;
	}
	else
	{
		fscanf (fp, "%d", &i);
		fseek (fp, 0, 0);
	}
	i++;
	fprintf (fp, "%d", i);
	fclose (fp);
	return i;
}
