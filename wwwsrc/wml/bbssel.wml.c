//bbssel.wml.c
//huangxu@nkbbs.org

//选择板面用
//新：根据版面内容列出待选版面
//请调试参数以便更好运行

#include "BBSLIB.inc"
#include "WMLLIB.inc"

#define WMLP_BR_MAXRB 20//返回结果的个数
#define WMLP_BR_FHBLEN 2048 
//telnet下的话，查找用户按一下空格要多少内存。我且用1M。
//以下是权重
#define WMLP_BR_MINPOST 0x400 //最少文章数
#define WMLP_BR_ENM 0x1000000//英文名匹配
#define WMLP_BR_ENS 0x400000//英文名开头
#define WMLP_BR_ENO 0x100000//英文名被包含
#define WMLP_BR_ENI 0x10000//英文名包含
#define WMLP_BR_CNM 0x1000000//中文名匹配
#define WMLP_BR_CNS 0x800000//中文名开头
#define WMLP_BR_CNO 0x200000//中文名被包含
#define WMLP_BR_CNI 0x100000//中文名包含
#define WMLP_BR_ARRATE 0x100//文章包含系数
#define WMLP_BR_ARN 2 //文章标题包含
#define WMLP_BR_ARR 1 //文章回复包含
#define WMLP_BR_ARW (WMLP_BR_ARN * 2) //精华
#define WMLP_BR_ARG (WMLP_BR_ARN * 8)//G标记，+8
#define WMLP_BR_ARM (WMLP_BR_ARN * 16)//M标记，+16。也就是说，B标记的话是24
#define WMLP_BR_RCTRNG (86400 * 7) //时间区段长度
#define WMLP_BR_RCTCNT 1 //时间区段个数
//以下是根据服务器负荷采取的措施。其数据是负荷*100
#define WMLP_BR_SRVBUSY 200 //服务器忙，只搜索一个区段
#define WMLP_BR_SRVFULL 800 //服务器快不行了，不搜索文章

struct BoardRank
{
	struct shortfile * board;
	int point;
};

time_t now;

int keywordFilter(char * kw)
//关键字预处理
//只做了trim而已
{
	char buf[256];
	char *p, *q;
	strncpy (buf, kw, 255);
	for (p = buf; *p >= 0 && *p <= ' ' && p - buf < 255; p++);
	if (p - buf < 255)
	{
		if (p > buf)
		{
			*(p - 1) = 0;
		}
	}
	else
	{
		*kw = 0;
		return 0;
	}
	for (q = p + strlen(p) - 1; q >= p && (*q < 0 || *q > ' '); q--);
	if (q >= p)
	{
		*q = 0;
	}
	strcpy (kw, p);
	return strlen(kw);
}

inline int brpost(const char * kw, struct shortfile * x, int mode)
//mode决定了取舍
//0--读取全部
//1--只读一个区块
//这里的板面都是可读的
{
	struct fileheader fh[WMLP_BR_FHBLEN];
	unsigned int pnt;
	unsigned int c, pc;
	unsigned int i, pp;
	time_t t;
	FILE * fp;
	char buf[256];
	pnt = 0;
	pc = 0;
	sprintf (buf, "boards/%s/.DIR", x->filename);
	fp = fopen(buf, "rb");
	if (!fp)
		//:"(
	{
		return 0;
	}
	if (mode)//只要部分
	{
		fseek (fp, 0, 2);//到结尾
		fseek (fp, - sizeof(struct fileheader) * WMLP_BR_FHBLEN * mode, 1);
		//这样好么？
	}
	while (c = fread(fh, sizeof(struct fileheader), WMLP_BR_FHBLEN, fp))//这里判断不为0就好了
	{
		pc += c;
		for (i = 0; i < c; i++)
		{
			if (strstr((fh + i)->title, kw))//包含关键字的处理
			{
				pp = strncmp((fh + i)->title, "Re: ", 3) ? WMLP_BR_ARN : WMLP_BR_ARR;//初始计分
				if ((fh + i)->accessed[0] & FILE_DIGEST)
					//G标记
				{
					pp += WMLP_BR_ARG;
				}
				if ((fh + i)->accessed[0] & FILE_VISIT)
					//收入精华区
				{
					pp += WMLP_BR_ARW;
				}
				if ((fh + i)->accessed[0] & FILE_MARKED)
					//M标记
				{
					pp += WMLP_BR_ARM;
				}
				t = (now - atoi((fh + i)->filename + 2) / WMLP_BR_RCTRNG);
				if (t < WMLP_BR_RCTCNT)
				{
					pp += WMLP_BR_RCTCNT - t;
				}
				pnt += pp;
			}
		}
		if (c < WMLP_BR_FHBLEN)
		{
			break;
		}
	}
	fclose (fp);
	if (pc < WMLP_BR_MINPOST)
	{
		pc = WMLP_BR_MINPOST;
	}
	return (pnt * WMLP_BR_ARRATE + pc / 2) / pc;
}

inline int brinsert(struct BoardRank * br, int brc, int bru, struct shortfile * x, int pnt)
//因为数量比较少，所以我觉得直接插入排序也未必慢。
//返回1表示个数增加一个。
//0表示个数没有增加，但并不是说没有插入
{
	int i;
	//这里就不再次判断0的问题了
	if (!bru) //空的情况。直接插入。
	{
		br->board = x;
		br->point = pnt;
		return 1;
	}
	if (pnt < (br + bru)->point)
		//经常发生的——末尾
	{
		return 0;
	}
	for (i = 0; i < bru; i++)//bru一定小于等于brc的
	{
		if (pnt > (br + i)->point)
		{
			memmove(br + i + 1, br + i, sizeof(struct BoardRank) * (brc - i - 1));
			//偷懒个-_-b
			(br + i)->board = x;
			(br + i)->point = pnt;
			return (bru < brc) ? 1 : 0;
				//这个时候，如果没有满就加1
		}
	}
	return 0;
	//Never here
}

int brcore(const char * kw, struct BoardRank * br, int brc)
{
	int bru, kwl, l;
	int i, tmp;
	int sysload;
	struct shortfile * x;
	char buf[1024];
	char * p, * cnn;
	FILE * fp;
	int pnt;
	now = time(NULL);
	kwl = strlen(kw);
	fp = fopen ("/proc/loadavg", "rt");
	if (fp)
	{
		*buf = 0;
		fgets (buf, 10, fp);
		sysload = (int)(atof(buf) * 100);
		fclose (fp);
	}
	else
	{
		sysload = 0;
	}
	bru = 0;
	memset (br, 0, sizeof(struct BoardRank) * brc);
	for(i = 0; i < MAXBOARD; i++) 
	{
		x = &(shm_bcache->bcache[i]);
		if(x->filename[0]<=32 || x->filename[0]>'z')
		{
			continue;
		}
		if(!has_read_perm(&currentuser, x->filename))
		{
			continue;
		}
		pnt = 0;
		l = strlen(x->filename);
		if (l >= kwl) //板面名称较长
		{
			p = strcasestr(x->filename, kw);
			if (p == x->filename)
			{
				if (l == kwl)//完全相等
				{
					pnt += WMLP_BR_ENM;
				}
				else//板面名称以kw开头
				{
					pnt += WMLP_BR_ENS;
				}
			}
			else if (p)//找到了，板面名称包含kw
			{
				pnt += WMLP_BR_ENI;
			}
		}
		else //关键字比较长
		{
			if (strcasestr(kw, x->filename))
			{
				pnt += WMLP_BR_ENO;
			}
		}
		cnn = x->title + 11;
		l = strlen(cnn);
		//这里比较中文名字
		if (l >= kwl) //板面名称较长
		{
			p = strcasestr(cnn, kw);
			if (p == cnn)
			{
				if (l == kwl)//完全相等
				{
					pnt += WMLP_BR_CNM;
				}
				else//板面名称以kw开头
				{
					pnt += WMLP_BR_CNS;
				}
			}
			else if (p)//找到了，板面名称包含kw
			{
				pnt += WMLP_BR_CNI;
			}
		}
		else //关键字比较长
		{
			if (strcasestr(kw, cnn))
			{
				pnt += WMLP_BR_CNO;
			}
		}
		if (sysload < WMLP_BR_SRVFULL)//小于才会处理板面文章
		{
			l = (sysload < WMLP_BR_SRVBUSY) ? 0 : 1;
			pnt += brpost(kw, x, l);
		}
		if (pnt)
		{
			bru += brinsert(br, brc, bru, x, pnt);
		}
	}
	return bru;
}

int bbssel_act(char * info)
{
	char kw[256];
	char buf[1024];
	struct BoardRank br[WMLP_BR_MAXRB];
	int bru, i;
	strncpy (kw, getparm("goto"), 255);
	keywordFilter(kw);
	if (strlen(kw) < 2)
	{
		strcpy (info, "关键字至少要两个字节。");
		return -1;
	}
	printf ("<card title=\"带我去 -- %s\">", BBSNAME);
	w_hsprintf(buf, "%s", kw);
	printf ("<p>关键字：%s</p>", buf);
	bru = brcore (kw, br, WMLP_BR_MAXRB);
	if (bru)//找到了
	{
		printf ("<p><em>共找到%d个结果</em><br />", bru);
		for (i = 0; i < bru; i++)
		{
			w_hsprintf(buf, "%s/%s", (br + i)->board->title + 11, (br + i)->board->filename);
			printf ("#%d <anchor><go href=\"%s.wml?board=%s\" />%s</anchor><br />", i + 1, ((br + i)->board->flag & ZONE_FLAG) ? "bbsboa" : "bbsdoc", (br + i)->board->filename, buf);
		}
		printf ("</p>");
	}
	else //没找到
	{
		printf ("<p>很抱歉，没有找到相关内容：（</p>");
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbssel_act(buf))
	{
		printf ("<card title=\"错误 -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><input name=\"inp_goto\" /><anchor><go href=\"bbssel.wml\" method=\"post\"><postfield name=\"goto\" value=\"$(inp_goto)\" /></go>带我去</anchor></p>");
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}


