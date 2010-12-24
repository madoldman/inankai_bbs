#include "BBSLIB.inc"

#define USFILE "etc/unionsites.dat"
#define EXTAUTHTRACE "extauth.trace"

typedef int TOKEN;

typedef struct _UNIONSITE //512B
{
	TOKEN token; //4B
	char host[16]; //16B
	char reqstr[256]; //256B
	char reserved[236]; //236B
} UNIONSITE;

int tokenequal(TOKEN * t1, TOKEN * t2)
{
	return (*t1 == *t2);
}

int checktoken(TOKEN * token)
{
	UNIONSITE us;
	if (gettoken(fromhost, &us))
	{
		return 0;
	}
	if (!tokenequal(&us.token, token))
	{
		return 0;
	}
	return -1;
}

int gettoken(char * host, UNIONSITE * pus)
{
	FILE * fp;
	int i;
	fp = fopen(USFILE, "r");
	if (!fp)
	{
		return -1;
	}
	i = 0;
	while (fread(pus, sizeof(UNIONSITE), 1, fp))
	{
		if (!strcmp(host, pus->host))
		{
			return i;
		}
		i++;
	}
	fclose (fp);
	return -2;
}

int gentoken(TOKEN * token)
{
	*token = (rand() << 16) ^ rand();
	return 0;
}

int str2token(char * str, TOKEN * token)
{
	*token = atoi(str);
	return 0;
}

int token2str(TOKEN * token, char * str, int size)
{
	return snprintf(str, size - 1, "%d", *token);
}

int extauth(char * buf, int size, char * id, char * pw, char * extinfo, TOKEN * token)
{
	char home[256], filename[256];
	char tmp[256];
	struct userec *x;
	int edumail;
	getdatestring(time(0), NA);
	if (!checktoken(token))
	{
		sleep(5);
		strncpy(buf, "<info>Token Error.</info>", size - 1);
		sprintf(tmp, "[TE] %-30s [%s] %s\n", datestring, fromhost, extinfo);
		f_append(EXTAUTHTRACE, tmp);
		return -4;
	}
	x = getuser(id);
	if (!x)
	{
		strncpy (buf, "<info>No such ID.</info>", size - 1);
		sprintf(tmp, "[NI] %-30s [%s] %s\n", datestring, fromhost, extinfo);
		f_append(EXTAUTHTRACE, tmp);
		return -1;
	}
	if(!checkpasswd(x->passwd, pw)) 
	{
		if(*pw)
		{
			sleep(2);
			strncpy(buf, "<info>Please input password.</info>", size - 1);
			return -2;
		}
		sleep(5);
		sprintf(tmp, "%-12.12s  %-30s [%s] %s\n", id, datestring, fromhost, extinfo);
		sprintf(filename, "home/%c/%s/logins.bad", toupper(x->userid[0]), x->userid);
		f_append(filename, tmp);
		f_append(EXTAUTHTRACE, tmp);
		strncpy(buf, "<info>Password error.</info>", size - 1);
		return -3;
	}
	sprintf(tmp, "home/%c/%s/.webauth.done", toupper(x->userid[0]), x->userid, home);
	edumail = file_exist(tmp);
	uleveltochar(tmp, x->userlevel);
	snprintf(buf, size - 1, "<userid>%s</userid><powerstr>%s</powerstr><edumail>%d</edumail>", x->userid, tmp);
	return 0;
}

int dropextchar(char * buf)
{
	char * p;
	for (p = buf; *p; p++)
	{
		if (!(*p & 0xe0))
		{
			*p = ' ';
		}
	}
	return 0;
}

int webact()
{
	char buf[1024];
	char extinfo[32];
	char id[16], pw[16];
	TOKEN token;
	int result;
	strncpy(id, getparm("id"), sizeof(id) - 1);
	strncpy(pw, getparm("pw"), sizeof(pw) - 1);
	strncpy(extinfo, getparm("ext"), sizeof(extinfo));
	if (!*(getparm("token")))
	{
		return 0;
	}
	str2token(getparm("token"), &token);
	dropextchar(extinfo);
	result = extauth(buf, sizeof(buf) - 1, id, pw, extinfo, &token);
	printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\n");
	printf("<authinfo>");
	printf("<result>%d</result>", result);
	printf("%s", buf);
	printf("</authinfo>");
	return result;
}

int addsite(char * host, char * reqstr)
{
	UNIONSITE us, usb;
	FILE * fp;
	memset(&us, 0, sizeof(us));
	strncpy(us.host, host, sizeof(us.host) - 1);
	strncpy(us.reqstr, reqstr, sizeof(us.reqstr) - 1);
	gentoken(&us.token);
	if (!(fp = fopen(USFILE, "r+")))
	{
		if (!(fp = fopen(USFILE, "w+")))
		{
			return -1;
		}
	}
	while (fread(&usb, sizeof(UNIONSITE), 1, fp))
	{
		if (!strcmp(usb.host, us.host))
		{
			fclose(fp);
			return -2;
		}
	}
	fwrite(&us, sizeof(UNIONSITE), 1, fp);
	tellsite(&us);
	fclose (fp);
	return 0;
}

int delsite(char * host)
{
	UNIONSITE us;
	int pos;
	pos = gettoken(host, &us);
	//注意这里的延时问题 仅仅是因为用途限制所以不会出错
	if (pos >= 0)
	{
		del_record(USFILE, sizeof(UNIONSITE), pos);
		return 0;
	}
	else
	{
		return -1;
	}
}

int listsites()
{
	UNIONSITE us;
	FILE * fp;
	int i;
	char buf[256];
	if (!(fp = fopen(USFILE, "r")))
	{
		printf ("Can not open the site file.\n");
		return -1;
	}
	i = 0;
	printf("List of sites.\n");
	while (fread(&us, sizeof(UNIONSITE), 1, fp))
	{
		token2str(&us.token, buf, sizeof(buf));
		printf("ID=%d, HOST=%s, REQSTR=%s, TOKEN=%s\n", i, us.host, us.reqstr, buf);
		i++;
	}
	printf("Total: %d site(s).\n", i);
	fclose(fp);
}

int tellsite(UNIONSITE * us)
{
	char buf[1024], *p;
	char buf1[256], buf2[2048];
	strcpy(buf, us->reqstr);
	for (p = buf; *p; p++)
	{
		if (strchr("\\\'\"", *p) || !(*p & 0xe0))
		{
			*p = '+';
		}
	}
	token2str(&us->token, buf1, sizeof(buf1));
	p = strstr(buf, "%s");
	*p = 0;
	sprintf(buf2, "wget '%s%s%s' -O /dev/null -q", buf, buf1, p + 2);
	return system(buf2);
}

int updatetokens()
{
	UNIONSITE us;
	FILE * fp;
	int i;
	char buf[256];
	if (!(fp = fopen(USFILE, "r+")))
	{
		printf ("Can not open the site file.\n");
		return -1;
	}
	//同时至多有一个进程在写，无须锁定
	//flock(fileno(fp), LOCK_EX);
	i = 0;
	printf("Updating tokens...\n");
	while (fread(&us, sizeof(UNIONSITE), 1, fp))
	{
		gentoken(&us.token);
		token2str(&us.token, buf, sizeof(buf));
		printf("ID=%d, HOST=%s, REQSTR=%s, TOKEN=%s\n", i, us.host, us.reqstr, buf);
		tellsite(&us);
		fseek(fp, - sizeof(UNIONSITE), 1);
		fwrite(&us, sizeof(UNIONSITE), 1, fp);
		i++;
	}
	printf("Total: %d site(s).\n", i);
	//flock(fileno(fp), LOCK_UN);
	fclose(fp);
}

int main(int argc, char ** argv)
{
	int conmode;
	xml_init();
	conmode = !*(getsenv("HTTP_HOST"));
	if (!conmode)
	{
		return webact();
	}
	if (argc < 2)
	{
		printf("USAGE: %s -a HOST REQSTR\n", *argv);
		printf("USAGE: %s -d HOST\n", *argv);
		printf("USAGE: %s -l\n", *argv);
		printf("USAGE: %s -u\n", *argv);
		return -1;
	}
	if (!strcmp(*(argv + 1), "-a"))
		//Add a site
	{
		if (argc < 4)
		{
			printf("USAGE: %s -a HOST REQSTR\n", *argv);
			return -1;
		}
		if (addsite(*(argv + 2), *(argv + 3)))
		{
			printf("The site is added successfully.\n");
		}
		else
		{
			printf("Error when adding site.\n");
		}
	}
	else if (!strcmp(*(argv + 1), "-d"))
		//Delete site(s)
	{
		if (argc < 3)
		{
			printf("USAGE: %s -d HOST\n", *argv);
			return -1;
		}
		if (delsite(*(argv + 2)))
		{
			printf("The site is added successfully.\n");
		}
		else
		{
			printf("Error when deleting site.\n");
		}
	}
	else if (!strcmp(*(argv + 1), "-l"))
		//List sites
	{
		listsites();
	}
	else if (!strcmp(*(argv + 1), "-u"))
		//Update tokens
	{
		updatetokens();
	}
	return 0;
}


