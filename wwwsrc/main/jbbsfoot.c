#include "BBSLIB.inc"
#include "NEWLIB.inc"
//huangxu:用以取代bbsfoot.c

//url:/cgi-bin/bbs/jbbsfoot?ms=123
//bbsfoot.htm使用cookies msgfilelen保存原有的msg文件长度
//这个破cgi控制head不方便-_-b

int checkmsg(int ms)
{
	char buf[1024];
	char path[256];
	int len;
	FILE * fp;
	sethomefile(path, currentuser.userid, "msgfile.me");
	len = file_size(path);
	if (ms < 0 || ms > len)
	{
		ms = len;
	}
	printf("<newms>%d</newms>\n", len);
	if (len == ms)
	{
		return 0;
	}
	fp = fopen(path, "rt");
	if (!fp)
	{
		return 0;
	}
	fseek(fp, ms, 0);
	j_hsprintf(1, buf, "");
	printf("<msglist>");
	for(; fgets(buf, 1023, fp); feof(fp))
	{
		printf("<msg>");
		j_hprintf("%s", noansi(buf));
		printf("</msg>\n");
	}
	printf("</msglist>\n");
	fclose(fp);
	return len - ms;
}

int main()
{
	int ms, dt;
	int mail_total, mail_unread;
	char id[16];
	xml_init();
	if(loginok)
	{
		strcpy(id, currentuser.userid);
	}
	else
	{
		strcpy(id, "guest");
	}
	printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\n");
	ms = atoi(getparm("ms"));
	if (!*(getparm("ms")))
	{
		ms = -1;
	}
	printf("<bbsfoot>\n");
	printf("<userid>%s</userid>\n", id);
	getdatestring(time(0), NA);
	printf("<time>%s</time>\n", datestring);
	printf("<online>%d</online>\n", (HAS_PERM(PERM_SEECLOAK)?count_online():count_online2()));
	if (loginok)
	{
		count_mails(id, &mail_total,&mail_unread);
		printf("<newmail>%d</newmail>\n", mail_unread);
		dt=abs(time(0) - *(int*)(u_info->from+30))/60;
		u_info->idle_time=time(0);
		printf("<stay>%d小时%d分</stay>\n", dt/60, dt%60);
		checkmsg(ms);
	}
	printf("</bbsfoot>\n");
}

