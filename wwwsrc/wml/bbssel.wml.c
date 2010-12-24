//bbssel.wml.c
//huangxu@nkbbs.org

//ѡ�������
//�£����ݰ��������г���ѡ����
//����Բ����Ա��������

#include "BBSLIB.inc"
#include "WMLLIB.inc"

#define WMLP_BR_MAXRB 20//���ؽ���ĸ���
#define WMLP_BR_FHBLEN 2048 
//telnet�µĻ��������û���һ�¿ո�Ҫ�����ڴ档������1M��
//������Ȩ��
#define WMLP_BR_MINPOST 0x400 //����������
#define WMLP_BR_ENM 0x1000000//Ӣ����ƥ��
#define WMLP_BR_ENS 0x400000//Ӣ������ͷ
#define WMLP_BR_ENO 0x100000//Ӣ����������
#define WMLP_BR_ENI 0x10000//Ӣ��������
#define WMLP_BR_CNM 0x1000000//������ƥ��
#define WMLP_BR_CNS 0x800000//��������ͷ
#define WMLP_BR_CNO 0x200000//������������
#define WMLP_BR_CNI 0x100000//����������
#define WMLP_BR_ARRATE 0x100//���°���ϵ��
#define WMLP_BR_ARN 2 //���±������
#define WMLP_BR_ARR 1 //���»ظ�����
#define WMLP_BR_ARW (WMLP_BR_ARN * 2) //����
#define WMLP_BR_ARG (WMLP_BR_ARN * 8)//G��ǣ�+8
#define WMLP_BR_ARM (WMLP_BR_ARN * 16)//M��ǣ�+16��Ҳ����˵��B��ǵĻ���24
#define WMLP_BR_RCTRNG (86400 * 7) //ʱ�����γ���
#define WMLP_BR_RCTCNT 1 //ʱ�����θ���
//�����Ǹ��ݷ��������ɲ�ȡ�Ĵ�ʩ���������Ǹ���*100
#define WMLP_BR_SRVBUSY 200 //������æ��ֻ����һ������
#define WMLP_BR_SRVFULL 800 //�������첻���ˣ�����������

struct BoardRank
{
	struct shortfile * board;
	int point;
};

time_t now;

int keywordFilter(char * kw)
//�ؼ���Ԥ����
//ֻ����trim����
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
//mode������ȡ��
//0--��ȡȫ��
//1--ֻ��һ������
//����İ��涼�ǿɶ���
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
	if (mode)//ֻҪ����
	{
		fseek (fp, 0, 2);//����β
		fseek (fp, - sizeof(struct fileheader) * WMLP_BR_FHBLEN * mode, 1);
		//������ô��
	}
	while (c = fread(fh, sizeof(struct fileheader), WMLP_BR_FHBLEN, fp))//�����жϲ�Ϊ0�ͺ���
	{
		pc += c;
		for (i = 0; i < c; i++)
		{
			if (strstr((fh + i)->title, kw))//�����ؼ��ֵĴ���
			{
				pp = strncmp((fh + i)->title, "Re: ", 3) ? WMLP_BR_ARN : WMLP_BR_ARR;//��ʼ�Ʒ�
				if ((fh + i)->accessed[0] & FILE_DIGEST)
					//G���
				{
					pp += WMLP_BR_ARG;
				}
				if ((fh + i)->accessed[0] & FILE_VISIT)
					//���뾫����
				{
					pp += WMLP_BR_ARW;
				}
				if ((fh + i)->accessed[0] & FILE_MARKED)
					//M���
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
//��Ϊ�����Ƚ��٣������Ҿ���ֱ�Ӳ�������Ҳδ������
//����1��ʾ��������һ����
//0��ʾ����û�����ӣ���������˵û�в���
{
	int i;
	//����Ͳ��ٴ��ж�0��������
	if (!bru) //�յ������ֱ�Ӳ��롣
	{
		br->board = x;
		br->point = pnt;
		return 1;
	}
	if (pnt < (br + bru)->point)
		//���������ġ���ĩβ
	{
		return 0;
	}
	for (i = 0; i < bru; i++)//bruһ��С�ڵ���brc��
	{
		if (pnt > (br + i)->point)
		{
			memmove(br + i + 1, br + i, sizeof(struct BoardRank) * (brc - i - 1));
			//͵����-_-b
			(br + i)->board = x;
			(br + i)->point = pnt;
			return (bru < brc) ? 1 : 0;
				//���ʱ�����û�����ͼ�1
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
		if (l >= kwl) //�������ƽϳ�
		{
			p = strcasestr(x->filename, kw);
			if (p == x->filename)
			{
				if (l == kwl)//��ȫ���
				{
					pnt += WMLP_BR_ENM;
				}
				else//����������kw��ͷ
				{
					pnt += WMLP_BR_ENS;
				}
			}
			else if (p)//�ҵ��ˣ��������ư���kw
			{
				pnt += WMLP_BR_ENI;
			}
		}
		else //�ؼ��ֱȽϳ�
		{
			if (strcasestr(kw, x->filename))
			{
				pnt += WMLP_BR_ENO;
			}
		}
		cnn = x->title + 11;
		l = strlen(cnn);
		//����Ƚ���������
		if (l >= kwl) //�������ƽϳ�
		{
			p = strcasestr(cnn, kw);
			if (p == cnn)
			{
				if (l == kwl)//��ȫ���
				{
					pnt += WMLP_BR_CNM;
				}
				else//����������kw��ͷ
				{
					pnt += WMLP_BR_CNS;
				}
			}
			else if (p)//�ҵ��ˣ��������ư���kw
			{
				pnt += WMLP_BR_CNI;
			}
		}
		else //�ؼ��ֱȽϳ�
		{
			if (strcasestr(kw, cnn))
			{
				pnt += WMLP_BR_CNO;
			}
		}
		if (sysload < WMLP_BR_SRVFULL)//С�ڲŻᴦ���������
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
		strcpy (info, "�ؼ�������Ҫ�����ֽڡ�");
		return -1;
	}
	printf ("<card title=\"����ȥ -- %s\">", BBSNAME);
	w_hsprintf(buf, "%s", kw);
	printf ("<p>�ؼ��֣�%s</p>", buf);
	bru = brcore (kw, br, WMLP_BR_MAXRB);
	if (bru)//�ҵ���
	{
		printf ("<p><em>���ҵ�%d�����</em><br />", bru);
		for (i = 0; i < bru; i++)
		{
			w_hsprintf(buf, "%s/%s", (br + i)->board->title + 11, (br + i)->board->filename);
			printf ("#%d <anchor><go href=\"%s.wml?board=%s\" />%s</anchor><br />", i + 1, ((br + i)->board->flag & ZONE_FLAG) ? "bbsboa" : "bbsdoc", (br + i)->board->filename, buf);
		}
		printf ("</p>");
	}
	else //û�ҵ�
	{
		printf ("<p>�ܱ�Ǹ��û���ҵ�������ݣ���</p>");
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
		printf ("<card title=\"���� -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><input name=\"inp_goto\" /><anchor><go href=\"bbssel.wml\" method=\"post\"><postfield name=\"goto\" value=\"$(inp_goto)\" /></go>����ȥ</anchor></p>");
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}


