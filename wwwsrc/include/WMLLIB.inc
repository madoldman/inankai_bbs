//WMLLIB.inc
//WML库
//huangxu@nkbbs.org
//BBSLIB.inc required

#ifdef CHARSET
	#undef CHARSET
	#define CHARSET "GBK"
#endif

int wml_init()
{
	int i = page_init("text/vnd.wap.wml");
#ifdef MUST_LOGIN
	if (!loginok && !strstr(getsenv("SCRIPT_NAME"), "log") && !strstr(getsenv("SCRIPT_NAME"), "index"))
	{
		wml_head();
		printf ("<anchor><go href=\"index.wml\">请登录后浏览.</go></anchor>");
		wml_tail();
		exit (0);
	}
#endif
	return i;
}

int wml_httpheader()
{
	printf("Content-type: text/vnd.wap.wml; charset=%s\n\n", CHARSET);
	return 0;
}

int wml_head()
{
	printf ("<?xml version=\"1.0\" encoding=\"%s\"?>\
<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.3//EN\" \"http://www.wapforum.org/DTD/wml13.dtd\">\
<wml>\
", CHARSET);
	return 0;
}

int wml_tail()
{
	printf ("%s", "\
</wml>\
");
	return 0;
}

int w_hsprintf(char *s, char *fmt, ...) 
{
	char src[512];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(src, 500, fmt, ap);
	va_end(ap);
	char xmlchar[16];
	int l=strlen(src);
	int i;
	int hz;
	char x,j;
	int charf = 0;
	int len;
	len = 0;
	for (i=0; i<l; i++)
	{
		x=*(src+i);
		if (x == 13 || x == 10)
		{
			;
		}
		else if (x == 27)
		{
			if (src[i+1]==91)//'['
			{
				i+=2;
				for(;;i++)
				{
					j=src[i];
					if (isalpha(j) || j == '@')
						break;//字母和@退出
				}
			}
		}
		else if (x == '$')
		{
			*(s + len) = '$';
			*(s + len + 1) = '$';
			len += 2;
		}
		else
		{
			if (charf || isalnum(x) || (x < 0 ))
			//GBK???!!!
			{
				*(s+len++)=x;
			}
			else
			{
				sprintf(xmlchar, "&#%d;", x);
				strnncpy(s, &len, xmlchar);
			}
			if (x < 0 )//是汉字
			{
				charf ^= 1;
			}
			else
			{
				charf = 0;
			}
		}
	}
	if (charf && len)
	{
		s[len - 1] = 0;
	}
	s[len] = 0;
	return len;
}

int w_hprintf(char *fmt, ...) {
	char buf[10000], buf2[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf2, 1023, fmt, ap);
	va_end(ap);
	w_hsprintf(buf, "%s", buf2);
	return printf("%s", buf);
}

int wml_bar(int mask)
{
	printf ("<p><anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor> ", currentuser.userid, currentuser.userid);
	if (loginok)
	{
		int dt, mail_total, mail_unread;
		count_mails(currentuser.userid, &mail_total, &mail_unread);
		dt=abs(time(0) - *(int*)(u_info->from+30))/60;
		u_info->idle_time=time(0);
		printf("%02d:%02d ", dt/60, dt%60);
		if (mail_unread)
		{
			printf ("<anchor><go href=\"bbsdoc.wml?board=*mail\" />%d信</anchor> ", mail_unread);
		}
	}
	printf ("<anchor><go href=\"bbsboa.wml\" />分区</anchor> ");
	printf ("<anchor><go href=\"bbsusr.wml\" />用户</anchor> ");
	printf ("<anchor><go href=\"bbs0an.wml\" />精华</anchor> ");
	if (loginok)
	{
		printf ("<anchor><go href=\"bbssettings.wml\" />设置</anchor> ");
	}
	printf ("<anchor><go href=\"logout.wml\" />注销</anchor></p>");
	return mask;
}

int wmlmode(struct user_info *x, int newmode) 
{
	if(x == 0)
	{
		return;
	}
	if (newmode < WML_MODE_OFFSET)
	{
		newmode += WML_MODE_OFFSET;
	}
	x->mode = newmode;
	return newmode;
}

int shorttime(char * buf, time_t t)
{
	time_t now;
	struct tm *tm;
	now = time(NULL);
	tm = localtime(&t);
	if (t < now - 3600 * 23 || t > now)
	{
		return sprintf(buf, "%02d-%02d", tm->tm_mon + 1, tm->tm_mday);
	}
	else
	{
		return sprintf(buf, "%02d:%02d", tm->tm_hour, tm->tm_min);
	}
}

int datetime(char * buf, time_t t)
{
	struct tm *tm;
	tm = localtime(&t);
	return sprintf(buf, "%02d-%02d.%02d:%02d", tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
}

int datestr(char * buf, time_t t)
{
	struct tm *tm;
	tm = localtime(&t);
	return sprintf(buf, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

int wmlUseridStr(char * buf, char * s)
{
	char buf2[256], tmp[256], *ptr, *ptr2;
	strsncpy(tmp, s, 255);
	*buf = 0;
	ptr = strtok(tmp, " ,();\r\n\t");
	while (ptr && strlen(buf)<400) 
	{
		if (ptr2 = strchr(ptr, '.')) 
		{
			ptr2[1] = 0;
			strcat(buf, ptr);
			strcat(buf, " ");
		}
		else
		{
			ptr = nohtml(ptr);
			if(strcmp(ptr, "SYSOPs") && !strchr(ptr, ':'))
			{
				sprintf(buf2, "<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor> ", ptr, ptr);
			}
			else
			{
				sprintf(buf2, "%s ", ptr);
			}
			strcat(buf, buf2);
		}
		ptr=strtok(0, " ,();\r\n\t");
	}
	return strlen(buf);
}

int readFileLine(char * buf, int bufsize, FILE * fp)
{
	*buf = 0;
	if (!fp)
	{
		return 0;
	}
	return !!fgets(buf, bufsize - 1, fp);
}

int skipHeader(FILE * fp)
{
	char buf[256];
	if (!fp)
	{
		return 0;
	}
	while (fgets(buf, 255, fp))
	{
		if (*buf >= 0 && *buf < ' ')
		{
			return ftell(fp);
		}
	}
	return -1;//fail
}

int wmlPostfields(char ** pf, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++)
	{
		wmlPostfield(*(pf + i));
	}
	return i;
}

int wmlPostfield(char * pf)
{
	printf ("<postfield name=\"%s\" value=\"$(inp_%s)\" />", pf, pf);
	return 0;
}
