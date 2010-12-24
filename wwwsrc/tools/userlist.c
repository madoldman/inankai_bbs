//负责id列表的读写

#include "BBSLIB.inc"
#include "NEWLIB.inc"

#define USERSTRLEN 16384

struct ltuser
{
	char userid[16];
	struct ltuser * next;
};

char titlestr[128];

int getpath(char* path, int type, char* parm)
{
	char buf[256];
	int i;
	struct shortfile *x;
	*path = 0;
	switch(type)
	{
		case 0x1://群信--b权限
			if(!HAS_PERM(PERM_BASIC))
			{
				return 1;
				//权限不够
			}
			i = atoi(parm);
			if (i <= 1)
			{
				strcpy(buf, "maillist");
			}
			else
			{
				sprintf(buf, "maillist%d", i);
			}
			sethomefile(path, currentuser.userid, buf);
			sprintf(titlestr, "群信名单 %d", i);
			break;
		case 0x2://拒收信件名单
			if(!HAS_PERM(PERM_BASIC))
			{
				return 1;
				//权限不够
			}
			strcpy(buf, "deny_user");
			sethomefile(path, currentuser.userid, buf);
			sprintf(titlestr, "拒收信件名单");
			break;			
		case 0x101://封闭板面--斑竹
		case 0x102://半开放板面--斑竹
			x=getbcache(parm);
			if (!x)
			{
				return 2;
				//文件未找到
			}
			strcpy(parm, x->filename);
			if(!(has_read_perm(&currentuser, x->filename) && has_BM_perm(&currentuser, x->filename)))
			{
				return 1;
			}
			sprintf (buf, "boards/%s/board.%s", x->filename, (type&1)?"write":"allow");
			if (file_exist(buf))
			{
				return 3;
				//不能同时存在双名单
			}
			sprintf (buf, "boards/%s/board.%s", x->filename, (type&1)?"allow":"write");
			if (HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_OBOARDS)||file_exist(buf))
			{
				strcpy(path, buf);
			}
			else
			{
				return 3;
				//不能创建名单
			}
			if(type & 1)
			{
				sprintf(titlestr, "%s 进板名单", x->filename);
			}
			else
			{
				sprintf(titlestr, "%s 发文名单", x->filename);
			}
			break;
		case 0x201://看穿隐身名单--C权限
			if (!HAS_PERM(PERM_CLOAK))
			{
				return 1;
			}
			sethomefile(path, currentuser.userid, "mylover");
			sprintf(titlestr, "看穿我隐身的名单");
			break;
	}
	return 0;
}

int main()
{
	int file1;//类别
	char file2[256];//参数
	char path[512];
	char buf[USERSTRLEN], buf2[512];
	char *p, *p1;
	struct userec * x;
	struct ltuser * head, ** pnext, * curr;
	FILE *fp;
	xml_init();
	file1=atoi(getparm("file1"));
	strncpy(file2, getparm("file2"), 255);
	getpath(path, file1, file2);
	if (!strcmp(getparm("act"), "save"))
	{
		strncpy(buf, getparm("list"), USERSTRLEN - 1);
		pnext = &head;
		head = NULL;
		for(p = p1 = buf;;)
		{
			p = strchr(p1, ' ');
			if (p)
			{
				*p = 0;
			}
			x = getuser(p1);
			if (x && strlen(x->userid) > 0 && ((file1 != 0x101 && file1 != 0x102) || user_perm(x, PERM_BASIC) ))
			{
				for (curr = head; curr; curr = curr->next)
				{
					if (!strcmp(x->userid, curr->userid))
					{
						break;
					}
				}
				if (!curr)
				{
					*pnext = ((struct ltuser*)malloc(sizeof(struct ltuser)));
					(*pnext)->next = NULL;
					strcpy ((*pnext)->userid, x->userid);
					pnext = &((*pnext)->next);
				}
			}
			if (!p)
			{
				break;
			}
			else
			{
				p1 = ++p;
			}
		}
		if (!head)
		{
			unlink (path);
		}
		else
		{
			fp = fopen(path, "wt");
			if (fp)
			{
				for(curr = head; curr; curr = curr->next)
				{
					fprintf (fp, "%s\n", curr->userid);
				}
				fclose(fp);
				if (file1 == 0x102 || file1 == 0x101)
				{
					sprintf(buf, "tmp/%d.tmp", getpid());
					fp = fopen(buf, "w");//这里会失败？……
					sprintf(buf2, "编辑%s板名单", file2);
					fprintf(fp, "%s\n", buf2);
					getuinfo(fp);
					fclose(fp);
					post_article("syssecurity3", buf2, buf, currentuser.userid, currentuser.username, "自动发信系统", -1);
					unlink(buf);
				}
			}
		}
	}
	printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\n");
	printf("<userlist>\n");
	printf("<title>%s</title>\n", titlestr);
	fp = fopen(path, "rt");
	if (fp)
	{
		for(;!feof(fp);)
		{
			*buf = 0;
			fgets(buf, 255, fp);
			if (*buf)
			{
				printf ("<user>%s</user>\n", trim(buf));
			}
		}
		fclose(fp);
	}
	printf("</userlist>\n");
	return 0;
}

